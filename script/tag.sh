#!/bin/bash
if [[ $# -ne 1 ]]; then
    echo "parameter of form: j904-beta-b"
    exit 2
fi

cd ~/git/jsource
echo $1 >> jsrc/tags.txt
git tag -a $1 -m "$1"
git commit -a -m "$1"
git push origin $1
git push --follow-tags




