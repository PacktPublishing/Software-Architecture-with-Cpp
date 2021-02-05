#!/usr/bin/env bash
if [[ $(docker-compose ps -q | wc -l) -gt 0 ]]; then
    echo "Example containers detected -- stopping and removing"
    docker-compose down
fi
