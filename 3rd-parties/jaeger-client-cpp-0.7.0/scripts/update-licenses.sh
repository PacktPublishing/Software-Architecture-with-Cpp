#!/bin/bash

set -e

python scripts/update-license.py $(git ls-files "*\.cpp" "*\.h" |
                                   grep -v thrift-gen |
                                   grep -v tracetest) \
    src/jaegertracing/Constants.h.in
