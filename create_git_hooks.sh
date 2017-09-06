#!/bin/bash

GIT_REPO_DIR=$(git rev-parse --show-toplevel)

cd "$GIT_REPO_DIR"

for hook in hooks/*
do
    SYMLINK_HOOK=.git/$hook
    if [ -f $hook ] && [ ! -e "$SYMLINK_HOOK" ] && [ ! -L "$SYMLINK_HOOK" ]
    then
        ln -v -s "../../$hook" .git/hooks/
    fi
done
