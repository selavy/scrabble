#include "letter_bag_type.h"
#include <stdlib.h>
#include <time.h>

void letter_bag_create(struct letter_bag_t * letter_bag, frequency_t freq) {
  int i = 0;
  int j = 0;
  int f = 0;
  tile_t * arr = 0;
  arr = &(letter_bag->bag[0]);

  // i -> iterate over letter types
  // j -> iterate over letter_bag->bag
  // f -> iterate to insert freq[i] number of i letter
  for (i = 0, j = 0; i < NUM_LETTERS && j < MAX_LETTERS; ++i) {
    // TODO: replace with memset?
    // memset(arr, i, sizeof(*arr) * freq[i]);
    for (f = 0; f < freq[i]; ++f) {
      if (i == BLANK) {
	arr[j++] = BLANK_BIT;
      } else {
	arr[j++] = i;
      }
    }
  }

  // randomly swap elements to shuffle the tiles
  // i -> iterate over letter_bag
  // f -> position to swap with letter_bag->bag[i]
  // j -> tmp var to allow swap
  //  srand(time(0));
  srand(5);
  for (i = 0; i < MAX_LETTERS; ++i) {
    f = rand() % MAX_LETTERS;
    // swap
    j = arr[i];
    arr[i] = arr[f];
    arr[f] = j;
  }

  letter_bag->letters = MAX_LETTERS;
}

tile_t letter_bag_draw_letter(struct letter_bag_t * letter_bag) {
  if (letter_bag->letters == 0) {
    return ERROR;
  }
  return letter_bag->bag[--letter_bag->letters];
}

void letter_bag_destroy(struct letter_bag_t * letter_bag) {
  // empty
}

void letter_bag_print(struct letter_bag_t * letter_bag) {
  #ifdef _DEBUG
  int i = 0;
  tile_t * arr = &(letter_bag->bag[0]);
  for (; i < MAX_LETTERS; ++i) {
    printf("%d ", arr[i]);
  }
  #endif
}
