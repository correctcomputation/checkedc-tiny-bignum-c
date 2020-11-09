CC     := clang
TSTF   := tests/alltests.c tests/golden.c tests/hand_picked.c tests/load_cmp.c tests/factorial.c tests/randomized.c tests/rsa.c tests/test_div_algo.c
MACROS := 
CFLAGS := -I. -Wundef -Wall -Wextra -O3 $(MACROS)


all:
	$(CC) $(CFLAGS) bn.c $(TSTF)      -o ./build/alltests


test:
	@echo
	@echo ================================================================================
	@./build/alltests 4
	@echo ================================================================================
	@./build/alltests 3
	@echo ================================================================================
	@./build/alltests 2
	@echo ================================================================================
	@python ./scripts/fact100.py
	@./build/alltests 5
	@python ./scripts/test_old_errors.py
	@echo ================================================================================
	@./build/alltests 1
	@echo ================================================================================
	@./build/alltests 0
	@echo ================================================================================
	@python ./scripts/test_rand.py 1000
	@echo ================================================================================
	@echo

clean:
	@rm -f ./build/alltests


