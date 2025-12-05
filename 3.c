#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define MAX_IN_SIZE 2048
#define MAX_JOLTS 128

#define FALSE 0
#define TRUE !FALSE

uint64_t run_through(char input[static MAX_IN_SIZE], int len, int num_jolts) {
    int prev_index = -1;
    char num[MAX_JOLTS + 1] = {-1};


    for (int jolt = 0; jolt < num_jolts; jolt++) {
        for (int i = prev_index + 1; i < len + jolt + 1 - num_jolts; i++) {
            if (input[i] > num[jolt]) {
                num[jolt] = input[i];
                prev_index = i;
            }
        }
    }
    num[num_jolts] = '\0';

    return strtoull(num, NULL, 10);
}

int main(int argc, char* argv[]) {
    char fname[256];
    char *in_buff;
    FILE *f;
    uint64_t score = 0, score2 = 0;

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

    while(fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            break;
        }
        in_buff[n--] = '\0';
        score += run_through(in_buff, n, 2);
        score2 += run_through(in_buff, n, 12);
        printf("Score -> %lu Score2 -> %lu\n", score, score2);
    }


    fclose(f);
    free(in_buff);

    return 0;
}