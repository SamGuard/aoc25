#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STOP_AFTER_FILL 0

#define MAX_NUM_TILES 1024
#define MAX_IN_SIZE 256
#define LARGE_FLOAT 1000000000.0f

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define CALC_POS(x, y, width) (x + y * (uint64_t)width)

#define FALSE 0
#define TRUE !FALSE

typedef struct {
    int x, y;
} tile_t;

typedef struct {
    tile_t *t1, *t2;
    uint64_t area;
} entry_t;

int entry_cmp(const void *_a, const void *_b) {
    entry_t *a, *b;
    a = (entry_t *)_a;
    b = (entry_t *)_b;

    if (a->area == b->area) {
        return 0;
    } else {
        return a->area < b->area;
    }
}

int is_adj_to(char *grid, int width, int height, int px, int py, char fill) {
    int startx, starty, endx, endy;

    startx = MAX(0, px - 1);
    starty = MAX(0, py - 1);
    endx = MIN(width - 1, px + 1);
    endy = MIN(height - 1, py + 1);

    for (int x = startx; x <= endx; x++) {
        for (int y = starty; y <= endy; y++) {
            if (grid[CALC_POS(x, y, width)] == fill) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

int is_all_inside(char *grid, tile_t *t1, tile_t *t2, char outside, int width) {
    int is_right, is_up, dx, dy;
    is_right = t2->x - t1->x > 0;
    is_up = t2->y - t1->y > 0;

    dx = is_right ? 1 : -1;
    dy = is_up ? 1 : -1;

    for (int y = t1->y; y - dy != t2->y; y += dy) {
        for (int x = t1->x; x - dx != t2->x; x += dx) {
            if (grid[CALC_POS(x, y, width)] == outside) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

int bucket_fill(char *grid, int width, int height, int startx, int starty, char fill, char tgt) {
    uint64_t pos = CALC_POS(startx, starty, width), subtotal = 0, total = width * (uint64_t)height, iter = 0;
    int touched_origin = FALSE;

    if (grid[pos] != tgt) {
        return touched_origin;
    }

    typedef struct {
        int x, y;
    } point_t;

    uint64_t stack_capacity = 1024;
    uint64_t stack_top = 0;
    point_t *stack = malloc(sizeof(point_t) * stack_capacity);
    if (!stack) {
        printf("Could not allocate stack for bucket fill\n");
        exit(-1);
    }

    stack[stack_top++] = (point_t){startx, starty};
    grid[pos] = fill;

    if (startx == 0 && starty == 0) {
        touched_origin = TRUE;
    }

    while (stack_top > 0) {
        point_t p = stack[--stack_top];

        const int dx[] = {0, 0, -1, 1};
        const int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }

            uint64_t npos = CALC_POS(nx, ny, width);
            if (grid[npos] == tgt) {
                grid[npos] = fill;
                subtotal++;

                // Grow stack if needed
                if (stack_top >= stack_capacity) {
                    stack_capacity *= 2;
                    point_t *new_stack = realloc(stack, sizeof(point_t) * stack_capacity);
                    if (!new_stack) {
                        printf("Could not grow stack for bucket fill\n");
                        free(stack);
                        exit(-1);
                    }
                    stack = new_stack;
                }

                stack[stack_top++] = (point_t){nx, ny};

                if (nx == 0 && ny == 0) {
                    touched_origin = TRUE;
                }
            }
        }
        if (iter % 100000 == 0) {
            printf("Done %lu out of %lu (%.1f%%)\n", subtotal, total, 100.0f * (float)(subtotal) / (float)total);
        }
        iter++;
    }

    free(stack);
    return touched_origin;
}

uint64_t run_through(tile_t tiles[static MAX_NUM_TILES], int num_tiles) {
    uint64_t best_area = 0; // , best_index_1 = 0, best_index_2 = 0;
    for (int i = 0; i < num_tiles; i++) {
        for (int j = i; j < num_tiles; j++) {
            tile_t *t1 = tiles + i, *t2 = tiles + j;
            uint64_t width, height, area;
            width = (abs(t1->x - t2->x) + 1);
            height = (abs(t1->y - t2->y) + 1);
            area = width * height;

            if (best_area < area) {
                best_area = area;
            }
        }
    }

    return best_area;
}

uint64_t run_through2(tile_t tiles[static MAX_NUM_TILES], int num_tiles) {
    int width, height;
    {
        int minx = 1 < 31 - 1, miny = 1 < 31 - 1, maxx = 0, maxy = 0;
        for (int i = 0; i < num_tiles; i++) {
            tile_t *t = tiles + i;
            if (t->x < minx)
                minx = t->x;
            if (t->y < miny)
                miny = t->y;
            if (t->x > maxx)
                maxx = t->x;
            if (t->y > maxy)
                maxy = t->y;
        }
        // Give buffer of 1
        minx--;
        miny--;
        printf("min: %d %d\n", minx, miny);
        for (int i = 0; i < num_tiles; i++) {
            tile_t *t = tiles + i;
            t->x -= minx;
            t->y -= miny;
        }
        width = maxx - minx + 2;
        height = maxy - miny + 2;
    }
    uint64_t grid_size = ((uint64_t)width) * ((uint64_t)height);
    printf("Width %d, Height %d, size %lu\n", width, height, grid_size);
    printf("Allocating large amount of memory...\n");
    char *grid = malloc(sizeof(char) * grid_size);
    printf("Done!\n");

    if (!grid) {
        printf("Could not alloc mem\n");
        exit(-1);
    }

    printf("Formatting buffer...\n");
    memset(grid, '.', sizeof(char) * grid_size);
    printf("Done!\n");

    for (int i = 0; i < num_tiles; i++) {
        tile_t *t1 = tiles + i, *t2 = tiles + ((i + 1) % num_tiles);
        int px, py, dx = t2->x - t1->x, dy = t2->y - t1->y;
        if (dx)
            dx /= abs(dx);
        if (dy)
            dy /= abs(dy);

        px = t1->x;
        py = t1->y;
        do {
            uint64_t pos = CALC_POS(px, py, width);
            grid[pos] = '#';
            px += dx;
            py += dy;
        } while (!(px == t2->x && py == t2->y));

        printf("Done %d out of %d (%.1f%%)\n", i + 1, num_tiles, 100.0f * (float)(i + 1) / (float)num_tiles);
    }

    char outside = '.';
    {
        int startx = tiles->x, starty = tiles->y;
        // if (grid[CALC_POS(startx - 1, starty, width)] == '.') {
        //     startx--;
        // } else if (grid[CALC_POS(startx + 1, starty, width)] == '.') {
        //     startx++;
        // } else if (grid[CALC_POS(startx, starty - 1, width)] == '.') {
        //     starty--;
        // } else if (grid[CALC_POS(startx, starty + 1, width)] == '.') {
        //     starty++;
        // } else {
        //     printf("\n:(\n");
        //     exit(-1);
        // }
        startx = tiles->x + 1;
        starty = tiles->y - 1;
        printf("Bucket filling...\n");
        if (bucket_fill(grid, width, height, startx, starty, '@', '.') == TRUE) {
            outside = '@';
        }
        printf("Done!\n");

        printf("Outside is %c\n", outside);
    }

    if (STOP_AFTER_FILL) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%c", grid[x + y * width]);
            }
            printf("\n");
        }
        exit(0);
    }

    printf("Creating list of all distances\n...");
    entry_t *entries = calloc(num_tiles * num_tiles, sizeof(entry_t));
    int num_entries = 0;
    for (int i = 0; i < num_tiles; i++) {
        for (int j = i; j < num_tiles; j++) {
            tile_t *t1 = tiles + i, *t2 = tiles + j;
            uint64_t width, height, area;
            width = (abs(t1->x - t2->x) + 1);
            height = (abs(t1->y - t2->y) + 1);
            area = width * height;

            entries[num_entries].t1 = t1;
            entries[num_entries].t2 = t2;
            entries[num_entries].area = area;
            num_entries++;
        }
    }
    printf("Done!\n");

    printf("Sorting...\n");
    qsort(entries, num_entries, sizeof(entry_t), entry_cmp);
    printf("Done!\n");

    int found = FALSE;
    entry_t *entry;
    printf("Finding best rectangle...");
    for (int i = 0; i < num_entries; i++) {
        entry = entries + i;
        if (is_all_inside(grid, entry->t1, entry->t2, outside, width) == TRUE) {
            printf("FOUND AREA %lu between (%d,%d) -> (%d,%d)\n", entry->area, entry->t1->x, entry->t1->y, entry->t2->x,
                   entry->t2->y);
            found = TRUE;
            break;
        }
        printf("Done %d out of %d (%.1f%%)\n", i + 1, num_entries, 100.0f * (float)(i + 1) / (float)num_entries);
    }
    printf("Done!\n");

    if (!found) {
        printf("I couldnt do it boss\n");
        exit(-1);
    }

    return entry->area;
}

int main(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    FILE *f;
    uint64_t score = 0, score2 = 0;

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

    tile_t tiles[MAX_NUM_TILES];
    int num_tiles = 0;
    while (fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            break;
        }
        tile_t *t = tiles + (num_tiles++);
        sscanf(in_buff, "%d,%d\n", &t->x, &t->y);
    }

    score = run_through(tiles, num_tiles);
    score2 = run_through2(tiles, num_tiles);

    printf("Score   -> %lu\n", score);
    printf("Score 2 -> %lu\n", score2);

    fclose(f);
    free(in_buff);

    return 0;
}
