#!/usr/bin/zsh
#
# This script runs a bunch of tests after one another, feeding the input through.
# Because apparently Makefiles are not smart enough to do this.
# 
# Env var SAMPLE is the name of the sample image, just like with the Makefile.
#
# Usage: 
# $ DEBUG=true SAMPLE=0a0a0b1a-7c39d841 ./test.sh grayscale gaussian sobel
#

set -o nounset

#
# Run the first test
#

make test-lane-man ARG_TEST="$1" ARG_SAMPLE="${SAMPLE}"

#
# Run all of the subsequential tests, feeding the input from the previous into the next.
#
for ((i = 2; i <= $#; ++i))
do
	FILENAME+=".$@[i-1].out"
	make test-lane-man ARG_TEST="$@[i]" ARG_SAMPLE="${SAMPLE}${FILENAME}"
	
done

echo "Finished"
times

