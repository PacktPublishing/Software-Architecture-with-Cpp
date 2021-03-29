#!/bin/bash

# Copyright (c) 2018 Uber Technologies, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

function main() {
    set -x
    if ! [[ "${CMAKE_OPTIONS}" =~ "-DJAEGERTRACING_COVERAGE=ON" ]]; then
       exit 0
    fi
    unset -x

    local project_dir
    project_dir=$(git rev-parse --show-toplevel)
    cd "$project_dir" || exit 1

    local gcov_tool
    case "$CC" in
        gcc*) gcov_tool=${CC/gcc/gcov}
            ;;
        *) gcov_tool="$project_dir/scripts/llvm-gcov.sh"
            ;;
    esac
    find build -name '*.gcno' -exec "$gcov_tool" {} \;
    bash <(curl -s https://codecov.io/bash) || \
        echo "Codecov did not collect coverage reports"
}

main
