#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_system.h>
#include <nvs_flash.h>

#include "camera_pins.h"
#include "connect_wifi.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"

static const char *TAG = "Camera Capture UDP Client";

#define CONFIG_XCLK_FREQ 20000000

static esp_err_t init_camera(void) {
  camera_config_t camera_config = {
      .pin_pwdn = CAM_PIN_PWDN,
      .pin_reset = CAM_PIN_RESET,
      .pin_xclk = CAM_PIN_XCLK,
      .pin_sccb_sda = CAM_PIN_SIOD,
      .pin_sccb_scl = CAM_PIN_SIOC,

      .pin_d7 = CAM_PIN_D7,
      .pin_d6 = CAM_PIN_D6,
      .pin_d5 = CAM_PIN_D5,
      .pin_d4 = CAM_PIN_D4,
      .pin_d3 = CAM_PIN_D3,
      .pin_d2 = CAM_PIN_D2,
      .pin_d1 = CAM_PIN_D1,
      .pin_d0 = CAM_PIN_D0,
      .pin_vsync = CAM_PIN_VSYNC,
      .pin_href = CAM_PIN_HREF,
      .pin_pclk = CAM_PIN_PCLK,

      .xclk_freq_hz = CONFIG_XCLK_FREQ,
      .ledc_timer = LEDC_TIMER_0,
      .ledc_channel = LEDC_CHANNEL_0,

      .pixel_format = PIXFORMAT_JPEG,
      .frame_size = FRAMESIZE_QVGA,

      .jpeg_quality = 12,
      .fb_count = 1,
      .grab_mode = CAMERA_GRAB_WHEN_EMPTY}; // CAMERA_GRAB_LATEST. Sets when
                                            // buffers should be filled
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    return err;
  }
  return ESP_OK;
}
// Helper: write uint32_t in little-endian
static void write_uint32_le(uint8_t *buf, uint32_t value) {
  buf[0] = (uint8_t)(value & 0xFF);
  buf[1] = (uint8_t)((value >> 8) & 0xFF);
  buf[2] = (uint8_t)((value >> 16) & 0xFF);
  buf[3] = (uint8_t)((value >> 24) & 0xFF);
}

void udp_stream_task(void *pvParameters) {
  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(UDP_SERVER_PORT);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    vTaskDelete(NULL);
    return;
  }
  ESP_LOGI(TAG, "UDP socket created, sending to %s:%d", UDP_SERVER_IP,
           UDP_SERVER_PORT);

  while (1) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      ESP_LOGE(TAG, "Camera capture failed");
      continue;
    }

    // Send 4-byte frame length first
    uint8_t size_buf[4];
    write_uint32_le(size_buf, fb->len);

    int sent = sendto(sock, size_buf, sizeof(size_buf), 0,
                      (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent < 0) {
      ESP_LOGE(TAG, "error sending frame size: errno %d", errno);
      esp_camera_fb_return(fb);
      continue;
    }

    // Send JPEG data in chunks
    size_t remaining = fb->len;
    uint8_t *buf_ptr = fb->buf;

    while (remaining > 0) {
      size_t chunk_size =
          remaining > UDP_CHUNK_SIZE ? UDP_CHUNK_SIZE : remaining;

      sent = sendto(sock, buf_ptr, chunk_size, 0, (struct sockaddr *)&dest_addr,
                    sizeof(dest_addr));
      if (sent < 0) {
        ESP_LOGE(TAG, "Error sending chunk: errno %d", errno);
        break;
      }

      buf_ptr += chunk_size;
      remaining -= chunk_size;
    }

    ESP_LOGI(TAG, "Sent frame: %u bytes", fb->len);

    esp_camera_fb_return(fb);

    vTaskDelay(pdMS_TO_TICKS(30)); // ~33 FPS limit
  }

  close(sock);
  vTaskDelete(NULL);
}

void app_main() {
  esp_err_t err;

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  connect_wifi();

  if (wifi_connect_status) {
    err = init_camera();
    if (err != ESP_OK) {
      printf("err: %s\n", esp_err_to_name(err));
      return;
    }
    xTaskCreatePinnedToCore(udp_stream_task, "udp_stream_task", 4096, NULL, 5,
                            NULL,
                            tskNO_AFFINITY); // Create UDP stream task
  } else
    ESP_LOGI(
        TAG,
        "Failed to connected with Wi-Fi, check your network Credentials\n");
}
