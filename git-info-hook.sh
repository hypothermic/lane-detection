#!/usr/bin/zsh
# 
# This script updates the gitinfo2 information
# Run it as a post-commit hook or as part of .latexmkrc
# see https://mirrors.nxthost.com/ctan/macros/latex/contrib/gitinfo2/gitinfo2.pdf
# 

FIRSTTAG=$(git describe --tags --always --dirty='-*' 2>/dev/null)
RELTAG=$(git describe --tags --long --always --dirty='-*' --match '[0-9]*.*' 2>/dev/null)
git --no-pager log -1 --date=short --decorate=short --pretty=format:"\usepackage[shash={%h},lhash={%H},authname={%an},authemail={%ae},authsdate={%ad},authidate={%ai},authudate={%at},commname={%cn},commemail={%ce},commsdate={%cd},commidate={%ci},commudate={%ct},refnames={%d},firsttagdescribe={$FIRSTTAG},reltag={$RELTAG}]{gitexinfo}" HEAD > .git/gitHeadInfo.gin
echo "Successfully updated git head information for gitinfo2"

