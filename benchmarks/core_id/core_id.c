#include <stdio.h>
//#include <stdint.h>

register int x asm("s2");

int main(void) {
  printf("Core id = \n");

  /*register long a7 asm("a7") = 1234;
  register long a0 asm("a0") = 0;
  register long a1 asm("a1") = 0;
  register long a2 asm("a2") = 0;
  asm volatile ("scall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7));
  */
  //asm ("csrr s2, mcpuid");
  //asm volatile ("csrrc a0, stats, 1" ::: "a0");


  printf("%x - ", x);
  x = 0;
  printf("%x\n", x);
  //asm volatile("eret");
  return 0;
}
