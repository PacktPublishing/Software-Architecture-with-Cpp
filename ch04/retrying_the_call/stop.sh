#!/usr/bin/env bash
if docker ps -a | grep -q example; then
    echo "Example container detected -- stopping and removing"
    docker stop example
    docker rm example
fi
