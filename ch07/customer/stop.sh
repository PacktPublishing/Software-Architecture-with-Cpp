#!/usr/bin/env bash
if docker ps -a | grep -q customer; then
    echo "Customer container detected -- stopping and removing"
    docker stop customer
    docker rm customer
fi
