#!/usr/bin/env bash

$(dirname $0)/stop.sh

echo "Running example container"
docker run --name example -d -p 10000:10000 example
