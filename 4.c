#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define MAX_GRID_SIZE (256 * 256)
#define MAX_IN_SIZE 256

#define FALSE 0
#define TRUE !FALSE

int get_adj(char grid[static MAX_GRID_SIZE], int width, int height, int pos) {
    int num_adj = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int x, y;
            x = pos % width + dx;
            y = pos / width + dy;
            if (x < 0 || x >= width || y < 0 || y >= height || (dx == dy && dx == 0)) {
                continue;
            }
            if (grid[x + y * width] == '@') {
                num_adj++;
            }
        }
    }
    return num_adj;
}

uint64_t run_through(char grid[static MAX_GRID_SIZE], int width, int height) {
    uint64_t score = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = x + y * width;
            if (grid[pos] == '@' && get_adj(grid, width, height, pos) < 4) {
                score++;
                printf("%d %d\n", x, y);
            }
        }
    }
    return score;
}

int main(int argc, char* argv[]) {
    char fname[256];
    char *in_buff;
    FILE *f;
    uint64_t score = 0; // , score2 = 0;
    char grid[MAX_GRID_SIZE] = {0};
    int width = -1, row = 0;

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
        width = n - 1;
        memcpy(grid + width * row, in_buff, width);
        row++;
    }

    score = run_through(grid, width, row);

    printf("Score -> %lu\n", score);

    fclose(f);
    free(in_buff);

    return 0;
}