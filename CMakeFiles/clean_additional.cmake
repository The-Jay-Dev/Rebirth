# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\appRebirth_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appRebirth_autogen.dir\\ParseCache.txt"
  "appRebirth_autogen"
  )
endif()
