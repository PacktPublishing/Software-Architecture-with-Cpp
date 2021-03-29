#!/bin/bash

# Based on https://github.com/codecov/example-cpp11-cmake/blob/master/run_build.sh.

set -e

RED='\033[0;31m'
BLUE='\033[0;34m'
NO_COLOR='\033[0m'
GREEN='\033[0;32m'

function info() {
    echo -e "${GREEN}$1${NO_COLOR}"
}

function working() {
    echo -e "${BLUE}$1${NO_COLOR}"
}

function main() {
    local project_dir
    project_dir=$(git rev-parse --show-toplevel)
    cd "$project_dir"

    mkdir -p build
    cd build
    cmake ${CMAKE_OPTIONS} ..
    make -j3 UnitTest
    info "Running tests..."
    ./UnitTest
    working "All tests compiled and passed"

    set -x
    if ! [[ "${CMAKE_OPTIONS}" =~ "-DJAEGERTRACING_BUILD_CROSSDOCK=ON" ]]; then
        exit 0
    fi
    make crossdock-fresh
}

main
