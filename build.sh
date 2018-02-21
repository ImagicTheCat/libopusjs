#!/bin/bash
# usage: ./build.sh [flags]
em++ api.cpp -Iopus/include -Lopus/.libs -lopus --post-js api.js $@ -o libopus.js
