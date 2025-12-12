#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IN_SIZE 2048
#define MAX_SPLITS 64

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

uint64_t calc_num_digits(uint64_t num) { return (uint64_t)floor(log10((double)(num)) + 1.0); }

uint64_t split_num(uint64_t num, uint64_t splits_out[static MAX_SPLITS], int num_splits) {
    uint64_t digits, right, pow;

    if (num == 0) {
        return FALSE;
    }

    digits = calc_num_digits(num);

    if (digits % num_splits != 0) {
        return FALSE;
    }

    // left = (num / powi(10, digits / 2));
    // right = (num % powi(10, digits / 2));
    // printf("%lu -> ", num);
    for (int i = 0; i < num_splits; i++) {
        pow = powi(10, digits / num_splits);
        right = (num % pow);
        splits_out[num_splits - i - 1] = right;
        num /= pow;
        // printf("%lu ", splits_out[num_splits - i - 1]);
    }
    // printf("\n");

    return TRUE;
}

int is_repeat(uint64_t splits[static MAX_SPLITS], int num_splits) {
    for (int i = 0; i < num_splits - 1; i++) {
        if (splits[i] != splits[i + 1]) {
            return FALSE;
        }
    }
    return TRUE;
}

uint64_t count_invalid(uint64_t low, uint64_t high) {
    uint64_t count = 0, splits[MAX_SPLITS] = {0};
    for (uint64_t i = low; i <= high; i++) {
        if (!split_num(i, splits, 2)) {
            continue;
        }
        if (is_repeat(splits, 2)) {
            // printf("--------> %lu\n", i);
            count += i;
        }
    }
    printf("%lu %lu -> %lu\n", low, high, count);
    return count;
}

uint64_t count_invalid_2(uint64_t low, uint64_t high) {
    uint64_t count = 0, splits[MAX_SPLITS] = {0}, num_digits;
    for (uint64_t i = low; i <= high; i++) {
        num_digits = calc_num_digits(i);
        for (uint64_t num_splits = 2; num_splits <= num_digits; num_splits++) {
            if (!split_num(i, splits, num_splits)) {
                continue;
            }
            if (is_repeat(splits, num_splits)) {
                // printf("--------> %lu\n", i);
                count += i;
                break;
            }
        }
    }
    printf("%lu %lu -> %lu\n", low, high, count);
    return count;
}

void run_through(char *input) {
    char *str;
    uint64_t low, high, tok_count, score, score2;

    tok_count = score = score2 = 0;
    str = strtok(input, "-,");
    do {
        if (tok_count % 2 == 0) {
            low = strtoull(str, NULL, 10);
        } else {
            high = strtoull(str, NULL, 10);
            // printf("| %lu %lu\n", low, high);
            score += count_invalid(low, high);
            score2 += count_invalid_2(low, high);
        }

        tok_count++;
    } while ((str = strtok(NULL, "-,")) != NULL);

    printf("Score: %lu, Score2: %lu\n", score, score2);
}

int main(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    FILE *f;

    // count_invalid(18446744072573696541lu, 18446744072573844258lu);
    // count_invalid(86, 113);
    // return 0;

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    in_buff = (char *)malloc(sizeof(char) * MAX_IN_SIZE);
    if (in_buff == NULL) {
        printf("Failed to alloc memory\n");
        return -1;
    }

    snprintf(fname, sizeof(fname), "./in/%s", argv[1]);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("File not found\n");
        return -1;
    }
    fgets(in_buff, MAX_IN_SIZE, f);

    run_through(in_buff);

    fclose(f);
    free(in_buff);

    return 0;
}