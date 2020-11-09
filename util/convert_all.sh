# Make sure that the PATH includes 3c
# If necessary, set CPATH to point to checked C headers

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
