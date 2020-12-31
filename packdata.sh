#!/bin/bash
set -e
mkdir -p packages
pushd data
zip -9X ../packages/saland-0001.data $(find * | sort)
popd
