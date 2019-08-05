#! /bin/bash
find ../src -not \( -path ../src/Libs -prune \) -name \*.cpp | xargs astyle -t -j -y -c -k1 -z2 -A2 --pad-header
find ../src -not \( -path ../src/Libs -prune \) -name \*.hpp | xargs astyle -t -j -y -c -k1 -z2 -A2 --pad-header
