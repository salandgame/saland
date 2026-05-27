#!/usr/bin/env bash
# Download the SDL3 family of source tarballs into embedded_libs/downloads/.
#
# After running this script the project can be configured with the default
# (FetchContent) path while completely offline.
#
# Usage:
#   ./extra/scripts/fetch_sdl3_sources.sh
#
# Requires:
#   curl

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DOWNLOAD_DIR="${ROOT_DIR}/embedded_libs/downloads"

mkdir -p "${DOWNLOAD_DIR}"

URLS=(
    "https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-3.4.8.tar.gz"
    "https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.4/SDL3_image-3.4.4.tar.gz"
    "https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.2/SDL3_mixer-3.2.2.tar.gz"
    "https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz"
)

for url in "${URLS[@]}"; do
    file="${url##*/}"
    out="${DOWNLOAD_DIR}/${file}"
    if [ -f "${out}" ]; then
        echo "Already present: ${file}"
        continue
    fi
    echo "Downloading ${file} ..."
    curl -fL --retry 3 -o "${out}.tmp" "${url}"
    mv "${out}.tmp" "${out}"
done

echo
echo "Done. Tarballs in ${DOWNLOAD_DIR}:"
ls -la "${DOWNLOAD_DIR}"
