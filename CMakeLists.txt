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

add_subdirectory(execs)
