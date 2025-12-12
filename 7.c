#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRID_SIZE (256 * 256)
#define MAX_IN_SIZE 256

#define FALSE 0
#define TRUE !FALSE

int get_adj(const char grid[static MAX_GRID_SIZE], int width, int height, int pos) {
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

uint64_t run_through(const char grid_in[static MAX_GRID_SIZE], int width, int height) {
    uint64_t score = 0;
    char grid[MAX_GRID_SIZE];
    memcpy(grid, grid_in, width * height);

    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = x + y * width, prev_pos = x + (y - 1) * width;
            char c_pos = grid[pos], c_prev = grid[prev_pos];
            if (c_prev == 'S' || c_prev == '|') {
                if (c_pos == '^') {
                    grid[pos - 1] = '|';
                    grid[pos + 1] = '|';
                    score++;
                } else {
                    grid[pos] = '|';
                }
            }
        }
        for (int x = 0; x < width; x++) {
            int pos = x + y * width;
            printf("%c", grid[pos]);
        }
        printf("\n");
    }
    return score;
}

uint64_t *g_cache;
uint64_t _run_through2(const char grid[static MAX_GRID_SIZE], const int width, const int height, int pos) {
    uint64_t score = 0;

    // for (int y = 1; y < height; y++) {
    //     for (int x = 0; x < width; x++) {
    //         int _pos = x + y * width;
    //         if (_pos == pos) {
    //             printf("X");
    //         } else {
    //             printf("%c", grid[_pos]);
    //         }
    //     }
    //     printf("\n");
    // }

    while (pos < width * height) {
        char c_pos = grid[pos];

        if (c_pos == '^') {
            if (g_cache[pos] != (uint64_t)-1) {
                return g_cache[pos];
            }
            score += _run_through2(grid, width, height, pos - 1);
            score += _run_through2(grid, width, height, pos + 1);
            g_cache[pos] = score;
            return score;
        } else {
            pos += width;
        }
    }
    return 1;
}

uint64_t run_through2(const char grid[static MAX_GRID_SIZE], const int width, const int height) {
    g_cache = malloc(sizeof(uint64_t) * width * height);

    for (int i = 0; i < width * height; i++) {
        g_cache[i] = (uint64_t)-1;
    }

    for (int i = 0; i < width; i++) {
        if (grid[i] == 'S') {
            return _run_through2(grid, width, height, (uint64_t)(i + width));
        }
    }
    printf("Did not find start :(\n");
    exit(-1);
}

int main(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    FILE *f;
    uint64_t score = 0, score2 = 0;
    char grid[MAX_GRID_SIZE] = {0};
    int width = -1, row = 0;

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

    while (fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            break;
        }
        width = n - 1;
        memcpy(grid + width * row, in_buff, width);
        row++;
    }

    score = run_through(grid, width, row);
    score2 = run_through2(grid, width, row);

    printf("Score   -> %lu\n", score);
    printf("Score 2 -> %lu\n", score2);

    fclose(f);
    free(in_buff);

    return 0;
}
