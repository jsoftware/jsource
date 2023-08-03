#!/bin/bash
#
# write out sha, user
T=commit.txt
echo $1 > $T
echo $2 >> $T
