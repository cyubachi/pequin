#include <stdint.h>

//Constants
#define SIZE 32

struct In {
  int16_t props[SIZE]; 
};

struct Out {
  int64_t props[SIZE];
};

int compute(struct In *input, struct Out *output) {
  int i,j,k;

  for(i = 0; i < SIZE; i++){
    output->props[i] = input->props[i];
  }
}

