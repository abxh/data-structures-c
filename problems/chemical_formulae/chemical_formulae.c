#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strmap.h"

static const size_t MAX_LINE_LEN = 150;

int main(void) {

    int status = system("python get_data.py");
    if (status != 0) {
        return 1;
    }

    FILE* fp = fopen("data.csv", "r");
    if (fp == NULL) {
        return 1;
    }

    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return 1;
    }

    size_t lines = 0;
    while (true) {
        char buf[MAX_LINE_LEN];
        if (fgets(buf, MAX_LINE_LEN, fp) == NULL) {
            break;
        }
        size_t n = strnlen(buf, 150);
        buf[n - 1] = '\0'; // remove newline

        size_t i = strcspn(buf, ",");
        buf[i] = '\0';
        size_t j = i + 1 + strcspn(&buf[i+1], ",");
        buf[j] = '\0';

        char* formula_str = &buf[0];
        char* synonym_str = &buf[i + 1];
        char* cas_str = &buf[j + 1];

        char buf1[MAX_LINE_LEN];
        sprintf(buf1, "%s%s%s", formula_str, *cas_str != '\0' ? ", CAS: " : "", cas_str);
        strmap_set(strmap_p, synonym_str, buf1);
        printf("%s: %s\n", synonym_str, strmap_get(strmap_p, synonym_str));

        lines++;
    }



    strmap_deinit(&strmap_p);
    fclose(fp);
    return 0;
}
