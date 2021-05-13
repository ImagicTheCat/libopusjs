#!/bin/bash

# opus repo
git init
git remote add origin https://github.com/xiph/opus.git
git pull

# version
git checkout v1.3.1

# build lib
./autogen.sh
emconfigure ./configure --disable-rtcd --disable-intrinsics --disable-shared \
  --enable-static  --disable-stack-protector --disable-hardening
emmake make -j$(nproc)

# should ouput libopus.a in .libs/ that is linkable with emcc
