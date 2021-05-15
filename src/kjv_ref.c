#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kjv_data.h"
#include "kjv_ref.h"

kjv_ref *
kjv_newref()
{
    return calloc(1, sizeof(kjv_ref));
}

void
kjv_freeref(kjv_ref *ref)
{
    if (ref) {
        free(ref->search_str);
        regfree(&ref->search);
        free(ref);
    }
}


static bool
kjv_bookequal(const char *a, const char *b, bool short_match)
{
    for (size_t i = 0, j = 0; ; ) {
        if ((!a[i] && !b[j]) || (short_match && !b[j])) {
            return true;
        } else if (a[i] == ' ') {
            i++;
        } else if (b[j] == ' ') {
            j++;
        } else if (tolower(a[i]) != tolower(b[j])) {
            return false;
        } else {
            i++;
            j++;
        }
    }
}

static bool
kjv_book_matches(const kjv_book *book, const char *s)
{
    return kjv_bookequal(book->name, s, false) ||
        kjv_bookequal(book->abbr, s, false) ||
        kjv_bookequal(book->name, s, true);
}

static int
kjv_book_fromname(const char *s)
{
    for (int i = 0; i < kjv_books_length; i++) {
        const kjv_book *book = &kjv_books[i];
        if (kjv_book_matches(book, s)) {
            return book->number;
        }
    }
    return 0;
}

static int
kjv_scanbook(const char *s, int *n)
{
    int i;
    int mode = 0;
    for (i = 0; s[i]; i++) {
        if (s[i] == ' ') {
            continue;
        } else if (('a' <= s[i] && s[i] <= 'z') || ('A' <= s[i] && s[i] <= 'Z')) {
            mode = 2;
        } else if ('0' <= s[i] && s[i] <= '9' && 0 <= mode && mode <= 1) {
            mode = 1;
        } else {
            break;
        }
    }
    *n = i;
    return mode >= 1;
}

int
kjv_parseref(kjv_ref *ref, const char *ref_str)
{
    // 1. <book>
    // 2. <book>:?<chapter>
    // 3. <book>:?<chapter>:<verse>
    // 3a. <book>:?<chapter>:<verse>[,<verse>]...
    // 4. <book>:?<chapter>-<chapter>
    // 5. <book>:?<chapter>:<verse>-<verse>
    // 6. <book>:?<chapter>:<verse>-<chapter>:<verse>
    // 7. /<search>
    // 8. <book>/search
    // 9. <book>:?<chapter>/search

    ref->type = 0;
    ref->book = 0;
    ref->chapter = 0;
    ref->chapter_end = 0;
    ref->verse = 0;
    ref->verse_end = 0;
    intset_free(ref->verse_set);
    ref->verse_set = NULL;
    free(ref->search_str);
    ref->search_str = NULL;
    regfree(&ref->search);

    int n = 0;
    if (kjv_scanbook(ref_str, &n) == 1) {
        // 1, 2, 3, 3a, 4, 5, 6, 8, 9
        char *bookname = strndup(ref_str, n);
        ref->book = kjv_book_fromname(bookname);
        free(bookname);
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '/') {
        // 7
        goto search;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ": %u %n", &ref->chapter, &n) == 1 || sscanf(ref_str, "%u %n", &ref->chapter, &n) == 1) {
        // 2, 3, 3a, 4, 5, 6, 9
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '/') {
        // 8
        goto search;
    } else if (ref_str[0] == '\0') {
        // 1
        ref->type = KJV_REF_EXACT;
        return 0;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ": %u %n", &ref->verse, &n) == 1) {
        // 3, 3a, 5, 6
        ref_str = &ref_str[n];
    } else if (sscanf(ref_str, "- %u %n", &ref->chapter_end, &n) == 1) {
        // 4
        if (ref_str[n] != '\0') {
            return 1;
        }
        ref->type = KJV_REF_RANGE;
        return 0;
    } else if (ref_str[0] == '/') {
        // 9
        goto search;
    } else if (ref_str[0] == '\0') {
        // 2
        ref->type = KJV_REF_EXACT;
        return 0;
    } else {
        return 1;
    }

    unsigned int value;
    int ret = sscanf(ref_str, "- %u %n", &value, &n);
    if (ret == 1 && ref_str[n] == '\0') {
        // 5
        ref->verse_end = value;
        ref->type = KJV_REF_RANGE;
        return 0;
    } else if (ret == 1) {
        // 6
        ref->chapter_end = value;
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '\0') {
        // 3
        ref->type = KJV_REF_EXACT;
        return 0;
    } else if (sscanf(ref_str, ", %u %n", &value, &n) == 1) {
        // 3a
        ref->verse_set = intset_new();
        intset_add(ref->verse_set, ref->verse);
        intset_add(ref->verse_set, value);
        ref_str = &ref_str[n];
        while (true) {
            if (sscanf(ref_str, ", %u %n", &value, &n) != 1) {
                break;
            }
            intset_add(ref->verse_set, value);
            ref_str = &ref_str[n];
        }
        if (ref_str[0] != '\0') {
            return 1;
        }
        ref->type = KJV_REF_EXACT_SET;
        return 0;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ": %u %n", &ref->verse_end, &n) == 1 && ref_str[n] == '\0') {
        // 6
        ref->type = KJV_REF_RANGE_EXT;
        return 0;
    } else {
        return 1;
    }

search:
    ref->type = KJV_REF_SEARCH;
    if (regcomp(&ref->search, &ref_str[1], REG_EXTENDED|REG_ICASE|REG_NOSUB) != 0) {
        return 2;
    }
    ref->search_str = strdup(&ref_str[1]);
    return 0;
}
