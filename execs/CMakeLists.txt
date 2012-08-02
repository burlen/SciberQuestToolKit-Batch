#    ____    _ __           ____               __    ____
#   / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
#  _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
# /___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)
#
# Copyright 2012 SciberQuest Inc.
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0003 NEW)

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

set(SQTK_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}/lib/paraview-${PARAVIEW_VERSION_MAJOR}.${PARAVIEW_VERSION_MINOR}"
  )
message(STATUS "SciberQuestToolKit batch executables will be installed to ${SQTK_INSTALL_PREFIX}")

# mpi
include(FindMPI)
if (NOT MPI_FOUND)
  message(SEND_ERROR "MPI is required.")
endif()
add_definitions("-DMPICH_IGNORE_CXX_SEEK")
message(STATUS "MPI found.")

# sqtk plugin
get_filename_component(ParaView_SOURCE ${PARAVIEW_USE_FILE} PATH)
set(SciberQuestToolKit_DIR "${ParaView_SOURCE}/Plugins/SciberQuestToolKit" CACHE FILEPATH "SciberQuestToolKit sources")
include_directories("${SciberQuestToolKit_DIR}")
message(STATUS "SciberQuestToolKit found: ${SciberQuestToolKit_DIR}")

# executables
add_executable(MagnetosphereTopoBatch MagnetosphereTopoBatch.cpp DebugUtil.cxx)
target_link_libraries(MagnetosphereTopoBatch ${MPI_LIBRARIES} SciberQuestToolKit)
install(TARGETS MagnetosphereTopoBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

add_executable(SmoothBatch SmoothBatch.cpp DebugUtil.cxx)
target_link_libraries(SmoothBatch ${MPI_LIBRARIES} SciberQuestToolKit)
install(TARGETS SmoothBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

add_executable(VortexDetectBatch VortexDetectBatch.cpp DebugUtil.cxx)
target_link_libraries(VortexDetectBatch ${MPI_LIBRARIES} SciberQuestToolKit)
install(TARGETS VortexDetectBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

add_executable(SlicerBatch SlicerBatch.cpp DebugUtil.cxx)
target_link_libraries(SlicerBatch ${MPI_LIBRARIES} SciberQuestToolKit)
install(TARGETS SlicerBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

add_executable(MagnetosphereTopoBatchPVD MagnetosphereTopoBatchPVD.cpp DebugUtil.cxx)
target_link_libraries(MagnetosphereTopoBatchPVD ${MPI_LIBRARIES} SciberQuestToolKit)
install(TARGETS MagnetosphereTopoBatchPVD DESTINATION ${SQTK_INSTALL_PREFIX}/)
