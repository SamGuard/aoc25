#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RANGES 2048
#define MAX_IDS 2048
#define MAX_JOLTS 128

#define FALSE 0
#define TRUE !FALSE

typedef struct range {
    uint64_t low, high;
    struct range *next;
} range_t;

uint64_t run_through(range_t *ranges, uint64_t *ids, int range_count, int id_count) {
    uint64_t score = 0;
    uint64_t num;
    range_t *range;
    for (int i = 0; i < id_count; i++) {
        num = ids[i];
        for (int j = 0; j < range_count; j++) {
            range = ranges + j;
            if (num >= range->low && num <= range->high) {
                score++;
                break;
            }
        }
    }
    return score;
}

uint64_t run_through2(range_t *ranges, int range_count) {
    uint64_t score;
    range_t *root = NULL, *c_range = NULL;

    score = 0;

    // shit sort
    for (int i = 0; i < range_count; i++) {
        range_t *range = ranges + i, *prev;

        prev = NULL;
        c_range = root;
        while (c_range != NULL && range->low > c_range->low) {
            prev = c_range;
            c_range = c_range->next;
        }
        if (c_range == NULL) {
            // End of list
            if (prev == NULL) {
                root = range;
            } else {
                prev->next = range;
            }
            continue;
        }
        if (c_range->low == range->low) {
            if (range->high <= c_range->high) {
                // Identical or subset of range, ignore this index
            } else {
                // Remove current index and replace
                if (prev == NULL) {
                    root = range;
                } else {
                    prev->next = range;
                }
                range->next = c_range->next;
            }
            continue;
        }

        // Insert into list
        if (prev == NULL) {
            root = range;
        } else {
            prev->next = range;
        }
        range->next = c_range;
    }

    uint64_t high = 0;
    for (range_t *range = root; range != NULL; range = range->next) {
        printf("Range %lu -> %lu => %lu (%lu)\n", range->low, range->high, score, high);

        int is_sep = high < range->low;
        if (is_sep) {
            score += range->high - range->low + 1;
            high = range->high;
            continue;
        }

        if (range->high <= high) {
            // Is a subset of this range
            continue;
        }

        score += range->high
                 // + 1 to move to the next uncounted index
                 - (high + 1)
                 // + 1 for inclusive range
                 + 1;

        high = range->high;
    }

    return score;
}

int main(int argc, char *argv[]) {
    char fname[256];
    char in_buff[256];
    uint64_t *ids;
    range_t *ranges;
    FILE *f;
    uint64_t score = 0, score2 = 0;

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    ranges = (range_t *)malloc(sizeof(range_t) * MAX_RANGES);
    ids = (uint64_t *)malloc(sizeof(uint64_t) * MAX_IDS);
    if (ranges == NULL || ids == NULL) {
        printf("Failed to alloc memory\n");
        return -1;
    }

    snprintf(fname, sizeof(fname), "./in/%s", argv[1]);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("File not found\n");
        return -1;
    }

    int range_count = 0;
    while (fgets(in_buff, sizeof(in_buff), f) != NULL) {
        int n = strnlen(in_buff, sizeof(in_buff));
        if (!n) {
            printf("Womp Womp. Should hit new line before EOF\n");
            exit(-1);
        }
        if (in_buff[0] == '\n') {
            break;
        }
        // Remove \n
        in_buff[n--] = '\0';

        range_t *range = ranges + range_count;

        if (sscanf(in_buff, "%lu-%lu", &(range->low), &(range->high)) != 2) {
            printf("Womp Womp:\n%s\n", in_buff);
            exit(-1);
        }
        range->next = NULL;
        range_count++;
    }

    int id_count = 0;
    while (fgets(in_buff, sizeof(in_buff), f) != NULL) {
        int n = strnlen(in_buff, sizeof(in_buff));
        if (!n) {
            break;
        }
        // Remove \n
        in_buff[n--] = '\0';

        ids[id_count] = strtoul(in_buff, NULL, 10);
        id_count++;
    }

    score = run_through(ranges, ids, range_count, id_count);
    score2 = run_through2(ranges, range_count);

    fclose(f);
    free(ranges);
    free(ids);

    printf("Score   -> %lu\n", score);
    printf("Score 2 -> %lu\n", score2);

    return 0;
}