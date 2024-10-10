#!/usr/bin/env bash

# This script performs a simple set request of a new large binary file

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

## To simplify cleanup, add all files that your script uses.
### You can do this ahead of time, or as you create them.
new_files=""


## Create command file
test_file=test_files/large_binary.dat
size=$(wc -c $test_file | awk '{print $1}')

printf "set\ntest.txt\n$size\n" > command
new_files="$new_files test.txt command"

## run set command and save to file:
cat command $test_file | ./memory > output.txt 2>err.log &
mpid=$!


echo "" > mem_size
while [ -n  "$(ps -p $mpid -o pid=)" ]; do
    ps -p $mpid -o vsize= | tail -1 >> mem_size
    sleep .001;
done
max_kbs=`cat mem_size | awk ' BEGIN {max=0}
    	       	     {if ($1 > $max) {max=$1}}
		     END {print $max}
		     '`

mem_rc=$(wait $mpid)
new_files="$new_files output.txt err.log"



## Check each of the outputs:
## (1) memory's return code is 0
## (2) memory's stdout is empty
## (3) memory wrote the correct content to test.txt
## (4) memory ram used is less than 10 MB
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

diff output.txt <(echo "OK") > output_diff.txt
output_diff_rc=$?
if [[  $output_diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory produced the wrong output to stdout"$'\n'
    rc=1
fi
new_files="$new_files output_diff.txt"


diff test.txt test_files/large_binary.dat > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory wrote test.txt incorrectly"$'\n'
    rc=1
fi
new_files="$new_files diff.txt"

## check for KBs used used:
if [[ $max_kbs -gt 10240 ]]; then
    msg=$"${msg}Memory used too much memory"$'\n'
    rc=1
fi


if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "return code:"
    echo $mem_rc
    echo "--------------------------------------------------------------------------------"
    echo "Kbs:"
    echo $max_kbs
    echo "--------------------------------------------------------------------------------"
    echo "stdout:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat err.log
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "test.txt is correct."
    else
	echo "test.txt's difference from correct:"
	cat diff.txt
    fi
    echo "--------------------------------------------------------------------------------"
    if [[ $output_diff_rc -eq 0 ]]; then
	echo "stdout is correct."
    else
	echo "stdout's difference from correct:"
	cat output_diff.txt
    fi
    echo "--------------------------------------------------------------------------------"
fi

cleanup $new_files
exit $rc
