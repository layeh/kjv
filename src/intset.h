#pragma once

#include <stdbool.h>

typedef struct intset intset;

intset *intset_new();

void intset_free(intset *set);

void intset_add(intset *set, int item);

bool intset_contains(intset *set, int item);
