#!/usr/bin/env bash

## This script performs a simple get request of binary data.


source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

## To simplify cleanup, add all files that your script uses.
### You can do this ahead of time, or as you create them.
new_files=""


# copy small binary file:
ln -s test_files/medium_binary.dat test.dat
new_files="$new_files test.dat"


## run get command and save to file:
printf "get\ntest.dat\n" | env THROTTLER_READS=1 LD_PRELOAD="$source_dir/throttler.so" ./memory > output.dat 2>err.log
mem_rc=$?

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
diff output.dat test.dat > diff.dat
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory produced wrong output"$'\n'
    rc=1
fi
new_files="$new_files diff.dat"


if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "return code:"
    echo $mem_rc
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat err.log
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "memory produced correct stdout."
    else
	echo "memory's stdout's difference from correct:"
	cat diff.dat
    fi
    echo "--------------------------------------------------------------------------------"
fi

cleanup $new_files
exit $rc


