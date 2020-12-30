#!/bin/bash
set -e
pushd data
zip -9X ../saland.data $(find * | sort)
popd