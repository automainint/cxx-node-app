cmake_minimum_required(VERSION 3.16)

set(NODEJS_TAG "v17.4.0" CACHE STRING "Node.js git tag")

set(NODEAPP_PROJECT nodeapp)
set(NODEAPP_EXE     nodeapp)
set(NODEAPP_OBJ     nodeapp-obj)

set(NODEJS_PREFIX ${CMAKE_SOURCE_DIR}/thirdparty/build)

project(
  ${NODEAPP_PROJECT}
    VERSION     0.1.1
    DESCRIPTION "C++ application with embedded Node.js example."
    LANGUAGES   CXX)

include(ExternalProject)

ExternalProject_Add(
  node
  GIT_REPOSITORY    https://github.com/nodejs/node.git
  GIT_TAG           ${NODEJS_TAG}
  PREFIX            ${NODEJS_PREFIX}
  BUILD_IN_SOURCE   ON
  CONFIGURE_COMMAND ./configure --enable-static --prefix=${CMAKE_SOURCE_DIR}/thirdparty
  BUILD_COMMAND     make -j4
  INSTALL_COMMAND   make install)

find_library(
  NODEJS_LIBRARY
  NAMES node libnode.a node.lib
  PATHS
    "${NODEJS_PREFIX}/src/node/Release"
    "${NODEJS_PREFIX}/src/node/out/Release"
    "${NODEJS_PREFIX}/src/node/out/Release/lib"
    "${NODEJS_PREFIX}/src/node/out/Release/obj.target"
  NO_DEFAULT_PATH
  REQUIRED)

message(STATUS "Node.js library: ${NODEJS_LIBRARY}")

add_library(${NODEAPP_OBJ} OBJECT)

target_sources(
  ${NODEAPP_OBJ}
    PRIVATE
      source/main.cpp)

target_include_directories(
  ${NODEAPP_OBJ}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/thirdparty/include/node)

target_link_libraries(
  ${NODEAPP_OBJ}
    PUBLIC
      ${NODEJS_LIBRARY})

add_executable(${NODEAPP_EXE})
target_link_libraries(${NODEAPP_EXE} ${NODEAPP_OBJ})
