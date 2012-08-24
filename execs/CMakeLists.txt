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
set(SciberQuestToolKit_DIR "${ParaView_SOURCE}/../Plugins/SciberQuestToolKit" CACHE FILEPATH "SciberQuestToolKit sources")
include_directories("${SciberQuestToolKit_DIR}")
find_library(SciberQuestToolKit_Lib SciberQuestToolKit "${ParaView_DIR}/bin" "${ParaView_DIR}/lib")
message(STATUS "SciberQuestToolKit found: ${SciberQuestToolKit_DIR}")
message(STATUS "SciberQUestToolKit lib: ${SciberQuestToolKit_Lib}")
# executables


add_executable(PoincareBatch PoincareBatch.cpp DebugUtil.cxx BatchUtil.cxx)
target_link_libraries(PoincareBatch ${MPI_LIBRARIES} ${SciberQuestToolKit_Lib})
install(TARGETS PoincareBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

add_executable(SmoothBatch SmoothBatch.cpp DebugUtil.cxx BatchUtil.cxx)
target_link_libraries(SmoothBatch ${MPI_LIBRARIES} ${SciberQuestToolKit_Lib})
install(TARGETS SmoothBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)

#
# TODO -- the following need integrate batch utils and be tested
#
#add_executable(MagnetosphereTopoBatch MagnetosphereTopoBatch.cpp DebugUtil.cxx BatchUtil.cxx)
#target_link_libraries(MagnetosphereTopoBatch ${MPI_LIBRARIES} ${SciberQuestToolKit_Lib})
#install(TARGETS MagnetosphereTopoBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)
#
#add_executable(VortexDetectBatch VortexDetectBatch.cpp DebugUtil.cxx BatchUtil.cxx)
#target_link_libraries(VortexDetectBatch ${MPI_LIBRARIES} ${SciberQuestToolKit_Lib})
#install(TARGETS VortexDetectBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)
#
#add_executable(SlicerBatch SlicerBatch.cpp DebugUtil.cxx BatchUtil.cxx)
#target_link_libraries(SlicerBatch ${MPI_LIBRARIES} ${SciberQuestToolKit_Lib})
#install(TARGETS SlicerBatch DESTINATION ${SQTK_INSTALL_PREFIX}/)
