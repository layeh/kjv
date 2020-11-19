#pragma once

typedef struct {
    char *book_name;
    char *book_abbr;
    int book;
    int chapter;
    int verse;
    char *text;
} kjv_verse;

extern kjv_verse kjv_verses[];

extern size_t kjv_verses_length;
