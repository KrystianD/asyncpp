#!/bin/bash
shopt -s nullglob

GLOBIGNORE="*/external/*"

FILES=$(/usr/bin/ls \
  */*/*/*.{h,cpp} \
  */*/*.{h,cpp} \
  */*.{h,cpp} \
)
clang-format -i $FILES