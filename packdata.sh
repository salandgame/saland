#!/bin/bash
set -e
mkdir -p packages
pushd data
find * | sort | xargs zip -9oX ../packages/saland-0001.data
popd
