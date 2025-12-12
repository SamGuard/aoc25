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

    int dial = 50, score = 0;

    while (fgets(in_buff, sizeof(in_buff), f) != NULL) {
        int n = strlen(in_buff);
        if (!n) {
            break;
        }
        int is_left = in_buff[0] == 'L';
        int rot = strtol(in_buff + 1, NULL, 10);

        dial += is_left ? -rot : rot;

        if (dial < 0) {
            dial = 100 + (dial % 100);
        }

        dial %= 100;

        if (dial == 0) {
            score++;
        }
        printf("%c%d - Score: %d, Dial: %d\n", in_buff[0], rot, score, dial);
    }

    fclose(f);

    printf("Score: %d\n", score);

    return 0;
}