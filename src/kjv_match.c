#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>

#include "kjv_data.h"
#include "kjv_match.h"
#include "intset.h"

static bool
kjv_verse_matches(const kjv_ref *ref, const kjv_verse *verse)
{
    switch (ref->type) {
        case KJV_REF_SEARCH:
            return (ref->book == 0 || ref->book == verse->book) &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                regexec(&ref->search, verse->text, 0, NULL, 0) == 0;

        case KJV_REF_EXACT:
            return ref->book == verse->book &&
                (ref->chapter == 0 || ref->chapter == verse->chapter) &&
                (ref->verse == 0 || ref->verse == verse->verse);

        case KJV_REF_EXACT_SET:
            return ref->book == verse->book &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                intset_contains(ref->verse_set, verse->verse);

        case KJV_REF_RANGE:
            return ref->book == verse->book &&
                ((ref->chapter_end == 0 && ref->chapter == verse->chapter) ||
                    (verse->chapter >= ref->chapter && verse->chapter <= ref->chapter_end)) &&
                (ref->verse == 0 || verse->verse >= ref->verse) &&
                (ref->verse_end == 0 || verse->verse <= ref->verse_end);

        case KJV_REF_RANGE_EXT:
            return ref->book == verse->book &&
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

#define KJV_DIRECTION_BEFORE -1
#define KJV_DIRECTION_AFTER 1

static int
kjv_chapter_bounds(int i, int direction, int maximum_steps)
{
    assert(direction == KJV_DIRECTION_BEFORE || direction == KJV_DIRECTION_AFTER);

    int steps = 0;
    for ( ; 0 <= i && i < kjv_verses_length; i += direction) {
        bool step_limit = (maximum_steps != -1 && steps >= maximum_steps) ||
            (direction == KJV_DIRECTION_BEFORE && i == 0) ||
            (direction == KJV_DIRECTION_AFTER && i + 1 == kjv_verses_length);
        if (step_limit) {
            break;
        }

        const kjv_verse *current = &kjv_verses[i], *next = &kjv_verses[i + direction];
        if (current->book != next->book || current->chapter != next->chapter) {
            break;
        }
        steps++;
    }
    return i;
}

static int
kjv_next_match(const kjv_ref *ref, int i)
{
    for ( ; i < kjv_verses_length; i++) {
        const kjv_verse *verse = &kjv_verses[i];
        if (kjv_verse_matches(ref, verse)) {
            return i;
        }
    }
    return -1;
}

static void
kjv_next_addrange(kjv_next_data *next, kjv_range range) {
    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        next->matches[0] = range;
    } else if (range.start < next->matches[0].end) {
        next->matches[0] = range;
    } else {
        next->matches[1] = range;
    }
}

int
kjv_next_verse(const kjv_ref *ref, const kjv_config *config, kjv_next_data *next)
{
    if (next->current >= kjv_verses_length) {
        return -1;
    }

    if (next->matches[0].start != -1 && next->matches[0].end != -1 && next->current >= next->matches[0].end) {
        next->matches[0] = next->matches[1];
        next->matches[1] = (kjv_range){-1, -1};
    }

    if ((next->next_match == -1 || next->next_match < next->current) && next->next_match < kjv_verses_length) {
        int next_match = kjv_next_match(ref, next->current);
        if (next_match >= 0) {
            next->next_match = next_match;
            kjv_range bounds = {
                .start = kjv_chapter_bounds(next_match, KJV_DIRECTION_BEFORE, config->context_chapter ? -1 : config->context_before),
                .end = kjv_chapter_bounds(next_match, KJV_DIRECTION_AFTER, config->context_chapter ? -1 : config->context_after) + 1,
            };
            kjv_next_addrange(next, bounds);
        } else {
            next_match = kjv_verses_length;
        }
    }

    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        return -1;
    }

    if (next->current < next->matches[0].start) {
        next->current = next->matches[0].start;
    }

    return next->current++;
}
