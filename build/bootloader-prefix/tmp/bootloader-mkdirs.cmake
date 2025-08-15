# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/gandhi/esp/esp-idf/components/bootloader/subproject"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/tmp"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/src/bootloader-stamp"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/src"
  "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/gandhi/rimtex_robotics/firmware/canbot_camera/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
