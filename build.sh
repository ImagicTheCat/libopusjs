#!/bin/bash
# usage: ./build.sh [flags]
# release flags: -O2 --memory-init-file 0
em++ api.cpp -Iopus/include -Lopus/.libs -lopus --post-js api.js $@ -o libopus.js
