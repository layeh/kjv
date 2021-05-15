#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "kjv_data.h"
#include "kjv_match.h"
#include "kjv_render.h"

#define ESC_BOLD "\033[1m"
#define ESC_UNDERLINE "\033[4m"
#define ESC_RESET "\033[m"

static void
kjv_output_verse(const kjv_verse *verse, FILE *f, const kjv_config *config)
{
    fprintf(
        f,
        config->highlighting ?
            ESC_BOLD "%d:%d" ESC_RESET "\t" :
            "%d:%d\t",
        verse->chapter, verse->verse
    );

    char verse_text[1024];
    strcpy(verse_text, verse->text);
    size_t characters_printed = 0;
    char *word = strtok(verse_text, " ");
    while (word != NULL) {
        size_t word_length = strlen(word);
        if (characters_printed + word_length + (characters_printed > 0 ? 1 : 0) > config->maximum_line_length - 8 - 2) {
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
}

static bool
kjv_output(const kjv_ref *ref, FILE *f, const kjv_config *config)
{
    kjv_next_data next = {
        .current = 0,
        .next_match = -1,
        .matches = {
            {-1, -1},
            {-1, -1},
        },
    };

    kjv_verse *last_printed = NULL;
    for (int verse_id; (verse_id = kjv_next_verse(ref, config, &next)) != -1; ) {
        kjv_verse *verse = &kjv_verses[verse_id];
        kjv_book *book = &kjv_books[verse->book - 1];

        if (config->pretty) {
            if (last_printed == NULL || verse->book != last_printed->book) {
                if (last_printed != NULL) {
                    fprintf(f, "\n");
                }
                fprintf(
                    f,
                    config->highlighting ?
                        ESC_UNDERLINE "%s" ESC_RESET "\n\n" :
                        "%s\n\n",
                    book->name
                );
            }
            kjv_output_verse(verse, f, config);
        } else {
            fprintf(
                f,
                config->highlighting ?
                    ESC_UNDERLINE "%s" ESC_RESET " " ESC_BOLD "%d:%d" ESC_RESET "  %s\n" :
                    "%s %d:%d  %s\n",
                book->name,
                verse->chapter,
                verse->verse,
                verse->text
            );
        }
        last_printed = verse;
    }
    return last_printed != NULL;
}

static int
kjv_render_pretty(const kjv_ref *ref, const kjv_config *config)
{
    int fds[2];
    if (pipe(fds) == -1) {
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        char *args[9] = {NULL};
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
    return 0;
}

int
kjv_render(const kjv_ref *ref, const kjv_config *config)
{
    if (config->pretty) {
        return kjv_render_pretty(ref, config);
    }
    kjv_output(ref, stdout, config);
    return 0;
}
