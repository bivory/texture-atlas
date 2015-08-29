#
# The MIT License
#
# Copyright (c) 2010 Bryan Ivory bivory+textureatlas@gmail.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required (VERSION 2.6)
project (texture_atlas)


set (TEXTURE_ATLAS_LIBS)


#
# LibPNG
#
find_package(PNG REQUIRED)
include_directories (${PNG_INCLUDE_DIR})
list(APPEND TEXTURE_ATLAS_LIBS ${PNG_LIBRARIES})


#
# PNG++
#
set (PNG_PLUSPLUS_DIR "${PROJECT_SOURCE_DIR}/png++")
include_directories (${PNG_PLUSPLUS_DIR})
file (GLOB PNG_PLUSPLUS_SRC "${PNG_PLUSPLUS_DIR}/*.cpp")
source_group (png_plusplus FILES ${PNG_PLUSPLUS_SRC})


#
# Texture Packer
#
set (TEXTURE_PACKER_DIR "${PROJECT_SOURCE_DIR}/texturepacker")
include_directories (${TEXTURE_PACKER_DIR})
file (GLOB TEXTURE_PACKER_SRC "${TEXTURE_PACKER_DIR}/*.cpp")
source_group (texture_packer FILES ${TEXTURE_PACKER_SRC})


#
# Templatized C++ Command Line Parser
#
set (TCLAP_DIR "${PROJECT_SOURCE_DIR}/tclap/include")
include_directories (${TCLAP_DIR})
file (GLOB TCLAP_DIR_SRC "${TCLAP_DIR}/*.cpp")
source_group (tclap FILES ${TCLAP_DIR_SRC})


#
# Texture Atlas
#
file (GLOB TEXTURE_ATLAS_SRC "${PROJECT_SOURCE_DIR}/*.cpp")
source_group (texture_atlas FILES ${TEXTURE_ATLAS_SRC})
add_executable (texture-atlas
                  ${TEXTURE_ATLAS_SRC}
                  ${TEXTURE_PACKER_SRC})
target_link_libraries (texture-atlas ${TEXTURE_ATLAS_LIBS})

