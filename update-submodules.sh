#!/usr/bin/env sh
for s in `ls libs`; do
  git submodule update --init "libs/$s"
done

