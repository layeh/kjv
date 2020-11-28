#pragma once

typedef struct {
    int number;
    char *name;
    char *abbr;
} kjv_book;

typedef struct {
    int book;
    int chapter;
    int verse;
    char *text;
} kjv_verse;

extern kjv_verse kjv_verses[];

extern int kjv_verses_length;

extern kjv_book kjv_books[];

extern int kjv_books_length;
