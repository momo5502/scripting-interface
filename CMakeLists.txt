cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#####################################

project(scripting-interfaces LANGUAGES CXX)

#####################################

option(MOMO_SCRIPTING_ENABLE_SAMPLES "Enable sample projects" OFF)

#####################################

add_subdirectory(src)

if(MOMO_SCRIPTING_ENABLE_SAMPLES)
  add_subdirectory(samples)
endif()
