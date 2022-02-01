cmake_minimum_required(VERSION 3.16)

project(nodeapp-root)

set(THIRDPARTY_PREFIX ${CMAKE_SOURCE_DIR}/thirdparty)
set(NODEJS_TAG "v17.4.0" CACHE STRING "Node.js git tag")

include(ExternalProject)

ExternalProject_Add(
  node
  GIT_REPOSITORY    https://github.com/nodejs/node.git
  GIT_TAG           ${NODEJS_TAG}
  PREFIX            ${THIRDPARTY_PREFIX}/build
  BUILD_IN_SOURCE   ON
  CONFIGURE_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ./configure --enable-static --prefix=${THIRDPARTY_PREFIX}
  BUILD_COMMAND     $(MAKE)
  INSTALL_COMMAND   $(MAKE) install)

ExternalProject_Add(
  nodeapp
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/source
  CMAKE_ARGS
    -D THIRDPARTY_PREFIX=${THIRDPARTY_PREFIX})

ExternalProject_Add_StepDependencies(nodeapp build node)
