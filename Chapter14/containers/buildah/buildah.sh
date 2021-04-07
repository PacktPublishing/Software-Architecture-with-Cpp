#!/bin/sh

set -x

ctr=$(buildah from ubuntu:bionic)

buildah run $ctr -- /bin/sh -c 'apt-get update && apt-get install -y build-essential gcc'

buildah config --cmd '/usr/bin/gcc' "$ctr"

buildah commit "$ctr" dominican-gcc

buildah rm "$ctr"
