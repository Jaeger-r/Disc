# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/DiskClientApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/DiskClientApp_autogen.dir/ParseCache.txt"
  "CMakeFiles/DiskClientSmokeTest_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/DiskClientSmokeTest_autogen.dir/ParseCache.txt"
  "DiskClientApp_autogen"
  "DiskClientSmokeTest_autogen"
  )
endif()
