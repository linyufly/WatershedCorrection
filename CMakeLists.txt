cmake_minimum_required(VERSION 2.8)

project(WatershedCorrection)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()

add_executable(WatershedCorrectorTest watershed_corrector_test.cc
                                      watershed_corrector.cc)
target_link_libraries(WatershedCorrectorTest ${VTK_LIBRARIES})

