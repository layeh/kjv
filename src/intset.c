#include <stdlib.h>
#include <stdbool.h>

#include "intset.h"

typedef struct intset {
    int *arr;
    size_t length;
    size_t capacity;
} intset;

intset *
intset_new()
{
    intset *set = malloc(sizeof(intset));
    set->arr = malloc(sizeof(int) * 10);
    set->length = 0;
    set->capacity = 10;
    return set;
}

void
intset_free(intset *set)
{
    if (set) {
        free(set->arr);
        free(set);
    }
}

static int
intset_cmp(const void *a, const void *b)
{
    int x = *(int *)a, y = *(int *)b;
    if (x < y) {
        return -1;
    } else if (x > y) {
        return 1;
    }
    return 0;
}

void
intset_add(intset *set, int item)
{
    if (intset_contains(set, item)) {
        return;
    }

    if (set->length + 1 > set->capacity) {
        set->capacity *= 2;
        set->arr = realloc(set->arr, sizeof(int) * set->capacity);
    }
    set->arr[set->length] = item;
    set->length++;
    qsort(set->arr, set->length, sizeof(int), intset_cmp);
}

bool
intset_contains(intset *set, int item)
{
    void *found = bsearch(&item, set->arr, set->length, sizeof(int), intset_cmp);
    return found != NULL;
}
