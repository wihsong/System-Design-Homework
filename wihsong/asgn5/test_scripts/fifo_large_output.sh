source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

# New files created by program
new_files=""

# Standard input sent to program
printf "a\nb\nc\nd\nd\nc\nb\na\nh\ng\nf\ne\nh\ng\nf\ne\ni\nj\nk\nl\ni\na\nj\nb\nk\nc\nl\nd\nd\nb\nc\na\n" > input.txt
./cacher -N 8 -F < input.txt > output.txt 2>err.log
cache_rc=$?

# Create accepted output file
printf "MISS\nMISS\nMISS\nMISS\nHIT\nHIT\nHIT\nHIT\nMISS\nMISS\nMISS\nMISS\nHIT\nHIT\nHIT\nHIT\nMISS\nMISS\nMISS\nMISS\nHIT\nMISS\nHIT\nMISS\nHIT\nMISS\nHIT\nMISS\nHIT\nHIT\nHIT\nHIT\n" > accepted.txt

# Add input file to new files for cleanup
new_files="$new_files input.txt output.txt accepted.txt err.log"

# Remove last line from output file to test for correct stdout response (HIT/MISS)
sed -i '$d' output.txt

rc=0
msg=""

## Check if cacher's return code is 0
if [[ $cache_rc -ne 0 ]]; then
    msg=$'Cacher returned non-zero error code\n'
    rc=1
fi

# Check difference of output and accepted output files
diff output.txt accepted.txt > diff.txt
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
