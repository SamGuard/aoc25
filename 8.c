#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JUNCS 1024
#define MAX_IN_SIZE 256
#define LARGE_FLOAT 1000000000.0f

#define FALSE 0
#define TRUE !FALSE

typedef struct junc {
    int x, y, z;
    struct circ *circuit;
    struct junc *next;
} junc_t;

typedef struct circ {
    junc_t *root;
    struct circ *next, *prev;
} circ_t;

typedef struct pair {
    junc_t *a, *b;
    float dist;
} pair_t;

float_t euclid(junc_t *a, junc_t *b) {
    float x, y, z;
    x = a->x - b->x;
    y = a->y - b->y;
    z = a->z - b->z;

    return sqrtf(x * x + y * y + z * z);
}

int pair_cmp(const void *_a, const void *_b) {
    pair_t *a, *b;
    a = (pair_t *)_a;
    b = (pair_t *)_b;

    if (a->dist == b->dist) {
        return 0;
    } else {
        return a->dist > b->dist;
    }
}

int circ_len(circ_t *c) {
    int num = 0;
    for (junc_t *j = c->root; j != NULL; j = j->next) {
        num++;
    }
    return num;
}

void merge_circ(circ_t *a, circ_t *b, circ_t **root) {
    if (a->root == NULL || b->root == NULL) {
        printf("Somethings gone wrong\n");
        exit(-1);
    }
    if (a == b) {
        printf("Same circuit\n");
        return;
    }
    // Find tail of list a
    junc_t *j = a->root;
    while (j->next != NULL) {
        j = j->next;
    }
    j->next = b->root;

    // Update juncs in bs' circuits to point to a
    j = b->root;
    while (j != NULL) {
        j->circuit = a;
        j = j->next;
    }
    // Remove b
    if (b->prev) {
        b->prev->next = b->next;
    } else {
        printf("new root is %p\n", b->next);
        *root = b->next;
    }
    if (b->next)
        b->next->prev = b->prev;

    b->next = b->prev = NULL;
    b->root = NULL;

    printf("Merged circuits, new size %d\n", circ_len(a));
}

circ_t *add_circ(junc_t *j, circ_t *next, circ_t *prev) {
    circ_t *new = malloc(sizeof(circ_t));
    memset(new, 0, sizeof(circ_t));
    new->root = j;
    new->next = next;
    new->prev = prev;
    j->circuit = new;
    return new;
}

void iterate(pair_t *pair, circ_t **root) {
    junc_t *a, *b;
    a = pair->a;
    b = pair->b;

    printf("(%5d,%5d,%5d) -> (%5d,%5d,%5d)\n", a->x, a->y, a->z, b->x, b->y, b->z);

    circ_t *circ_a, *circ_b;
    circ_a = a->circuit;
    circ_b = b->circuit;
    merge_circ(circ_a, circ_b, root);
}

void run_through(junc_t juncs[static MAX_JUNCS], int num_juncs, int iters, uint64_t *_score, uint64_t *_score2) {
    uint64_t score = 1, score2 = 0;
    circ_t *root = NULL;
    int num_dists = 0;
    pair_t *dists = calloc(num_juncs * num_juncs, sizeof(junc_t));

    // 1 to 1 mapping of circ to junc
    for (int i = 0; i < num_juncs; i++) {
        root = add_circ(juncs + i, root, NULL);
        if (root->next) {
            root->next->prev = root;
        }
    }

    // Calculate all dists between all points, avoiding duplicated
    // a -> b === b -> a
    for (int i = 0; i < num_juncs; i++) {
        for (int j = i; j < num_juncs; j++) {
            junc_t *a, *b;
            pair_t *p = dists + num_dists;
            a = juncs + i;
            b = juncs + j;
            p->a = a;
            p->b = b;

            if (a == b) {
                p->dist = LARGE_FLOAT;
            } else {
                p->dist = euclid(a, b);
            }

            num_dists++;
        }
    }

    qsort(dists, num_dists, sizeof(pair_t), pair_cmp);
    for (int i = 0; i < num_dists; i++) {
        pair_t *d = dists + i;
        junc_t *a = d->a, *b = d->b;
        // printf("(%5d,%5d,%5d) -> (%5d,%5d,%5d) = %f\n", a->x, a->y, a->z, b->x, b->y, b->z, d->dist);
    }

    for (int i = 0; i < iters; i++) {
        iterate(dists + i, &root);
    }

    for (circ_t *c = root; c != NULL; c = c->next) {
        int len = circ_len(c);
        if (len < 1) {
            continue;
        }
        printf("Circ len %p -> %d\n", c, len);
    }

    // Shit sort
    int top[3] = {0};
    for (circ_t *c = root; c != NULL; c = c->next) {
        int len = circ_len(c);
        for (int i = 0; i < 3; i++) {
            if (top[i] < len) {
                int swp = top[i];
                top[i] = len;
                len = swp;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        score *= (uint64_t)top[i];
        printf("Top: %d\n", top[i]);
    }

    // Part 2
    pair_t *pair;
    for (int pair_index = 0; root->next != NULL; pair_index++) {
        if (pair_index >= num_dists) {
            printf("Did not connect\n");
            exit(-1);
        }
        pair = dists + pair_index;
        iterate(pair, &root);
    }

    printf("X dim of last connection: %d %d\n", pair->a->x, pair->b->x);
    score2 = pair->a->x * pair->b->x;

    free(dists);
    *_score = score;
    *_score2 = score2;
}

int main(int argc, char *argv[]) {
    char fname[256];
    char *in_buff;
    int is_example = FALSE;
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

    if (strstr(fname, "ex") != NULL) {
        is_example = TRUE;
    }

    junc_t juncs[MAX_JUNCS] = {0};
    int num_juncs = 0;
    while (fgets(in_buff, MAX_IN_SIZE, f) != NULL) {
        int n = strnlen(in_buff, MAX_IN_SIZE);
        if (!n) {
            break;
        }
        junc_t *j = juncs + (num_juncs++);
        j->next = NULL;
        sscanf(in_buff, "%d,%d,%d\n", &j->x, &j->y, &j->z);
    }

    run_through(juncs, num_juncs, is_example ? 10 : 1000, &score, &score2);

    printf("Score   -> %lu\n", score);
    printf("Score 2 -> %lu\n", score2);

    fclose(f);
    free(in_buff);

    return 0;
}
