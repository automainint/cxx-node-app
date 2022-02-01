cmake_minimum_required(VERSION 3.16)

set(THIRDPARTY_PREFIX "" CACHE STRING "Third-party prefix")
set(NODEJS_FIND_PATH ${THIRDPARTY_PREFIX}/build/src/node)

project(nodeapp CXX)

if("${CMAKE_STATIC_LIBRARY_PREFIX_CXX}${CMAKE_STATIC_LIBRARY_SUFFIX_CXX}" STREQUAL "")
  if(MSVC)
    set(CMAKE_STATIC_LIBRARY_PREFIX_CXX "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX_CXX ".lib")
  else()
    set(CMAKE_STATIC_LIBRARY_PREFIX_CXX "lib")
    set(CMAKE_STATIC_LIBRARY_SUFFIX_CXX ".a")
  endif()
endif()

file(
  GLOB_RECURSE NODEJS_LIBS
  ${NODEJS_FIND_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX_CXX}*${CMAKE_STATIC_LIBRARY_SUFFIX_CXX})

add_library(node-all INTERFACE)

foreach(_lib ${NODEJS_LIBS})
  get_filename_component(_name ${_lib} NAME_WE)
  string(REGEX REPLACE "^lib" "" _name ${_name})

  add_library(${_name} STATIC IMPORTED GLOBAL)
  set_target_properties(${_name} PROPERTIES IMPORTED_LOCATION ${_lib})

  target_link_libraries(node-all INTERFACE ${_name})

  message(STATUS "Found ${_name}: ${_lib}")
endforeach()

add_executable(nodeapp main.cpp)

target_include_directories(nodeapp PRIVATE ../thirdparty/include/node)

find_package(Threads REQUIRED)
target_link_libraries(nodeapp PRIVATE Threads::Threads)

if(UNIX AND NOT APPLE)
  target_link_libraries(nodeapp PRIVATE -ldl)
endif()

target_link_libraries(nodeapp PRIVATE node-all)
