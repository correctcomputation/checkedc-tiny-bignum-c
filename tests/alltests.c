#include <stdio_checked.h>
#include <stdlib_checked.h>

extern int test_div_main();
extern int rsa_main();
extern int load_cmp_main();
extern int hand_picked_main();
extern int golden_main();
extern int factorial_main();
extern int randomized_main(int argc, _Array_ptr<_Nt_array_ptr<char>> argv : count(argc));

int main(int argc, _Array_ptr<_Nt_array_ptr<char>> argv : count(argc)) {
  if (argc < 2) 
  {
    printf("ERROR\n\nUsage:\n    %s n ...\n", argv[0]);
    printf("      where n is between 0 and 6.\n");
    return -1;
  }
  int n = atoi(argv[1]);
  switch (n) {
  case 0: return test_div_main();
  case 1: return rsa_main();
  case 2: return load_cmp_main();
  case 3: return hand_picked_main();
  case 4: return golden_main();
  case 5: return factorial_main();
  case 6:
    argv[1] = argv[0];
    return randomized_main(argc-1,argv+1);
  }
  printf ("n=%d not between 0 and 6\n", n);
  return -1;
}
