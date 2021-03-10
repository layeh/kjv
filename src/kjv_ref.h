#pragma once

#include <regex.h>

#include "intset.h"

#define KJV_REF_SEARCH 1
#define KJV_REF_EXACT 2
#define KJV_REF_EXACT_SET 3
#define KJV_REF_RANGE 4
#define KJV_REF_RANGE_EXT 5

typedef struct kjv_ref {
    int type;
    unsigned int book;
    unsigned int chapter;
    unsigned int chapter_end;
    unsigned int verse;
    unsigned int verse_end;
    intset *verse_set;
    char *search_str;
    regex_t search;
} kjv_ref;

kjv_ref *
kjv_newref();

void
kjv_freeref(kjv_ref *ref);

int
kjv_parseref(kjv_ref *ref, const char *ref_str);
