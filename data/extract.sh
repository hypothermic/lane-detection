#!/bin/zsh
#
# This script moves images from the BDD100K 10K images set
# and puts them in the target folder after converting them.
#

#
# Options
#

BDD_PATH=${1:-/tmp/bdd100k_images_10k/bdd100k/images/10k}
TARGETS_PATH=${2:-./data/targets}
OUTPUT_PATH=${3:-./data}

#
# A list of all the images we want to move
#

typeset -a sample_pics
sample_pics=("${(@f)$(< ${TARGETS_PATH})}")

#
# Instructions start
#

echo -e "Using BDD path: $BDD_PATH\nUsing targets from: $TARGETS_PATH\nOutputting to: $OUTPUT_PATH"

for pic in "${sample_pics[@]}";
do 
	echo "Extracting picture: $pic";
	cp "$BDD_PATH/train/$pic" "$OUTPUT_PATH/$pic" &&
	mogrify -compress none -format ppm "$OUTPUT_PATH/$pic";
done

echo "Finished"
times

