#!/usr/bin/env bash
$(dirname $0)/stop.sh
if docker images | grep -q customer; then
    echo "Customer image detected -- removing"
    docker rmi customer
fi
echo "Building customer image"
docker build -t customer .
