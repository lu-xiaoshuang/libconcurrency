#
# Copyright (C) 2018 Xiaoshuang LU
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required (VERSION 3.26.0)

#option(ENABLE_TESTING "enable testing" ON)

set(LIBCONCURRENCY_VERSION "1.0.0" CACHE STRING "LIBCONCURRENCY_VERSION")

set(CMAKE_INSTALL_PREFIX "/opt/libconcurrency-${LIBCONCURRENCY_VERSION}" CACHE STRING "PREFIX")

# TODO
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    message (FATAL_ERROR "CMAKE_INSTALL_PREFIX cannot be empty.")
endif()

project(libconcurrency C CXX)

set(
    TARGET_LIBRARIES
    debug libconcurrency_c-static optimized libconcurrency_c-static
)

INCLUDE_DIRECTORIES(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/code)

enable_testing()

#
#   The add_subdirectory command introduces a new scope which inherits a copy of the parent's
#   variables. However, subsequent changes to the variables in the parent scope don't affect
#   the child copies, and vice versa.
#
#   References
#
#   http://stackoverflow.com/questions/20800166/
#

add_subdirectory(code)
add_subdirectory(test)
