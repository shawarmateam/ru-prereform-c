#include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 int main() { char test = 'A'; switch (test) { case 'a': printf("test == 'a'\n"); break; case 'b': printf("test == 'b'\n"); break; default: printf("test != 'b' && test != 'a'\n"); } return 0; } 