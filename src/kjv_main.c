/*
kjv: Read the Word of God from your terminal

License: Public domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ioctl.h>

#include "kjv_config.h"
#include "kjv_data.h"
#include "kjv_ref.h"
#include "kjv_render.h"
#include "strutil.h"

const char *
usage = "usage: kjv [flags] [reference...]\n"
    "\n"
    "Flags:\n"
    "  -A num  show num verses of context after matching verses\n"
    "  -B num  show num verses of context before matching verses\n"
    "  -C      show matching verses in context of the chapter\n"
    "  -e      highlighting of chapters and verse numbers\n"
    "          (default when output is a TTY)\n"
    "  -p      output to less with chapter grouping, spacing, indentation,\n"
    "          and line wrapping\n"
    "          (default when output is a TTY)\n"
    "  -l      list books\n"
    "  -h      show help\n"
    "\n"
    "Reference:\n"
    "    <Book>\n"
    "        Individual book\n"
    "    <Book>:<Chapter>\n"
    "        Individual chapter of a book\n"
    "    <Book>:<Chapter>:<Verse>[,<Verse>]...\n"
    "        Individual verse(s) of a specific chapter of a book\n"
    "    <Book>:<Chapter>-<Chapter>\n"
    "        Range of chapters in a book\n"
    "    <Book>:<Chapter>:<Verse>-<Verse>\n"
    "        Range of verses in a book chapter\n"
    "    <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>\n"
    "        Range of chapters and verses in a book\n"
    "\n"
    "    /<Search>\n"
    "        All verses that match a pattern\n"
    "    <Book>/<Search>\n"
    "        All verses in a book that match a pattern\n"
    "    <Book>:<Chapter>/<Search>\n"
    "        All verses in a chapter of a book that match a pattern\n";

int
main(int argc, char *argv[])
{
    bool is_atty = isatty(STDOUT_FILENO) == 1;
    kjv_config config = {
        .highlighting = is_atty,
        .pretty = is_atty,

        .maximum_line_length = 80,

        .context_before = 0,
        .context_after = 0,
        .context_chapter = false,
    };

    bool list_books = false;

    opterr = 0;
    for (int opt; (opt = getopt(argc, argv, "A:B:CeplWh")) != -1; ) {
        char *endptr;
        switch (opt) {
        case 'A':
            config.context_after = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "kjv: invalid flag value for -A\n\n%s", usage);
                return 1;
            }
            break;
        case 'B':
            config.context_before = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "kjv: invalid flag value for -B\n\n%s", usage);
                return 1;
            }
            break;
        case 'C':
            config.context_chapter = true;
            break;
        case 'e':
            config.highlighting = true;
            break;
        case 'p':
            config.pretty = true;
            break;
        case 'l':
            list_books = true;
            break;
        case 'h':
            printf("%s", usage);
            return 0;
        case '?':
            fprintf(stderr, "kjv: invalid flag -%c\n\n%s", optopt, usage);
            return 1;
        }
    }

    if (list_books) {
        for (int i = 0; i < kjv_books_length; i++) {
            kjv_book *book = &kjv_books[i];
            printf("%s (%s)\n", book->name, book->abbr);
        }
        return 0;
    }

    struct winsize ttysize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttysize) == 0 && ttysize.ws_col > 0) {
        config.maximum_line_length = ttysize.ws_col;
    }

    signal(SIGPIPE, SIG_IGN);

    if (argc == optind) {
        using_history();
        while (true) {
            char *input = readline("kjv> ");
            if (input == NULL) {
                break;
            }
            add_history(input);
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
