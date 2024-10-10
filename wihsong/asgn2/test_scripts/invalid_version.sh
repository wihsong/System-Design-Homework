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

newfiles="output.txt error.txt"

# Wait until we can connect.
sleep 1


# run the test!
printf "GET /foo.txt HTTP/1.10\r\n\r\n" > requestfile
cat requestfile | $source_dir/cse130_nc localhost $port > actual.txt

# Expected status code.
printf "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n" > expected.txt

# Check the diff.
diff actual.txt expected.txt > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Server produced wrong resonse"$'\n'
    rc=1
fi
new_files="$new_files requestfile actual.txt expected.txt diff.txt"


if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "stdout:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat error.txt
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "Correct response."
    else
	echo "Incorrect response:"
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

