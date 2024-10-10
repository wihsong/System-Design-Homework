# Assignment 3 Resources

This directory contains resources for assignment 3:

* test_scripts/ A directory containing test scripts for evaluating your
  assignment.

* queue_users/ A directory of applications that use your queue for
  testing your assignment.

* rwlock_users/ A directory of applications that use your rwlock for
  testing your assignment.

* queue.h the header file that you are implementing

* rwlock.h the header file that you are implementing

* load_repo.sh a script that can be used to load your asgn2 for testing

* test_repo.sh a script that can be used to test your repository

In the instructions that follow, {path_to_asgn3_dir} is the path to
your asgn3 directory.

## Using the header:

Copy the header files into your asgn3 directory:

```
cp helper_funcs/queue.h {path_to_asg3_dir}
cp helper_funcs/rwlock.h {path_to_asg3_dir}

```

You can use the header file in the same way that you would use any
other header.  DON'T MAKE ANY CHANGES--we will replace your header
with ours when we build and evaluate your code.


## Using the tests

1. Copy the tests to your repository using the supplied load_repo.sh
   script:

```
./load_repo.sh {path_to_asgn3_dir}
```


2. Go to your asgn3 directory:

```
    cd {path_to_asgn3_dir}
```

3. Make your queue.o binary:

```
    make
````

4. execute test_repo.sh:

```
    ./test_repo.sh
```

This command will print out each test and whether it passed or failed.
If the test passed, you will see a message saying "SUCCESS"; if it
fails you will see a message saying "FAILURE".  You can execute each
test individually as well.  For example, to execute the test
test_scripts/{test}.sh, run:

```
./test_scripts/{test}.sh
```
