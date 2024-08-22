# Debugging code

This lab will walk you through two different tools for debugging:
- GDB (Adapted from https://classes.engr.oregonstate.edu/eecs/winter2020/cs161-020/labs/lab8-W20.pdf)
- Valgrind (Adapted from [this lab](http://people.cs.ksu.edu/~sherrill/labs/lab05/lineage.cpp))

# Debugging with GDB

Instructions for using GDB on VS Code may be found [here](https://code.visualstudio.com/docs/cpp/CMake-linux). This lab will show you how to do it from a terminal but feel free to use VS Code in the future.

GDB (GNU Debugger) is a tool we can use to identify issues inside a program by looking into its code and execute statements step by step. This is called interactive or dynamic debugging (you will learn about static debugging in lecture). The whole manual for gdb can be found by running:
```
man gdb
```
In this lab you will learn some of the most common uses of GDB. The first step is to compile your program adding the -g flag:
```
$ g++ <filename>.cpp -g -O0 *.cpp -o <exec_name>
```
Let's start with a simple program. Create a new file called `debug.cpp` and add the following code:
```
#include <iostream> 
#include <cstring> 

using namespace std; 

int main() { 
    char input[50]; 
    int i = 42; 
    cin.getline(input, 50); 
    for (i = strlen(input); i >= 0; i--) {  
        cout << input[i]; 
        }  
    cout << endl; 
    return 0; 
    }
```

Compile the program and start the debugging by running:
```
$ g++ debug.cpp -g -o debug 
$ gdb ./debug 
```
## Break
You can setup breakpoints in your code. These breakpoints tell the debugger to pause at that statement during execution. You generally want to pause at statements that seem to be causing issues. If you don't know which statement it is, then set a breakpoint before the suspected issue. You can always back forwards from a breakpoint. For now setup a breakpoint in line 8 (the line with `int i=42`, modify the number if it happens to be a different one for you)  from within GDB by running the following command:
```
(gdb) break 8 
```
You should see the message `Breakpoint 1 at 0x4008b5: file debug.cpp, line 8.` Where 0x4008b5 is the location of that line in memory, so yours might be different.

## Run

Now that you have set your breakpoint, you can start executing the program by typing the gdb command `run`. This will run the program as you would normally but it will pause once it encoounters the first breakpoint, at which point you will be returned to the gdb command prompt. Your terminal should display something like this:
```
Starting program: ./debug

Breakpoint 1, main () at debug.cpp:8
8       int i=42;
```
> Note: If you failed to compile with the debug flag, you will see a message indicating that gdb cannot find the symbol table.

## Print

An important piece of information to find bugs is knowing the value that variables have at a given statement. To get this information we use `print` followed by the variable we want to see. Run the following gdb command:
```
(gdb) print i
```
You should get the value of i, which at this point it is 0.

## Next and Step

To keep moving forward in the execution order, you will need to use `next` and `step`. The difference between them is that next will move forward to the next function, whereas step will go into each function you encounter and visit each statement inside it (and inside other functions that might be inside this function). Let's try moving to line 9 by using step. Run the following gdb command:
```
(gdb) step
```
You should now see line 9 displayed on the gdb command prompt. No check the value of i again by running `print i`. What value do you see and why is it different from before?

Since we don't want to go into the `getline()` function, we need to use next if we want to move to line 10. Just type `next`. You should notice that line 10 doesn't appear right away. This is because line 9 is expecting a user input. Go ahead and type `hello` then press enter. After that, line 10 will appear.

## Continue

The gdb command `continue` will resume exectuion until another breakpoint or the end of the program is reached. Type this command and you should see the final output for the program followed by the message that it exited normally (if there were running issues in the code that made it finish early, you would get a message regarding those issues).

> Once again, let me remind you that you can do debugging using your IDE. On Visual Studio Code, setting a breakpoint is just a matter of clicking the empty space to the left of a statement, clicking again to remove it. Once you start debugging you can so step, next and continue through the use of the keyboard o by clicking the debugging controls. This manual is walking you through the use of gdb in a terminal so you know how to debug if you don't have access to an IDE but after this lab it is ok for you to debug using the visual tools.

## Display and Watch

Before moving forward, run the gdb command `info breakpoints`. This should give you information on which breakpoints are currently present in the code. After that, delete the breakpoint by typing `del break 1` and verify there are no breakpoint by running `info breakpoints` again. Now, set another breakpoint at line 11 (the line with the cout statement inside the for loop). Now we will use `display` to show how a variable or expression changes throughout the code. Type the following (the lines without gdb at the start are what you should see on your screen, you don't need to type them):
```
(gdb) run 
Starting program: <path>
hello
Breakpoint 2, main () at debug.cpp:11 
11      cout << input[i];  
(gdb) display input[i]
1: input[i] = 0 '\000' 
(gdb) print i 
$3 = 5 
(gdb) next 
10      for (i = strlen(input); i >= 0; i--) { 
1: input[i] = 0 '\000' 
(gdb) next 
Breakpoint 2, main () at debug.cpp:11
11            cout << input[i];
1: input[i] = 111 'o'
(gdb) print i
$4 = 4 
(gdb) next 
13      for (i = strlen(input); i >= 0; i--) {
1: input[i] = 111 'o' 
```
Each time you type next, it will display the value for input. But what if we don't want to see how the value changes (or doesn't) after every statement and instead we want to see the value only when there's a change. For this we use `watch`. Run the following statements on gdb and see how `watch`behaves:

```
(gdb) run
The program being debugged has been started already. Start it from the beginning? (y or n) y
Starting program: ./debug 
hello
(gdb) watch i
Hardware watchpoint 2: i 
(gdb) continue 
Continuing.
Hardware watchpoint 2: i

Old value = 0 New value = 5
0x00000000004008e1 in main () at debug.cpp:10
10	for (i = strlen(input); i >= 0; i--) { 
(gdb) continue
Continuing.
Hardware watchpoint 3: i

Old value = 5 New value = 4
0x0000000000400900 in main () at debug.cpp:10
10	for (i = strlen(input); i >= 0; i--) { 
(gdb) continue
Continuing.
Hardware watchpoint 3: i

Old value = 4 New value = 3
0x0000000000400900 in main () at debug.cpp:10
10	for (i = strlen(input); i >= 0; i--) {

```
With `watch` we don't need to set breakpoints to see when the vlue changes. It will automatically break each time it changes and display the new value.

### Commit and push
At this point you should add debug.cpp to git, then commit and push your changes. Add a comment mentioning that you are adding debug.cpp to the repository

## Where

The last gdb command we will introduce (though there are many more) is the `where` (or `backtrace`) command which prints a backtrace of all stack frames. This is very useful for identifying which function is causing the program to crash. Let's edit your `debug.cpp` program so that it will crash. With VS Code or VIM, edit the code so it looks like this (you just need to modify the first line after `main()`):

```
#include <iostream> 
#include <cstring>

using namespace std;

int main() {
    char* input = NULL; 
    int i = 42; 
    cin.getline(input, 50);
    for (i = strlen(input); i >= 0; i--) { 
        cout << input[i];
    }
    cout << endl; 
    return 0;
}
```
Do a commit now but don't push it, we haven't tested the code and you shouldn't push until we do so. Recompile your program (using the debug flag) and start gdb for debugging. You should see something like this:
```
(gdb) run
Starting program: ./debug hello

Program received signal SIGSEGV, Segmentation fault. 
0x00007ffff7b499b0 in std::istream::getline(char*, long, char) () from /lib64/libstdc++.so.6 
(gdb) where
#0 0x00007ffff7b499b0 in std::istream::getline(char*, long, char) () from /lib64/libstdc++.so.6
#1 0x00000000004008ca in main () at debug.cpp:9
```
The program crashed when getline() tried to put data into the NULL pointer. This is in stack frame #0. We can move "up" to frame #1 to inspect what was happening in main() when getline() was called.
```
(gdb) up
#1 0x00000000004008ca in main () at debug.cpp:9
9	cin.getline(input, 50);
```
This indicates that line #9 is where the program crashed. We can then inspect the input
variable and figure out why there was a crash.

```
(gdb) p input
$5 = 0x0
```
It was that NULL pointer!!! Now you can correct the bug, compile and run away. Remember to commit and push with a message expalining that a bug was found and fixed.

There are many other commands and if you get stuck you can always type `help <command>` to get a description and usage of a specific gdb command. 

# Memory management
Now we shift our attention to a diffetent type of debugging. The ability to dynamically allocate and deallocate memory is one of the strongest features of C++ programming, but the greatest strength can also be the greatest weakness. This is certainly true of C++ applications, where memory-handling problems are among the most common bugs.

One of the most subtle and hard-to-detect bugs is the memory leak - the failure to properly deallocate memory that was previously allocated. A small memory leak that occurs only once may not be noticed, but programs that leak large amounts of memory, or leak progressively, may display symptoms ranging from poor (and gradually decreasing) performance to running out of memory completely. Worse, a leaking program may use up so much memory that it causes another program to fail, leaving the user with no clue to where the problem truly lies. In addition, even harmless memory leaks may be symptomatic of other problems.
## Valgrind
[Valgrind](https://valgrind.org/) is an instrumentation framework for building dynamic analysis tools. We will be focusing on the Memcheck tool in their extensive tool suite. 

> Note: Valgrind is designed for Linux, but it is also compatible with some versions of Mac OS (unless you are running a very new version of the OS).  If you are performing this lab on Windows, it is recommended that you use the `CS100` server to run valgrind.

### Memcheck
**Memcheck** detects memory-management problems, and is aimed primarily at C and C++ programs. When a program is run under Memcheck's supervision, all reads and writes of memory are checked, and calls to `malloc`/`new`/`free`/`delete` are *intercepted*. As a result, Memcheck can detect if your program:
 * Accesses memory it shouldn't (areas not yet allocated, areas that have been freed, areas past the end of heap blocks, inaccessible areas of the stack).
 * Uses uninitialized values in dangerous ways.
 * Leaks memory.
 * Does bad frees of heap blocks (double frees, mismatched frees).
 * Passes overlapping source and destination memory blocks to `memcpy()` and related functions.

Memcheck reports these errors as soon as they occur, giving the source line number at which it occurred, and also a stack trace of the functions called to reach that line. Memcheck tracks valid addresses and initialization of values. As a result, it can detect the use of uninitialized memory. Memcheck runs programs about 10-30x slower than normal.

### Perparing your program
Compile your program with the following flags:
 * `-g` - to include debugging information so that Memcheck's error messages include *exact* line numbers. 
 * `-O0` - to turn off all optimizations. Memcheck's error messages can be slightly inaccurate at `-O1` and can lead to spurious errors at `-O2` or above. 

For this lab the command: 
```
$ g++ -g -O0 *.cpp -o <exec_name>
```
will be sufficient.

### Running your program under Memcheck
The examples/exercises in this lab don't use command line arguments, but if your program is called like this:
```
$ ./myprog arg1 arg2
```
Then run Valgrind like this:
```
$ valgrind --leak-check=full ./myprog arg1 arg2
```
Memcheck is the default tool so no tool flags are necessary. The `--leak-check` option turns on the detailed memory leak detector. 

Your program will run much slower (e.g. 20 - 30 times) than normal and use a lot more memory. Memcheck will issue messages about memory errors and leaks that it detects. 

## Examples
Let's go over some simple examples of the types of errors you can uncover using Valgrind and how to interpret the error messages. We're going to start with a simple c++ program (save the file as simple.cpp):
```
int main() {
    int *p = new int[10];
    p[10] = 1;

    return 0;
}
```
This is a pretty typical introduction to pointers example from CS012. It also contains a **very** common memory management mistake. On line 2 we allocated a contiguous location in memory large enough for 10 `int`s and assigned the starting address to an `int` pointer `p`. The next line then attempts to assign to the element with index 10. This may seem fine since it *seems* to be the last element in the array; however, due to 0-based addressing, the last element is actual `p[9]`.

Now, let's compile the program:
```
$ g++ -g -O0 -o example1 simple.cpp
```

And run it:
```
$ ./example1
```
You may get results similar to:
```
example1: malloc.c:2401: sysmalloc: Assertion `(old_top == initial_top (av) && old_size == 0) || ((unsign
ed long) (old_size) >= MINSIZE && prev_inuse (old_top) && ((unsigned long) old_end & (pagesize - 1)) == 0
)' failed.
Aborted (core dumped)
```
Or it might even run, either way figuring out your error is cryptic and difficult. Let's run it through Valgrind (you don't need to recompile):
```
$ valgrind --leak-check=full ./example1
```
And you'll get a much more in detailed report of what is going on:
```
==13621== Memcheck, a memory error detector
==13621== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==13621== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==13621== Command: ./example1
==13621== 
==13621== Invalid write of size 4
==13621==    at 0x108698: main (simple.cpp:3)
==13621==  Address 0x5b7dca8 is 0 bytes after a block of size 40 alloc'd
==13621==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==13621==    by 0x10868B: main (simple.cpp:2)
==13621== 
==13621== 
==13621== HEAP SUMMARY:
==13621==     in use at exit: 40 bytes in 1 blocks
==13621==   total heap usage: 2 allocs, 1 frees, 72,744 bytes allocated
==13621== 
==13621== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==13621==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==13621==    by 0x10868B: main (simple.cpp:2)
==13621== 
==13621== LEAK SUMMARY:
==13621==    definitely lost: 40 bytes in 1 blocks
==13621==    indirectly lost: 0 bytes in 0 blocks
==13621==      possibly lost: 0 bytes in 0 blocks
==13621==    still reachable: 0 bytes in 0 blocks
==13621==         suppressed: 0 bytes in 0 blocks
==13621== 
==13621== For counts of detected and suppressed errors, rerun with: -v
==13621== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```
Let's look at this bit by bit. 
```
==13621== Memcheck, a memory error detector
==13621== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==13621== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==13621== Command: ./example1
``` 
 * 13621 is the process ID; typically it is not important.
 * The rest shows some copyright and version information as well as the command Valgrind is running (your executable).

Now we get into the errors:
```
==13621== Invalid write of size 4
==13621==    at 0x108698: main (simple.cpp:3)
==13621==  Address 0x5b7dca8 is 0 bytes after a block of size 40 alloc'd
==13621==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==13621==    by 0x10868B: main (simple.cpp:2)
```
This shows us that the error was an `Invalid write of size 4` on line `(simple.cpp:3)` in function `main`. The address of the line (`0x108698`) as well as the address of the memory (`0x5b7dca8`) are shown here and they can be incredibly useful when debugging some really nasty memory bugs on the stack/heap, but they aren't helpful in this simple case. The rest of the error message will provide additional information that may help in identifying and fixing the error, in this case, a part of the root cause of the error comes from line (`simple.cpp:2`) with the `operator new[](unsigned long)`.

After this we get a `HEAP SUMMARY`:
```
==13621== HEAP SUMMARY:
==13621==     in use at exit: 40 bytes in 1 blocks
==13621==   total heap usage: 2 allocs, 1 frees, 72,744 bytes allocated
==13621== 
==13621== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==13621==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==13621==    by 0x10868B: main (simple.cpp:2)
```
This shows us the second error we have. We can see here that we have `40 bytes in 1 blocks` still *in use at exit*. Additionally, we had `2 allocs` but only `1 free`. There is memmory that was allocated but then did not have a corresponding deallocation. Looking at the records (second half of the message), we can see that `40 bytes in 1 block` are **definitely lost** in loss record 1 of 1. We are given some additional information to help track it down. `operator new[](unsigned long)` at `main (simple.cpp:2)` is at fault here. 

Finally, we are given a summary:
```
==13621== LEAK SUMMARY:
==13621==    definitely lost: 40 bytes in 1 blocks
==13621==    indirectly lost: 0 bytes in 0 blocks
==13621==      possibly lost: 0 bytes in 0 blocks
==13621==    still reachable: 0 bytes in 0 blocks
==13621==         suppressed: 0 bytes in 0 blocks
==13621== 
==13621== For counts of detected and suppressed errors, rerun with: -v
==13621== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```
We have `40 bytes in 1 blocks` **definitely lost** and nothing *indirectly*, or *possibly* lost, no memory *still reachable* and no errors were suppressed. Don't worry, the next examples will cover these types of errors as well. You can add to verbosity by adding additional `-v` flags, but the default typically has enough information to cover most errors. 

Let's fix the errors one at a time. First, let's get rid of the `Invalid write` since that is causing our program to crash. The fix is quite simple, figure out the size of the array, the last index in the array, and make sure you are in bounds of the array. 
```
int main() {
    int *p = new int[10];
    p[9] = 1; // Correct this to size - 1 = 10 - 1 = 9

    return 0;
}
```
And compile and run again (I add a trick I frequently use, the `&&` means "execute the second command **only if** the first command **succeeds**):
```
$ g++ -g -O0 simple.cpp -o example1 && ./example1
```
Now your program should run to completion (you won't see any output). Woohoo! You fixed it...wait, there were two errors weren't there? Let's run it back through Valgrind and see:
```
$ valgrind ./example1
```
And you'll see the following (tool information excluded for brevity): 
```
==14490== HEAP SUMMARY:
==14490==     in use at exit: 40 bytes in 1 blocks
==14490==   total heap usage: 2 allocs, 1 frees, 72,744 bytes allocated
==14490== 
==14490== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==14490==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==14490==    by 0x10868B: main (simple.cpp:2)
==14490== 
==14490== LEAK SUMMARY:
==14490==    definitely lost: 40 bytes in 1 blocks
==14490==    indirectly lost: 0 bytes in 0 blocks
==14490==      possibly lost: 0 bytes in 0 blocks
==14490==    still reachable: 0 bytes in 0 blocks
==14490==         suppressed: 0 bytes in 0 blocks
==14490== 
==14490== For counts of detected and suppressed errors, rerun with: -v
==14490== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
We can clearly see from our summaries that we are still *leaking* memory even though we don't have anymore write errors. We have the same error as above, but to remind us, we have `40 bytes in 1 blocks` **definitely lost** in record 1 due to the `operator new[](unsigned long)` on line (`simple.cpp:2`) in `main` function. Let's look at that code block (between `{` and `}`):
```
{
    int *p = new int[10];
    p[9] = 1;

    return 0;
}
```
The `new` operator allocates new memory, but we never deallocate that memory. Let's do that now:
```
{
    int *p = new int[10];
    p[9] = 1;

    delete p; // Deallocate the memory from the new above
    return 0;
}
```
*Yes I used the wrong delete but this was done on purpose, bear with me. If you didn't realize that was the wrong delete, that's exactly what this tool helps with so don't feel bad!*

### When a block is freed with an inappropriate deallocation function
 * If allocated with `malloc`, `calloc`, `realloc`, `valloc`, or `memalign`, you must deallocate with free\*.
 * If allocated with `new[]`, you must deallocate with `delete[]`.
 * If allocated with `new`, you must deallocate with `delete`.

\* These are common in `C` but less commonly seen in `C++`, so you likely haven't seen them much at this point.

Let's run this through Valgrind again:
```
==14593== Mismatched free() / delete / delete []
==14593==    at 0x4C3123B: operator delete(void*) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==14593==    by 0x10871E: main (simple.cpp:5)
==14593==  Address 0x5b7dc80 is 0 bytes inside a block of size 40 alloc'd
==14593==    at 0x4C3089F: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==14593==    by 0x1086FB: main (simple.cpp:2)
==14593== 
==14593== 
==14593== HEAP SUMMARY:
==14593==     in use at exit: 0 bytes in 0 blocks
==14593==   total heap usage: 2 allocs, 2 frees, 72,744 bytes allocated
==14593== 
==14593== All heap blocks were freed -- no leaks are possible
==14593== 
==14593== For counts of detected and suppressed errors, rerun with: -v
==14593== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
If we look at the summary it appears we freed all of the memory, however, if we look at the error list we will see that we used a `Mismatched free() / delete / delete []` at `operator delete(void*)` on line (`simple.cpp:5`) in the `main` function. And if we look at the additional information we can see the root is from `operator new[](unsigned long)` at line (`simple.cpp:2`) in the `main` function. Looking at these two operators as shown by Valgrind, we can see that we allocated with a `[]` but didn't deallocate with the same. Let's fix that now. 
```
{
    int *p = new int[10];
    p[9] = 1;

    delete[] p; // Deallocate the memory from the new above
    return 0;
}
```
And run Valgrind again:
```
==14620== HEAP SUMMARY:
==14620==     in use at exit: 0 bytes in 0 blocks
==14620==   total heap usage: 2 allocs, 2 frees, 72,744 bytes allocated
==14620== 
==14620== All heap blocks were freed -- no leaks are possible
==14620== 
==14620== For counts of detected and suppressed errors, rerun with: -v
==14620== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
Now we're at what is called **Memcheck-clean**, we have `0 bytes in 0 blocks` in use at exit, `All heap blocks were freed -- no leaks are possible` and we have `0 errors from 0 contexts`. This is the goal of any program we write.

The final step, now that we're at **Memcheck-clean**, is to re-run with optimizations turned back on:
```
$ g++ -g -O2 *.cpp -o example1
```

Congratulations! You've successfully cleaned up a simple program from all memory leaks using the Memcheck tool in the Valgrind suite.
### Illegal read/illegal write errors
The first example showed an example of an *Illegal write* error. If we had tried to print out the value 
```
cout << p[10] << endl;
```
We would have had a reported `Invalid read of size 4` at that line. The rest of that error message looks very similar to the `Invalide write` error. 

### Use of uninitialized values
As a general rule, you should always initialize your values. Nonetheless there are times that it happens (unintentional or not). Memcheck will catch these errors with a caveat. Let's look at the following program (`uninitialized.cpp`):
```
#include <iostream>
using namespace std;

int main() {
    int x;
    cout << x << endl;

    return 0;
}
```
Yes, it is again a silly examply, but it is used for illustrative purposes so bear with me. Let's compile and run this program:
```
$ g++ -g -O0 uninitialized.cpp -o example2 && ./example2
```
This should run and will print a number (often 0), **but** I want to emphasize that this should not be expected and is not because of the C++ standard. `x` is unintialized, and you may very well get a junk value. (I have graded a lot of assignments where students submitted programs that worked for them because their uninitialized variables happened to be zero.  When I graded them on my computer, they held junk values, and their programs failed.  The only way to be sure is to test your code under valgrind.) Now, let's run it through Valgrind and see what happens:
```
==14854== Conditional jump or move depends on uninitialised value(s)
==14854==    at 0x4F43B2A: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
==14854== 
==14854== Use of uninitialised value of size 8
==14854==    at 0x4F4362E: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F43B53: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
==14854== 
==14854== Conditional jump or move depends on uninitialised value(s)
==14854==    at 0x4F4363B: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F43B53: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
==14854== 
==14854== Conditional jump or move depends on uninitialised value(s)
==14854==    at 0x4F43B86: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
==14854== 
0
==14854== 
==14854== HEAP SUMMARY:
==14854==     in use at exit: 0 bytes in 0 blocks
==14854==   total heap usage: 2 allocs, 2 frees, 73,728 bytes allocated
==14854== 
==14854== All heap blocks were freed -- no leaks are possible
==14854== 
==14854== For counts of detected and suppressed errors, rerun with: -v
==14854== Use --track-origins=yes to see where uninitialised values come from
==14854== ERROR SUMMARY: 4 errors from 4 contexts (suppressed: 0 from 0)
```
Oh boy...that's a lot of errors, and they really don't seem friendly to look at. This is similar to compiler errors generated when using the Standard Template Library (STL), `cout` in this case. Let's take a look at the first error though:
```
==14854== Conditional jump or move depends on uninitialised value(s)
==14854==    at 0x4F43B2A: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
```
Okay...so there is a *Conditional jump or move* somewhere that depends on an *unitialised value(s)*. You may have learned about *jumps* in CS061 if you've taken that course so far but the important part here is that it *depends on an unitialised value(s)*. In fact, if we look at the second error we can see:
```
==14854== Use of uninitialised value of size 8
==14854==    at 0x4F4362E: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F43B53: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x4F50074: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.25)
==14854==    by 0x1088A2: main (uninitialized.cpp:6)
```
There it is, right at the top: `Use of unintialised value of size 8`. However, if we inspect the error message we see that it is at `???` by `std::ostreambuf...` by `std::ostream&...` by `main (uninitialized.cpp:6)`. That last *by...by...by* sequence is the stack trace. It's frequently a good idea to skip past all the library files and see what is the last function **you wrote** that caused that. We can see that in the `main` function on line 6 we set off this error. But we still don't know what caused it. We can see by inspecting line 6 `cout << x << endl;` that it is likely caused by the variable `x`, but that's about all we get. It turns out, we can add additional flags into the Valgrind call to get more out of this, specifically the `--track-origins=yes` flag:
```
$ valgrind --leak-check=full --track-origins=yes ./example2
```
Now let's look at that same error again (the second one):
```
==14904== Use of uninitialised value of size 8
==14904==    ...
==14904==    ...
==14904==    ...
==14904==    by 0x1088A2: main (uninitialized.cpp:6)
==14904==  Uninitialised value was created by a stack allocation
==14904==    at 0x10888A: main (uninitialized.cpp:4)
```
I've truncated the errors from the STL for brevity. You shouldn't need to dig into those in most errors. If you do, grab a cup of tea, clear your schedule for the week and get busy. Now we can see that the new flag added some additional information:
```
==14904==  Uninitialised value was created by a stack allocation
==14904==    at 0x10888A: main (uninitialized.cpp:4)
```
The variable at fault here was created by stack allocation in the `main` function from (`unitialized.cpp:4`). After some inspection of that function we can see that the variable `x` is at fault and can fix it by simply initializing it to some value, let's say 10.
```
int x = 10;
```
Now, compiling and running Valgrind again we can see that we achieved **Memcheck-clean**.
```
==14973== HEAP SUMMARY:
==14973==     in use at exit: 0 bytes in 0 blocks
==14973==   total heap usage: 2 allocs, 2 frees, 73,728 bytes allocated
==14973== 
==14973== All heap blocks were freed -- no leaks are possible
==14973== 
==14973== For counts of detected and suppressed errors, rerun with: -v
==14973== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
It is important to note here that Valgrind will let your program copy around junk data as much as it likes. Memcheck will observe and keep track of the data, but doesn't complain. It doesn't complain until the use of uninitialized data might affect your program's externally-visible behaviour. Experiment with the following program to see how the generated reports look:
```
#include <iostream>
using namespace std;

int main() {
    int x;
    bool z;
    int y = x + 5;
    if (x) {
        cout << "X is non-zero" << endl;
    }
    if (z) {
        cout << "Z is truthy" << endl;
    }
    cout << y << endl;
    return 0;
}
```

Another interesting example is this one:
```
#include <iostream>

void foo(int x)
{
    std::cout<<x<<std::endl;
}

int main()
{
    int x;
    int r=4;
    int y=x+r;

    foo(y);
    
    return 0;
}
```
When I run this example, the valgrind report flags line 5 (the std::cout line), line 14 (the call to foo) in the stack trace.  It points to line 9 (the open brace on the main function) as the origin.  The origin is actually line 10, where the x is declared.  Note that the test for uninitialized values occurs when the value is used in a conditional, not on arithematic.  Thus, valgrind will not flag the computation of y.  A handy debugging trick is to insert "dummy" conditions to trigger the test, allowing you to test variables one by one to track down the problem.
```
#include <iostream>

void foo(int x)
{
    std::cout<<x<<std::endl;
}

int zzzz=0; // dummy variable
int main()
{
    int x;
    int r=4;
    int y=x+r;
    if(x) zzzz++; // uninitialized error reported here
    if(r) zzzz++;
    if(y) zzzz++; // uninitialized error reported here

    foo(y);
    
    return 0;
}
```

### Illegal frees
Memcheck will also track the memory that has been deallocated so if you try to re-deallocate memory (as in a double free) it will catch that and report it to you. Consider the following program (doubleFree.cpp):
```
int main() {
    int *p = new int(10);

    delete p;
    delete p;

    return 0;
}
```
By inspection it is easy to see that `p` has been `delete`d twice, but let's run it through Valgrind anyways:
```
==15125== Invalid free() / delete / delete[] / realloc()
==15125==    at 0x4C3123B: operator delete(void*) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==15125==    by 0x108727: main (doubleFree.cpp:5)
==15125==  Address 0x5b7dc80 is 0 bytes inside a block of size 4 free'd
==15125==    at 0x4C3123B: operator delete(void*) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==15125==    by 0x108716: main (doubleFree.cpp:4)
==15125==  Block was alloc'd at
==15125==    at 0x4C3017F: operator new(unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==15125==    by 0x1086FB: main (doubleFree.cpp:2)
==15125== 
==15125== 
==15125== HEAP SUMMARY:
==15125==     in use at exit: 0 bytes in 0 blocks
==15125==   total heap usage: 2 allocs, 3 frees, 72,708 bytes allocated
==15125== 
==15125== All heap blocks were freed -- no leaks are possible
==15125== 
==15125== For counts of detected and suppressed errors, rerun with: -v
==15125== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
The first (and only) error shows us that we have an `Invalid free() / delete / delete[] / realloc()` at `operator delete(void*)` on line (`doubleFree.cpp:5`) in the `main` function. This memory was `free'd` by `operator delete(void*)` on line (`doubleFree.cpp:4`) in the `main` function. The block was also originally `alloc'd` at `operator new(unsigned long)` on line `doubleFree.cpp:2` in the `main` function. We can get to **Memcheck-clean** by simply removing either of the `delete p;` statements.


### Memory leak detection
Memcheck has the following four leak kinds:
 * "Still reachable" - A start-pointer or chain of start-pointers to the block is found. The program could have *in theory* deallocated the memory.
 * "Definitely lost" - No pointer to the block can be found. There is no possible way to have deallocated this memory before the program exited. 
 * "Indirectly lost" - The block is lost, not because there are no pointers to it, but rather because all the blocks that point to it are themselves lost. For example, if you have a binary tree and the root node is lost, all it's children are indirectly lost. If you fixed the "definitely lost" block correctly these will be fixed as a side effect.
 * "Possibly lost" - A chain of one or more pointers to the block have been found, but at least one of them is an *interior* pointer. This is typically not good unless, through inspection, you identify the interior pointer and now how to access this memory. 

A **start-pointer** is a pointer at the beginning of a block whereas an **interior-pointer** points somewhere in the middle of the block (intentionally or unintentionally), for example:
```
int *p = new int[10]
int *ip = p + 2;
```
 * `p` points to the beginning of the array and is a start-pointer
 * `ip` points to the middle of the array and is an interior-pointer

There are many more kinds of interior pointers and you can read more about them on your own.

## Lab exercise
Now use what you have learned from this lab to get the `lineage` program to **Memheck-clean**. The program is a simplified family tree manager. It maintains a list of `Person`s in a `PersonList`. Each `Person` object maintains a set of pointers to his/her parents as well as to his/her children. The code contains several memory leaks. To compile:
```
$ g++ -g -O0 -fno-inline *.cpp -o lineage
```
\* The -fno-inline instructs the compiler to not inline any functions and makes it easier to see the function call chain. 

Some things to consider:
 * As with compiler errors, it's a good idea to start at the top and see if fixing those errors fix later errors. 
 * Sometimes by fixing an error you *introduce* more. These aren't new errors, they were just revealed by fixing an earlier error. 
 * Think carefully about what `delete` vs. `delete[]` do and why we spent time in CS014 discussing *shallow* vs. *deep* copies.
 * If you get to only (or mostly) "indirectly lost" errors you'll notice that Valgrind doesn't report each one individually. This is because they aren't "lost" so much as "forgotten" by the programmer. If this happens you can turn on `--show-reachable=yes` to show more details about each of those errors.
 
