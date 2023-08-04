# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/marcus/esp/esp-idf/components/bootloader/subproject"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/tmp"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/src/bootloader-stamp"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/src"
  "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marcus/Documents/esp32_tempsensor/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
