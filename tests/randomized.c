#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <assert_checked.h>
#include <string_checked.h>
#include "bn.h"

enum { ADD, SUB, MUL, DIV, AND, OR, XOR, POW, MOD, RSHFT, LSHFT, ISQRT };

int randomized_main(int argc, _Array_ptr<_Nt_array_ptr<char>> argv : count(argc))
{

  if (argc < 5)
  {
    printf("ERROR\n\nUsage:\n    %s [oper] [operand1] [operand2] [result]\n\nWhere oper means:\n    0 = add, 1 = sub, 2 = mul, 3 = div\n\nExample:\n    %s 3 0100 80 02\n\n    [divide 0x0100 by 0x80 and expect 0x02 as result] \n\n", argv[0], argv[0]);

    return -1;
  }  

  int oper = atoi(argv[1]);

/*
  printf("program  = %s \n", argv[0]);
  printf("operator = %s [%d]\n", argv[1], oper);
  printf("operand1 = %s \n", argv[2]);
  printf("operand2 = %s \n", argv[3]);
  printf("expected = %s \n", argv[4]);
*/

  struct bn a = {};
struct bn b = {};
struct bn c = {};
struct bn res = {};


  bignum_init(&a);
  bignum_init(&b);
  bignum_init(&c);
  bignum_init(&res);
  {
    int l = strlen(argv[2]);
    _Nt_array_ptr<char> arg : count(l) =
      _Assume_bounds_cast<_Nt_array_ptr<char>>(argv[2],count(l));
    bignum_from_string(&a, arg, l);
  }
  {
    int l = strlen(argv[3]);
    _Nt_array_ptr<char> arg : count(l) =
      _Assume_bounds_cast<_Nt_array_ptr<char>>(argv[3],count(l));
    bignum_from_string(&b, arg, l);
  }
  {
    int l = strlen(argv[4]);
    _Nt_array_ptr<char> arg : count(l) =
      _Assume_bounds_cast<_Nt_array_ptr<char>>(argv[4],count(l));
    bignum_from_string(&c, arg, l);
  }

  struct bn a_before = {};
struct bn b_before = {};

  bignum_assign(&a_before, &a);
  bignum_assign(&b_before, &b);


  switch (oper)
  {
    case ADD:   bignum_add(&a, &b, &res);   break;
    case SUB:   bignum_sub(&a, &b, &res);   break;
    case MUL:   bignum_mul(&a, &b, &res);   break;
    case DIV:   bignum_div(&a, &b, &res);   break;
    case AND:   bignum_and(&a, &b, &res);   break;
    case OR:    bignum_or (&a, &b, &res);   break;
    case XOR:   bignum_xor(&a, &b, &res);   break;
    case POW:   bignum_pow(&a, &b, &res);   break;
    case MOD:   bignum_mod(&a, &b, &res);   break;
    case ISQRT: bignum_isqrt(&a, &res);     break;
    case RSHFT:
    {
      bignum_rshift(&a, &res, bignum_to_int(&b));
    } break;
    case LSHFT: 
    {
      bignum_lshift(&a, &res, bignum_to_int(&b));
    } break;
    

    default:
      printf("default switch-case hit: unknown operator '%d' \n", oper);
      assert(0); 
  }

  int cmp_result = (bignum_cmp(&res, &c) == EQUAL);

  if (!cmp_result)
  {
    char buf _Nt_checked[8192];
    bignum_to_string(&res, buf, sizeof(buf)-1);
    printf("\ngot %s\n", buf);
    printf(" a  = %d \n", bignum_to_int(&a));
    printf(" b  = %d \n", bignum_to_int(&b));
    printf("res = %d \n", bignum_to_int(&res));
    printf("\n");
    return 1;
  }

  assert(bignum_cmp(&a_before, &a) == EQUAL);
  assert(bignum_cmp(&b_before, &b) == EQUAL);

  return 0;
}


