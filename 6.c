#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IN_SIZE 4096
#define MAX_COLS 2048

#define FALSE 0
#define TRUE !FALSE

#define MAX_NUMS_PER_COLUMN 4
typedef struct {
    int nums[MAX_NUMS_PER_COLUMN];
    int size;
    char op;
} column_t;

uint64_t run_through(column_t *cols, int num_cols) {
    uint64_t score = 0;
    for (int i = 0; i < num_cols; i++) {
        column_t *col = cols + i;
        uint64_t subtotal = col->op == '+' ? 0 : 1;
        for (int j = 0; j < col->size; j++) {
            if (col->op == '+') {
                subtotal += col->nums[j];
            } else if (col->op == '*') {
                subtotal *= col->nums[j];
            } else {
                printf("Incorrect OP %c\n", col->op);
                exit(-1);
            }
        }
        printf("%4d: %lu %lu\n", i, subtotal, score);
        score += subtotal;
    }
    return score;
}

int part1(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    column_t *cols;
    FILE *f;
    uint64_t score = 0;

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    in_buff = (char *)malloc(sizeof(char) * MAX_IN_SIZE);
    cols = (column_t *)malloc(sizeof(column_t) * MAX_COLS);
    if (in_buff == NULL || cols == NULL) {
        printf("Failed to alloc memory\n");
        return -1;
    }

    snprintf(fname, sizeof(fname), "./in/%s", argv[1]);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("File not found\n");
        return -1;
    }

    int num_cols = -1, in_ops_line = FALSE;
    while (fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            printf("Shouldnt have reached here\n");
            exit(-1);
        }
        if (in_buff[0] == '+' || in_buff[0] == '*') {
            in_ops_line = TRUE;
        }

        char *str = strtok(in_buff, " ");
        int col_index = 0;
        do {
            if (str == NULL) {
                printf("Somethings gone wrong processesing:\n%s\n", in_buff);
                exit(-1);
            }
            column_t *col = cols + col_index;
            if (in_ops_line) {
                col->op = str[0];
            } else {
                col->nums[col->size++] = atoi(str);
            }
            col_index++;
        } while ((str = strtok(NULL, " ")) != NULL);

        if (num_cols == -1) {
            num_cols = col_index;
        }
    }

    for (int i = 0; i < num_cols; i++) {
        for (int j = 0; j < cols[i].size; j++) {
            printf("%d ", cols[i].nums[j]);
        }
        printf("%c\n", cols[i].op);
    }

    score = run_through(cols, num_cols);
    // score2 = run_through();
    printf("Score -> %lu\n", score);

    fclose(f);
    free(in_buff);

    return 0;
}

int part2(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    column_t *cols;
    char *txt_grid;
    FILE *f;
    uint64_t score = 0;

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    in_buff = (char *)malloc(sizeof(char) * MAX_IN_SIZE);
    cols = (column_t *)malloc(sizeof(column_t) * MAX_COLS);
    txt_grid = (char *)malloc(sizeof(char) * MAX_IN_SIZE * 5);
    if (in_buff == NULL || cols == NULL) {
        printf("Failed to alloc memory\n");
        return -1;
    }

    snprintf(fname, sizeof(fname), "./in/%s", argv[1]);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("File not found\n");
        return -1;
    }

    int width, height;
    width = -1;
    height = 0;
    while (fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            break;
        }

        in_buff[n--] = '\0';

        if (width == -1) {
            width = n;
        }
        memcpy(txt_grid + width * height, in_buff, width);
        height++;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%c", txt_grid[y * width + x]);
        }
        printf("\n");
    }

    char tmp_num[5];
    int col_index = 0;
    int x = width - 1;
    while (x >= 0) {
        memset(tmp_num, '\0', sizeof(tmp_num));
        for (int y = 0; y < height - 1; y++) {
            tmp_num[y] = txt_grid[y * width + x];
        }
        column_t *col = cols + col_index;
        char op = txt_grid[(height - 1) * width + x];

        printf("%5d: %4d %s\n", x, col_index, tmp_num);

        col->nums[col->size++] = atoi(tmp_num);

        if (op == '+' || op == '*') {
            col_index++;
            col->op = op;
            x--;
        }
        x--;
    }

    for (int i = 0; i < col_index; i++) {
        for (int j = 0; j < cols[i].size; j++) {
            printf("%d ", cols[i].nums[j]);
        }
        printf("%c\n", cols[i].op);
    }

    score = run_through(cols, col_index);

    printf("Score -> %lu\n", score);

    fclose(f);
    free(in_buff);

    return 0;
}

int main(int argc, char *argv[]) {
    // part1(argc, argv);
    part2(argc, argv);
}