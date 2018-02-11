#!/usr/bin/env sh

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

# If a command fails, then the shell shall immediately exit.
set -o errexit

THIS_SCRIPT="$(basename "$(readlink -f "$0")")"
WORKING_DIRECTORY="$(dirname "$(readlink -f "$0")")"

if [[ "x$PROJECT_VERSION" == "x" ]]
then
    PROJECT_VERSION=1.0.0
fi

if [[ "x$BUILD_MODE" == "x" ]]
then
    BUILD_MODE=debug
fi

if [[ "x$BUILD_NUMBER" == "x" ]]
then
    BUILD_NUMBER=1
fi

run()
{
    # write command itself to standard output
    echo "\$ $@"

    # run
    "$@"

    exit_status_value="$?"

    # check exit status value
    if [ "x$exit_status_value" != "x0" ];
    then
        echo
        echo "FAILURE"
        echo
        exit "$exit_status_value"
    fi
}

function check_variables()
{
    for entry in $@
    do
        entry_value=""
        eval entry_value=\${${entry}}

        if [[ "x${entry_value}" == "x" ]]
        then
            echo "${entry} is not specified."
            exit 1
        fi
    done
}

run \
    check_variables \
    THIS_SCRIPT \
    WORKING_DIRECTORY \
    PROJECT_VERSION \
    BUILD_MODE \
    BUILD_NUMBER

# build

run rm -rf "$WORKING_DIRECTORY/target"

run mkdir "$WORKING_DIRECTORY/target"

run cd "$WORKING_DIRECTORY/target"

if [[ "x$BUILD_MODE" == "xdebug" ]]
then
    CFLAGS_VALUE="-std=c17 -pedantic -ansi -Wall -Wextra -Winline -Wno-deprecated-declarations -Wno-unused-parameter -fPIC -m64 -g3 -O0"
    CXXFLAGS_VALUE="-std=c++17 -pedantic -Wall -Wextra -Winline -Wno-deprecated-declarations -Wno-unused-parameter -fPIC -m64 -g3 -O0"
elif [[ "x$BUILD_MODE" == "xrelease" ]]
then
    CFLAGS_VALUE="-std=c17 -pedantic -ansi -Wall -Wextra -Winline -Wno-deprecated-declarations -Wno-unused-parameter -fPIC -m64 -g0 -O3"
    CXXFLAGS_VALUE="-std=c++17 -pedantic -Wall -Wextra -Winline -Wno-deprecated-declarations -Wno-unused-parameter -fPIC -m64 -g0 -O3"
else
    echo "Invalid build mode."
    exit 1
fi

run \
    cmake -G "Unix Makefiles" \
    -DBUILD_SHARED_LIBS=on \
    -DCMAKE_BUILD_TYPE=$BUILD_MODE \
    -DCMAKE_C_COMPILER="gcc" \
    -DCMAKE_C_FLAGS="$CFLAGS_VALUE" \
    -DCMAKE_CXX_COMPILER="g++" \
    -DCMAKE_CXX_FLAGS="$CXXFLAGS_VALUE" \
    -DCMAKE_INSTALL_PREFIX="$WORKING_DIRECTORY/target" \
    ..

run make -j 8 VERBOSE=1

#run make test VERBOSE=1

run make install VERBOSE=1

set +o errexit
