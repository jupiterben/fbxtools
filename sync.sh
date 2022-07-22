#!/usr/bin/bash
commitMsg="update content"
if [ ! -n "$1" ]; then
    echo "use default commit message: $commitMsg"
else
    commitMsg=$1
fi

git pull
git add .
git commit -m "${commitMsg}"
git push
