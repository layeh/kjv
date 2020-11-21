/*
kjv: Read the Word of God from your terminal

License: Public domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/ioctl.h>

#include "data.h"
#include "intset.h"

typedef struct {
    bool linewrap;
    int maximum_line_length;
} kjv_config;

#define KJV_REF_SEARCH 1
#define KJV_REF_EXACT 2
#define KJV_REF_EXACT_SET 3
#define KJV_REF_RANGE 4
#define KJV_REF_RANGE_EXT 5

typedef struct kjv_ref {
    int type;
    char book[64];
    unsigned int chapter;
    unsigned int chapter_end;
    unsigned int verse;
    unsigned int verse_end;
    intset *verse_set;
    char *search_str;
    regex_t search;
} kjv_ref;

static kjv_ref *
kjv_newref()
{
    return calloc(1, sizeof(kjv_ref));
}

static void
kjv_freeref(kjv_ref *ref)
{
    if (ref) {
        free(ref->search_str);
        regfree(&ref->search);
        free(ref);
    }
}

static int
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
    ref->book[0] = '\0';
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

    sscanf(ref_str, "%*1[1-3]%n", &n);
    bool has_booknum = n > 0;
    if (has_booknum && sscanf(ref_str, "%1[1-3]%62[a-zA-Z ]%n", &ref->book[0], &ref->book[1], &n) == 2) {
        ref_str = &ref_str[n];
    } else if (!has_booknum && sscanf(ref_str, "%63[a-zA-Z ]%n", &ref->book[0], &n) == 1) {
        // 1, 2, 3, 3a, 4, 5, 6, 8, 9
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '/') {
        // 7
        goto search;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ":%u%n", &ref->chapter, &n) == 1 || sscanf(ref_str, "%u%n", &ref->chapter, &n) == 1) {
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

    if (sscanf(ref_str, ":%u%n", &ref->verse, &n) == 1) {
        // 3, 3a, 5, 6
        ref_str = &ref_str[n];
    } else if (sscanf(ref_str, "-%u%n", &ref->chapter_end, &n) == 1) {
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
    int ret = sscanf(ref_str, "-%u%n", &value, &n);
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
    } else if (sscanf(ref_str, ",%u%n", &value, &n) == 1) {
        // 3a
        ref->verse_set = intset_new();
        intset_add(ref->verse_set, ref->verse);
        intset_add(ref->verse_set, value);
        ref_str = &ref_str[n];
        while (true) {
            if (sscanf(ref_str, ",%u%n", &value, &n) != 1) {
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

    if (sscanf(ref_str, ":%u%n", &ref->verse_end, &n) == 1 && ref_str[n] == '\0') {
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

static char *
str_join(size_t n, char *strs[])
{
    size_t length = 0;
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            length++;
        }
        length += strlen(strs[i]);
    }
    char *str = malloc(length + 1);
    str[0] = '\0';
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            strcat(str, " ");
        }
        strcat(str, strs[i]);
    }
    return str;
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
kjv_book_matches(const char *book, const kjv_verse *verse)
{
    return kjv_bookequal(verse->book_name, book, false) ||
        kjv_bookequal(verse->book_abbr, book, false) ||
        kjv_bookequal(verse->book_name, book, true);
}

static bool
kjv_should_output(const kjv_ref *ref, const kjv_verse *verse)
{
    switch (ref->type) {
        case KJV_REF_SEARCH:
            return (ref->book[0] == '\0' || kjv_book_matches(ref->book, verse)) &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                regexec(&ref->search, verse->text, 0, NULL, 0) == 0;

        case KJV_REF_EXACT:
            return kjv_book_matches(ref->book, verse) &&
                (ref->chapter == 0 || ref->chapter == verse->chapter) &&
                (ref->verse == 0 || ref->verse == verse->verse);

        case KJV_REF_EXACT_SET:
            return kjv_book_matches(ref->book, verse) &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                intset_contains(ref->verse_set, verse->verse);

        case KJV_REF_RANGE:
            return kjv_book_matches(ref->book, verse) &&
                ((ref->chapter_end == 0 && ref->chapter == verse->chapter) ||
                    (verse->chapter >= ref->chapter && verse->chapter <= ref->chapter_end)) &&
                (ref->verse == 0 || verse->verse >= ref->verse) &&
                (ref->verse_end == 0 || verse->verse <= ref->verse_end);

        case KJV_REF_RANGE_EXT:
            return kjv_book_matches(ref->book, verse) &&
                (
                    (verse->chapter == ref->chapter && verse->verse >= ref->verse && ref->chapter != ref->chapter_end) ||
                    (verse->chapter > ref->chapter && verse->chapter < ref->chapter_end) ||
                    (verse->chapter == ref->chapter_end && verse->verse <= ref->verse_end && ref->chapter != ref->chapter_end) ||
                    (ref->chapter == ref->chapter_end && verse->chapter == ref->chapter && verse->verse >= ref->verse && verse->verse <= ref->verse_end)
                );

        default:
            return false;
    }
}

#define ESC_BOLD "\033[1m"
#define ESC_UNDERLINE "\033[4m"
#define ESC_RESET "\033[m"

static bool
kjv_output(const kjv_ref *ref, FILE *f, const kjv_config *config)
{
    bool printed = false;
    int last_book_printed = 0;
    for (size_t i = 0; i < kjv_verses_length; i++) {
        kjv_verse *verse = &kjv_verses[i];
        if (kjv_should_output(ref, verse)) {
            if (verse->book != last_book_printed) {
                fprintf(f, ESC_UNDERLINE "%s" ESC_RESET "\n", verse->book_name);
                last_book_printed = verse->book;
            }
            fprintf(f, ESC_BOLD "%d:%d" ESC_RESET "\t", verse->chapter, verse->verse);
            char verse_text[1024];
            strcpy(verse_text, verse->text);
            size_t characters_printed = 0;
            char *word = strtok(verse_text, " ");
            while (word != NULL) {
                size_t word_length = strlen(word);
                if (config->linewrap && characters_printed + word_length + (characters_printed > 0 ? 1 : 0) > config->maximum_line_length - 8 - 2) {
                    fprintf(f, "\n\t");
                    characters_printed = 0;
                }
                if (characters_printed > 0) {
                    fprintf(f, " ");
                    characters_printed++;
                }
                fprintf(f, "%s", word);
                characters_printed += word_length;
                word = strtok(NULL, " ");
            }
            fprintf(f, "\n");
            printed = true;
        }
    }
    return printed;
}

static int
kjv_render(const kjv_ref *ref, const kjv_config *config)
{
    int fds[2];
    if (pipe(fds) == -1) {
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        char *args[9];
        int arg = 0;
        args[arg++] = "less";
        args[arg++] = "-J";
        args[arg++] = "-I";
        if (ref->search_str != NULL) {
            args[arg++] = "-p";
            args[arg++] = ref->search_str;
        }
        args[arg++] = "-R";
        args[arg++] = "-f";
        args[arg++] = "-";
        args[arg++] = NULL;
        execvp("less", args);
        printf("unable not exec less\n");
        _exit(0);
    } else if (pid == -1) {
        printf("unable to fork\n");
        return 2;
    }
    close(fds[0]);
    FILE *output = fdopen(fds[1], "w");
    bool printed = kjv_output(ref, output, config);
    if (!printed) {
        kill(pid, SIGTERM);
    }
    fclose(output);
    waitpid(pid, NULL, 0);
    if (!printed) {
        printf("unknown reference\n");
    }
    return 0;
}

const char *
usage = "usage: kjv [flags] [reference...]\n"
    "\n"
    "  -l      list books\n"
    "  -W      no line wrap\n"
    "  -h      show help\n"
    "\n"
    "  Reference:\n"
    "      <Book>\n"
    "          Individual book\n"
    "      <Book>:<Chapter>\n"
    "          Individual chapter of a book\n"
    "      <Book>:<Chapter>:<Verse>[,<Verse>]...\n"
    "          Individual verse(s) of a specific chapter of a book\n"
    "      <Book>:<Chapter>-<Chapter>\n"
    "          Range of chapters in a book\n"
    "      <Book>:<Chapter>:<Verse>-<Verse>\n"
    "          Range of verses in a book chapter\n"
    "      <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>\n"
    "          Range of chapters and verses in a book\n"
    "\n"
    "      /<Search>\n"
    "          All verses that match a pattern\n"
    "      <Book>/<Search>\n"
    "          All verses in a book that match a pattern\n"
    "      <Book>:<Chapter>/<Search>\n"
    "          All verses in a chapter of a book that match a pattern\n";

int
main(int argc, char *argv[])
{
    kjv_config config = {
        .linewrap = true,
        .maximum_line_length = 80,
    };

    bool list_books = false;

    int opt;
    while ((opt = getopt(argc, argv, "lWh")) != -1) {
        switch (opt) {
        case 'l':
            list_books = true;
            break;
        case 'W':
            config.linewrap = false;
            break;
        case 'h':
            printf("%s", usage);
            return 0;
        case '?':
            fprintf(stderr, "%s", usage);
            return 1;
        }
    }

    if (list_books) {
        int i;
        char *last_book_printed = NULL;
        for (i = 0; i < kjv_verses_length; i++) {
            if (last_book_printed == NULL || strcmp(kjv_verses[i].book_name, last_book_printed) != 0) {
                printf("%s (%s)\n", kjv_verses[i].book_name, kjv_verses[i].book_abbr);
                last_book_printed = kjv_verses[i].book_name;
            }
        }
        return 0;
    }

    struct winsize ttysize;
    memset(&ttysize, 0, sizeof(struct winsize));
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttysize) == 0 && ttysize.ws_col > 0) {
        config.maximum_line_length = ttysize.ws_col;
    }

    signal(SIGPIPE, SIG_IGN);

    if (argc == optind) {
        while (true) {
            char *input = readline("kjv> ");
            if (input == NULL) {
                break;
            }
            kjv_ref *ref = kjv_newref();
            int success = kjv_parseref(ref, input);
            free(input);
            if (success == 0) {
                kjv_render(ref, &config);
            }
            kjv_freeref(ref);
        }
    } else {
        char *ref_str = str_join(argc-optind, &argv[optind]);
        kjv_ref *ref = kjv_newref();
        int success = kjv_parseref(ref, ref_str);
        free(ref_str);
        if (success == 0) {
            kjv_render(ref, &config);
        }
        kjv_freeref(ref);
    }

    return 0;
}
