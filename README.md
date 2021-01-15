# Converting tiny-bignum-c

## Purpose
This repository serves as a workflow demonstration of [3C](https://github.com/correctcomputation/checkedc-clang), a tool that automatically converts C code to [Checked C](https://github.com/Microsoft/checkedc-clang/wiki) code.

### Checked C

Checked C is an extension to C which aims to ensure [spatial memory safety](http://www.pl-enthusiast.net/2014/07/21/memory-safety/). It achieves this by supporting three new *checked* pointer types:
- `_Ptr<`*T*`>` is either NULL or points to a single object of type *T*
- `_Array_ptr<`*T*`>` is either NULL or points to an array of zero or more objects of type *T*
- `_Nt_array_ptr<`*T*`>` is either NULL or points to an array of zero or more objects of type *T* up to a NULL terminator

The latter two types are associated with *bounds* declarations, that define how long the pointed-to array can be. For example, `_Array_ptr<int> x : count(10);` is a declaration of variable `x` which is a pointer to an array of size 10.

Programmers can annotate regions of code as *checked*; e.g., a code block `{ ... }` can be marked `_Checked { ... }`. Checked regions restrict their contents so as to ensure spatial safety; e.g.,  regions may *only* contain checked pointers. Regions of code deemed to be *unchecked* may freely intermix checked and legacy pointers. This flexibility permits a partial, incremental conversion of programs. 

### 3C

3C analyzes a codebase to determine how its pointers are used, and whether they are used safely. Safely used pointers have their declarations rewritten so as to have checked types. For example, a pointer declared `int *p = &x` might get rewritten to `_Ptr<int> p = &x`. 

3C was designed to support *iterative* conversion. 3C is unlikely to completely and correctly convert a whole program in one go, so programmers are encouraged to make additional changes to the converted code. Once they have, they can run that code through 3C again.

This demo therefore takes place over a number of commits, each of which represents the project after a step in the process. Commits will be commented as "3c_commit" or "manual_commit" based on whether they were automatically or manually updated, respectively.

## Install

You will need to [install 3C](https://github.com/correctcomputation/checkedc-clang/blob/main/clang/docs/checkedc/3C/INSTALL.md) and Checked C's `clang` compiler before beginning. Since 3C's repository also includes the `clang` compiler, you can build both when installing 3C. Follow the instructions linked above and also build TARGET `clang` (along with target `3c`). 

Make sure to clone the `checkedc` project into the `checkedc-wrapper` directory as in the instructions. There are important header files that 3C needs. Note the location of the `build/bin` directory, which might change if you use an IDE. Add the build directory to your `PATH` so that both `3c` and `clang` are available.

## Getting the Repo Ready for the Demo

We start from a fork of the [tiny-bignum project](https://github.com/kokke/tiny-bignum-c). This is a small library with multiple test programs. The rest of this README describes the changes we made starting at [this commit](https://github.com/correctcomputation/checkedc-tiny-bignum-c/commit/1d7a1f9b8e77316187a6b3eae8e68d60a6f9a4d4) (the root of the fork). If you want to go through the tutorial yourself, we recommend cloning this repo and then going to [this commit](https://github.com/correctcomputation/checkedc-tiny-bignum-c/commit/d41d44f2d208fd8f0076cff0b74752d3c60a751c), which corresponds to the beginning of the *Beginning the Conversion* section below. Save this README, though, as it was added to the repo after the work it describes.

Starting from the initial fork, we made some changes to the code so that it is easier demo. In particular:

- The repository has a library, in files `bn.c` and `bn.h`, that implements bignums. It has several test applications, each written as a separate executable with its own `main` function. We have renamed these `main` functions and created a file `alltests.c` that invokes each of them according to a flag. Thus we have one test executable but many ways of running it.
- The file `scripts/test_rand.py` had some indenting issues that prevented it from running on our Linux platform.

We commit these changes with the comment "Merged all test executables into a single one."

Then we made some changes to ease our use of 3C. In particular, we added some scripts for running 3C, in the `util` directory.

- The script `convert_all.sh` runs 3C on the entire codebase.
- The script `update_all.sh` replaces the original `.c` and `.h` files with the converted versions.
- The script `replace.py` is called by `update_all.sh` to replace the `.c` and `.h` files.

We will say more about these scripts as we go. They can be adapted to work with other projects.

- We changed the `Makefile` to build the test executable. We also changed the `CC` variable to `clang` from `gcc`; we'll run it with our `PATH` set so that `clang` refers to the Checked C compiler.

We commit these changes with the comment "manual\_commit - setup".

## Beginning the Conversion

The first step before running 3C on the code is to change it to refer to the Checked C versions of the standard header files. This way, 3C will consider the Checked C types for these standard functions when carrying out conversion.

Inside the C files to convert, we change the `#include`s to refer to Checked C versions. Most of these have the suffix `_checked`, e.g., `#include <stdio.h>` becomes `#include <stdio_checked.h>`. We continue with `stdio`, `assert`, `stdlib`, and `string` in the project `.c` and `.h` files, including those in the `tests` directory.

We commit these changes with comment "manual\_commit - set checked headers".

## Initial Conversion

Now we are ready to run 3C. To do that we execute the script `util/convert_all.sh`. If we look at the contents of the script, we can see that it contains the following:
```
3c -alltypes -warn-root-cause \
-output-postfix=checked \
-extra-arg-before=-Wall \
-extra-arg-before=-Wextra \
-extra-arg-before=-I. \
./bn.c \
tests/alltests.c \
tests/hand_picked.c \
tests/rsa.c \
tests/factorial.c \
tests/load_cmp.c \
tests/test_div_algo.c \
tests/golden.c \
tests/randomized.c
```
This is a single command that executes `3c` on `bn.c`, the main library file, and on `tests/*.c`, the various test files. In the process of considering all of these files together, it will also consider `bn.h`, since the files `#include` it. There are several options we are passing to `3c`.

- `-alltypes` directs `3c` to attempt infer `_Array_ptr` and `_Nt_array_ptr` types, and their bounds. If you do not pass in `-alltypes` then only `_Ptr` types are inferred.
- `-warn-root-cause` asks `3c` to output the root causes that pointers cannot be made checked
- `-output-postfix=checked` tells `3c` to not rewrite the original `.c` or `.h` file, but instead to output a different version with postfix `checked`. E.g., `bn.c` is output to `bn.checked.c` and `bn.h` is output to `bn.checked.h`.
- `-extra-arg-before` is a directive used with `clang`-based tools (which `3c` is). It says that the given argument should be passed to the `clang` portion of `3c` before running the `3c`-specific portion. The three uses of the argument here say to run the typechecker with extra warnings and to consider the current directory for `#include`s. 

After running this script, we will see `.checked` versions of the library in the current directory, and `.checked` versions of the `.c` files in the `tests` directory. Then we can run `util/update_all.sh` to move the checked versions over-top of the originals. Now, if we like, we can see the effect of the conversion by doing `git diff`. If we wanted to recover overwritten files we could use `git reset`. 

Note that there may be some errors about missing compilation databases. These may be ignored, since we are not using a database for this small project. There may be some warnings about root causes. These are potential conversions that did not happen. We deal with them later.

We commit these changes with comment "3c\_commit - initial"

## Initial Manual Adjustments

At this point we can see all the changes `3c` made with the command `git diff`. You'll see that most of the changes involve `struct bn*` being changed to `_Ptr<struct bn>`. Arrays have the `_Nt_checked` keyword added to ensure runtime bounds checks, and all checked pointers are initialized.

If we try to compile the code at this point, using Checked C's `clang`, it will not work. Typing `make` we will see errors like these:
```
tests/randomized.c:19:19: error: expression has unknown bounds
  int oper = atoi(argv[1]);
                  ^~~~~~~
tests/randomized.c:39:26: error: expression has unknown bounds
  bignum_from_string(&a, argv[2], strlen(argv[2]));
                         ^~~~~~~
...
```
Looking at `tests/alltests.c` and `tests/randomized.c`, it turns out that the latter's `test_main` function lacks a bounds annotation; it should be that `argv`'s bound is `count(argc)`. The reason is that the call to `randomized_main` on line 29 of `tests/alltests.c` refers to `argc-1` and `argv+1`; this sort of arithmetic is not something 3C tries to understand. The problem is easily fixed: We can add the needed annotation to the prototype in `tests/alltests.c` (so that the parameter list matches the one for `main`) and the definition in `tests/randomized.c`.

Another current weak point in 3C is macro conversion. Sometimes, macro-defined constants are inlined. We notice this change in `bn.h` and restore it manually, updating `struct bn` to include `DTYPE array _Checked[BN_ARRAY_SIZE];`.

With these changes the code builds successfully when running `make`, though it issues a couple of warnings. The tests also run as expected, which we can see by doing `make test`. So our partial conversion is successful. 

We commit this change as "manual\_commit - fix argc, macros"

## Removing itypes, and working around Checked C bug

If we look at `bn.h` and `bn.c`, we will notice that not every pointer is a fully checked pointer. In particular, the 2nd parameter of `bignum_to_string` has a mixed type, called an interop type:
```
char *str : itype(_Nt_array_ptr<char>)
```
This means that the function is internally unsafe (regular type), but can be used without marking the calling function as unsafe (itype). 3C does this to localize unsafe behavior and continue converting the rest of the code. For this reason, the rest of the code is already converted, and we will not need to run 3C again.

We want our entire codebase to run with Checked C enhancements, so we can force this by manually changing the type (in both the definition and the prototype) to the contents of the itype:
```
void bignum_to_string(_Ptr<struct bn> n, _Nt_array_ptr<char> str, int nbytes)
```
This will compile, be we expect a problem: The lack of a bound on `str` will mean that accesses to `str` in `bignum_to_string` will induce a bounds-check failure. We see this while testing: `make: *** [test] Illegal instruction: 4`. This unfortunatly described error means that the default size of the array was not large enough for an operation, and Checked C quit rather than propagate an out of bounds exploit. But the default size is not the given size. The bound for `str` is apparent from looking at the code: It is `nbytes`. So we can change the various declarations to
```
void bignum_to_string(_Ptr<struct bn> n, _Nt_array_ptr<char> str : count(nbytes), int nbytes)
```

However, if we try to compile the converted program now, it doesn't work. Here's an example error:
```
tests/golden.c:260:29: error: argument does not meet declared bounds for 2nd
      parameter
      bignum_to_string(&sa, buf, sizeof(buf));
                            ^~~
tests/golden.c:260:29: note: destination bounds are wider than the source bounds
tests/golden.c:260:29: note: destination upper bound is above source upper bound
tests/golden.c:260:29: note: (expanded) expected argument bounds are
      'bounds(buf, buf + (int)sizeof (buf))'
tests/golden.c:260:29: note: (expanded) inferred bounds are
      'bounds(buf, buf + 8191)'
      bignum_to_string(&sa, buf, sizeof(buf));
```
Notice the reference to `8191` -- this is an off-by-one error in the Checked C typechecker! We can work around this problem by replacing `sizeof(buf)` with `sizeof(buf)-1` in various places. We also need to offset the corresponding assert in `bignum_to_string`, changing it to `((nbytes + 1) & 1) == 0`.

All tests pass again so we commit this change as "manual\_commit - itypes, workaround sizeof()"

## Polishing off bn.c and bn.h

Ultimately, we would like all code to be considered as within a checked region. For this to be true, all pointers must be checked pointers, and certain programming idioms, such as unsafe casts, must be disallowed.

Adding `#pragma CHECKED_SCOPE on` to the beginning of a file forces all code within it be considered as within a checked region. In a header file, this extends to any others that include it unless wrapped in

```
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on
 ...code...
#pragma CHECKED_SCOPE pop
```
Running `make` with the pragma added to the top of `bn.c` and `bn.h`, the Checked C `clang` produces errors relating to functions `bignum_from_string` and `bignum_to_string`. These use variadic functions `sscanf` and `sprintf`, respectively, which need to be replaced or put in an *unchecked* block, which allows for more relaxed processing but with fewer guarantees about safe usage.  We can indicate these a statements are unchecked by wrapping then in `_Unchecked {...}`. Doing this, the code compiles and runs properly.

We commit this change to "manual\_commit - checked_scope".

## Coda: The dangers of unchecked code

It turns out there is a problem hidden in the last step we just carried out, in particular, in carrying out the unsafe call to `sscanf`. If we look carefully at this call, within the `bignum_from_string` function, we see the following:
```
void bignum_from_string(_Ptr<struct bn> n, _Nt_array_ptr<char> str, int nbytes)
{
  ...
  int i = nbytes - (2 * WORD_SIZE); /* index into string */
  ...
  _Unchecked { sscanf(&str[i], ((_Nt_array_ptr<const char>)SSCANF_FORMAT_STR), &tmp); }
```
There's a problem here. The declaration of `str` indicates no particular bounds; this means those bounds default to 0. But then the call to `sscanf` seems to be indexing `&str[i]` where `i` is nonzero. Why is this not resulting in a run-time exception? Two reasons. First, `&str[i]` does not induce a bounds check. If we were to add the following line just above the `_Unchecked` block, we'd get the run-time failure we expect. 
```
  str[i] = str[i]; // compilation fails because i > 0, but count(str) = 0
```
Second, `&str[i]` is equivalent to `str+i`, which is itself a `_Nt_array_ptr<char>` whose bounds are displaced by `i`. Since they started out as zero, this might be a problem, but it turns out that the prototype for `sscanf` in `stdio_checked.h` allows its argument to have no bounds. Hence, no problem is detected. Here there is indeed no issue. But what if we changed the `sscanf` call to be
```
  _Unchecked { sscanf(&str[10000], ((_Nt_array_ptr<const char>)SSCANF_FORMAT_STR), &tmp); }
```
The Checked C compiler does not complain, and when we run this code, it will likely crash.

We can fix this problem by updating `bignum_from_string` so that its `str` parameter has bounds `count(nbytes)`. Doing so causes calls to `bignum_from_string` from `tests/randomized.c` to fail because the Checked C compiler does not understand that `strlen` determines the length of the `argv` strings being passed in. We need to add some Checked C bounds casts to specify this. Once we do, the program compiles and runs properly (and still does even if we added the `str[i] = str[i]` line above).

We commit this change as "manual\_commit - assign bounds from strlen"
