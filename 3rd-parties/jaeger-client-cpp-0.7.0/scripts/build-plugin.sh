#!/bin/bash

set -e

function main() {
    local project_dir
    project_dir="$(git rev-parse --show-toplevel)"

    mkdir -p build
    cd build
    export CFLAGS="$CFLAGS -march=x86-64"
    export CXXFLAGS="$CXXFLAGS -march=x86-64"

    cat <<EOF > export.map
{
    global:
        OpenTracingMakeTracerFactory;
    local: *;
};
EOF

    cmake -DCMAKE_BUILD_TYPE=Release \
        -DJAEGERTRACING_PLUGIN=ON \
        -DBUILD_TESTING=ON \
        -DHUNTER_CONFIGURATION_TYPES=Release \
        ..
    make -j3
    mv libjaegertracing_plugin.so /libjaegertracing_plugin.so
    ./DynamicallyLoadTracerTest /libjaegertracing_plugin.so
}

main
