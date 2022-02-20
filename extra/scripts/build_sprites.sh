#! /bin/bash
set -euo pipefail
cd ../..

pushd data/textures/lpc_womens_shirts
python3 generate_sprite.py blouse  corset  long-sleeve_blouse  long-sleeve_shirt  pirate_shirt scoop_neck  sleeveless_shirt > ../../sprites/female_shirts.sprite
popd
