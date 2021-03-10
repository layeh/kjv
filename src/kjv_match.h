#pragma once

#include "kjv_config.h"
#include "kjv_ref.h"

typedef struct {
    int start;
    int end;
} kjv_range;

typedef struct {
    int current;
    int next_match;
    kjv_range matches[2];
} kjv_next_data;

int
kjv_next_verse(const kjv_ref *ref, const kjv_config *config, kjv_next_data *next);
