#include <stdio.h>
#include <stdlib.h>

int input_integer() {
  char buffer[16];
  printf("Enter an integer: ");
  fflush(stdout);
  if (!fgets(buffer, sizeof(buffer), stdin)) {
    perror("fgets");
    exit(1);
  }
  return atoi(buffer);
}

void output_integer(int n) {
  printf("%d\n", n);
}

void output_double(double x) {
  printf("%f\n", x);
}
