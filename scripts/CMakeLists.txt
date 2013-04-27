#    ____    _ __           ____               __    ____
#   / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
#  _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
# /___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)
#
# Copyright 2012 SciberQuest Inc.
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0003 NEW)

set(SQTK_SCRIPTS
  sqtk-batch-longhorn.qsub)

install(FILES ${SQTK_SCRIPTS} DESTINATION ${SQTK_INSTALL_PREFIX}/)
