#!/bin/bash

images_changed=$(git diff-tree --no-commit-id --name-only -r HEAD | grep ^Docker/ | sed -e 's@^Docker/@@;s@.mksh$@@;s@Dockerfile.@@' | uniq)

for image in $images_changed; do
  echo "Processing deps image $image"
  fullpath=$CI_REGISTRY_IMAGE/deps/$(echo $image | sed -e 's@/@-@g')
  docker manifest inspect "$fullpath" > /dev/null
  if [ $? -ne 1 ]; then continue; fi
  pushd "Docker/$(dirname $image)"
  docker buildx build --platform "linux/amd64,linux/ppc64le,linux/s390x,linux/386,linux/arm64,linux/arm/v7" -f "Dockerfile.$(basename $image)" -t "$fullpath" --push .
  popd
done
