#pragma once

#include <stdbool.h>

typedef struct {
    bool highlighting;
    bool pretty;

    int maximum_line_length;

    int context_before;
    int context_after;
    bool context_chapter;
} kjv_config;
