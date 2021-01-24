#!/usr/bin/env bash
$(dirname $0)/stop.sh
if docker images | grep -q example; then
    echo "Example image detected -- removing"
    docker rmi example
fi
echo "Building example image"
docker build -t example .
