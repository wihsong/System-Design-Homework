#!/usr/bin/env bash

## This script performs a get of the largest allowed filename

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

## To simplify cleanup, add all files that your script uses.  You can
## do this ahead of time, or as you create them.
new_files=""


## Get the largest filename that works.  This has an added benefit of
## setting "traps" for a program that uses an bad buffer sizes.

base=""
2>/dev/null echo "$base"a > "$base"a
rc=$?

while [[ $rc -eq 0 ]]; do
    base="$base"a
    new_files="$new_files $base"
    2>/dev/null echo "$base"a > "$base"a
    rc=$?
done


## run get command and save to file:
printf "get\n$base\n" | ./memory > output.txt 2>err.log
mem_rc=$?
new_files="$new_files output.txt err.log"


## Check each of the outputs:
## (1) memory's return code is 0
## (2) memory's stdout is the expected file
## If any of these fail, produce a helpful message and set rc=1

### Note: we don't check for stderr being empty; you can use it for
### logging should you like!

rc=0
msg=""

## check if memory's return code is 0
if [[ $mem_rc -ne 0 ]]; then
    msg=$'Memory returned non-zero error code\n'
    rc=1
fi

## check if memory's stdout is the expected file
diff output.txt $base > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory produced wrong output"$'\n'
    rc=1
fi
new_files="$new_files diff.txt"


if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "return code:"
    echo $mem_rc
    echo "--------------------------------------------------------------------------------"
    echo "stdout:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat err.log
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "memory produced correct stdout."
    else
	echo "memory's stdout's difference from correct:"
	cat diff.txt
    fi
    echo "--------------------------------------------------------------------------------"
fi

cleanup $new_files
exit $rc
