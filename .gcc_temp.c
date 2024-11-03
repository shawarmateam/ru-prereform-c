#include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 int main() { srand(time(NULL)); int random = (rand() % 8)+1; if (random == 8) { system("echo \"test\""); } return 0; } 