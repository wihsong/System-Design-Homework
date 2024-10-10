#!/usr/bin/env bash

print_usage() {
    echo "./load_repo.sh {path_to_asgn0_dir}"
}


repo_dir=$1
if [ ! -d $repo_dir ]; then
    echo "$repo_dir does not exist."
    print_usage
    exit 1
fi


if [ ! -d $repo_dir/test_scripts ]; then
    mkdir $repo_dir/test_scripts
fi

if [ ! -d $repo_dir/test_files ]; then
    mkdir $repo_dir/test_files
fi

cp test_scripts/* $repo_dir/test_scripts/
cp ../test_files/* $repo_dir/test_files/
cp test_repo.sh $repo_dir/

echo "Tests Loaded!"

exit 0
