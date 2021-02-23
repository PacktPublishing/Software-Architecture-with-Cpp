#!/usr/bin/env bash

$(dirname $0)/stop.sh

echo "Running customer container"
docker run --name customer -d -p 8080:8080 customer
