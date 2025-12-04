#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define MAX_IN_SIZE 2048

#define FALSE 0
#define TRUE !FALSE

uint64_t powi(uint64_t a, uint64_t b) {
    uint64_t ret = 1;
    if (b == 0) {
        return 1;
    }
    for (uint64_t i = 0; i < b; i++) {
        ret *= a;
    }
    return ret;
}

uint64_t split_num(uint64_t num, uint64_t *left_out, uint64_t *right_out, int num_splits) {
    uint64_t left, right, digits;

    if (num == 0) {
        return FALSE;
    }

    digits = (uint64_t)floor(log10((double) (num)) + 1.0);

    left = right = 0;

    if (digits % 2 != 0) {
        return FALSE;
    }

    left = (num / powi(10, digits / 2));
    right = (num % powi(10, digits / 2));
    // printf("left %lu right %lu\n", left, right);

    *left_out = left;
    *right_out = right;
    return TRUE;
}

uint64_t count_invalid(uint64_t low, uint64_t high) {
    uint64_t count = 0, left, right;
    for (uint64_t i = low; i <= high; i++) {
        if (!split_num(i, &left, &right)) {
            continue;
        }
        if (left == right) {
            // printf("--------> %lu\n", i);
            count += i;
        }
    }
    printf("%lu %lu -> %lu\n", low, high, count);
    return count;
}

uint64_t run_through(char *input) {
    char *str;
    uint64_t low, high, tok_count, score;

    tok_count = score = 0;
    str = strtok(input, "-,");
    do {
        if (tok_count % 2 == 0) {
            low = strtoull(str, NULL, 10);
        } else {
            high = strtoull(str, NULL, 10);
            // printf("| %lu %lu\n", low, high);
            score += count_invalid(low, high);
        }

        tok_count++;
    } while((str = strtok(NULL, "-,")) != NULL);
    return score;
}

int main(int argc, char* argv[]) {
    char fname[256];
    char *in_buff;
    uint64_t score = 0;
    FILE *f;

    // count_invalid(18446744072573696541lu, 18446744072573844258lu);
    // count_invalid(86, 113);
    // return 0;

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    in_buff = (char *)malloc(sizeof (char) * MAX_IN_SIZE);
    if (in_buff == NULL) {
        printf("Failed to alloc memory\n");
        return -1;
    }

    snprintf(fname, sizeof (fname), "./in/%s", argv[1]);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("File not found\n");
        return -1;
    }
    fgets(in_buff, MAX_IN_SIZE, f);

    score = run_through(in_buff);

    fclose(f);
    free(in_buff);

    printf("Score: %lu\n", score);

    return 0;
}