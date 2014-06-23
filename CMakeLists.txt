#    ____    _ __           ____               __    ____
#   / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
#  _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
# /___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)
#
# Copyright 2012 SciberQuest Inc.
#
# +---------------------------------------------------------------------------+
# |                                                                           |
# |                    SciberQuestToolKit Batch Programms                     |
# |                                                                           |
# +---------------------------------------------------------------------------+
#
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0003 NEW)
project(SciberQuestToolKit-Batch)

if("${PROJECT_SOURCE_DIR}"  STREQUAL "${PROJECT_BINARY_DIR}")
  message(FATAL_ERROR "SciberQuestToolKit requires an out of source Build.")
endif()

# paraview
set(ParaView_DIR
  /path/to/paraview/build
  CACHE FILEPATH
  "Path to ParaView build.")
if (NOT EXISTS ${ParaView_DIR})
  MESSAGE(FATAL_ERROR "Set ParaView_DIR to the path to a ParaView build." )
endif (NOT EXISTS ${ParaView_DIR})
find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})
message(STATUS "ParaView ${PARAVIEW_VERSION_FULL} found: ${ParaView_DIR}")

set(SQTK_LIB_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}/lib/paraview-${PARAVIEW_VERSION_MAJOR}.${PARAVIEW_VERSION_MINOR}"
  )
set(SQTK_BIN_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}/bin/"
  )
message(STATUS "SciberQuestToolKit batch executables will be installed to ${SQTK_BIN_INSTALL_PREFIX}")

add_subdirectory(execs)
add_subdirectory(scripts)
