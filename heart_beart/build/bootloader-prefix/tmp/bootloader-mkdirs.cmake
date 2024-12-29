# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/IT/IoT/ESP32/ESP32-IDF/Espressif/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/tmp"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/src/bootloader-stamp"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/src"
  "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/vi_xu_ly/heart_beart - Copy/heart_beart/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
