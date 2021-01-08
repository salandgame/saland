#! /bin/bash
set -e

if [ "$#" -ne 1 ]; then
	echo "Must be called with $0 <archivename>"
	exit 1
fi

ARCHIVENAME=$1

rm -rf staging
mkdir -p staging/$ARCHIVENAME/x86_64
mkdir -p staging/$ARCHIVENAME/docs
cp saland_standalone_launcher staging/$ARCHIVENAME/saland
cp README.txt staging/$ARCHIVENAME/README
chmod +x staging/$ARCHIVENAME/saland

cd ../..

docker build -f extra/docker/Dockerfile.Ubuntu18.04build_Standalone . -t saland_test

echo Copying to: $(pwd)/extra/standalone/staging/$ARCHIVENAME

docker run -it --rm -v $(pwd)/extra/standalone/staging/$ARCHIVENAME/:/output saland_test /bin/bash -c "cp /staging/saland-game/saland /output/x86_64/ && \
cp /usr/local/lib/libSDL2-2.0.so.0 /output/x86_64/ && \
cp /usr/local/lib/libphysfs.so.1  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_mixer-2.0.so.0  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_ttf-2.0.so.0  /output/x86_64/ && \
cp /usr/lib/x86_64-linux-gnu/libfreetype.so.6  /output/x86_64/ && \
cp /usr/lib/x86_64-linux-gnu/libpng16.so  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_image-2.0.so.0  /output/x86_64/"

docker run -it --rm -v $(pwd)/extra/standalone/staging/$ARCHIVENAME/:/output saland_test /bin/bash -c "\
cp -r /staging/saland-game/packages /output/ && \
cp /staging/saland-game/COPYING /output/ && \
cp /staging/saland-game/README.md /output/docs/README_ORG.md && \
chown -R 1000 /output"

exit 0
