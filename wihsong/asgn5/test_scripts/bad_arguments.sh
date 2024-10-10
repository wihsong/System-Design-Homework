source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

# New files created by program
new_files=""

# Standard input sent to program
./cacher > output.txt 2>err.log
cache_rc=$?
new_files="$new_files output.txt err.log"


rc=0
msg=""

## Check if cacher's return code is 0
if [[ $cache_rc -eq 0 ]]; then
    msg=$'Cacher returned zero\n'
    rc=1
fi

# Check if error message is outputted
if [[ `cat err.log | wc -c` -eq "0" ]]; then
    msg=$"${msg}Did not produce error message for invalid arguments"$'\n'
    rc=1
fi

new_files="$new_files"

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
fi

cleanup $new_files
exit $rc
