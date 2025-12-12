#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char fname[256];
    char in_buff[256];

    if (argc < 2) {
        printf("Gib test file name\n");
        return -1;
    }

    snprintf(fname, sizeof(fname), "./in/%s", argv[1]);
    FILE *f = fopen(fname, "r");

    int dial = 50, score = 0, score2 = 0;

    while (fgets(in_buff, sizeof(in_buff), f) != NULL) {
        int n = strlen(in_buff);
        if (!n) {
            break;
        }
        int is_left = in_buff[0] == 'L';
        int rot = strtol(in_buff + 1, NULL, 10);
        int save_rot = rot;

        while (rot > 0) {
            dial += is_left ? -1 : 1;
            rot--;
            if (dial < 0) {
                dial = 99;
            }
            if (dial > 99) {
                dial = 0;
            }
            if (dial == 0) {
                if (rot == 0) {
                    score++;
                }
                score2++;
            }
        }
        printf("%c%d - Score: %d, Dial: %d\n", in_buff[0], save_rot, score, dial);
    }

    fclose(f);

    printf("Score: %d Score2: %d\n", score, score2);

    return 0;
}