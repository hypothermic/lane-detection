#!/bin/zsh

EXEC=`compare -metric rmse data/0a0a0b1a-7c39d841.ppm data/0a0a0b1a-7c39d841.out.ppm null: 2>&1`
EXPECTED="0 (0)"

[ "$EXEC" = "$EXPECTED" ] && exit 0 || exit 1
