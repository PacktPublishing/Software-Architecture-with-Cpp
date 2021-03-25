#!/usr/bin/env bash

$(dirname $0)/stop.sh

echo "Running example containers"
docker-compose up --build -d
