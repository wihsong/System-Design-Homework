source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

# New files created by program
new_files=""

# Standard input sent to program
printf "a\nb\na\nc\nc\nd\nd\na\nb\nb\na\n" > input.txt
./cacher -N 3 -C < input.txt > output.txt 2>err.log
cache_rc=$?

# Create accepted output file
echo "4 1" > accepted.txt

# Extract the last line to test the summary of cache misses
tail -n 1 output.txt > summary.txt

# Add above files to new files for cleanup
new_files="$new_files input.txt output.txt accepted.txt err.log summary.txt"

rc=0
msg=""

## Check if cacher's return code is 0
if [[ $cache_rc -ne 0 ]]; then
    msg=$'Cacher returned non-zero error code\n'
    rc=1
fi

# Check difference of summary and accepted output files
diff summary.txt accepted.txt > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Cacher produced wrong output"$'\n'
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
    echo $cache_rc
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat err.log
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "cacher produced correct stdout."
    else
	echo "cacher's stdout's difference from correct:"
	cat diff.txt
    fi
    echo "--------------------------------------------------------------------------------"
fi

cleanup $new_files
exit $rc
