#!/usr/bin/env bash

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ `check_dir` -eq 1 ]]; then
    exit 1
fi

port=`get_port`

if [[ port -eq 1 ]]; then
    exit 1
fi

# Start up server.
./httpserver $port > output.txt 2>error.txt &
pid=$!

# Wait until we can connect.
sleep 1


# Test input file.
file="test_files/antihero.txt"
infile="temp.txt"
outfile="outtemp.txt"
new_files="$infile $outfile output.txt error.txt"

# Copy the input file.
cp $file $infile

# run the test!
actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile -H "Expect:")

# Expected status code.
expected=200

rc=0
msg=""

# Check the status code.
if [[ $actual -ne $expected ]]; then
    rc=1
    msg=$'Incorrect status code\n'
fi

# Check the diff.
diff $outfile $file  > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Server produced wrong message body"$'\n'
    rc=1
fi
new_files="$new_files diff.txt"


if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "Your status code: $actual (expected: $expected)"
    echo "--------------------------------------------------------------------------------"
    echo "stdout:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat error.txt
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "Message Body is correct."
    else
	echo "Message Body is incorrect.  The difference from correct is:"
	cat diff.txt
    fi
fi

# Clean up.
## Make sure the server is dead.
kill -9 $pid
wait $pid &>/dev/null

## remove ded files
cleanup $new_files

exit $rc
