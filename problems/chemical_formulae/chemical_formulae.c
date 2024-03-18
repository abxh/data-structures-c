#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "strmap.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char* s1, char* s2) {
    // Levenshtein distance - a measure of how similar two strings are.
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C

    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return column[s1len];
}

static char* CMP_CUSTOM_USR_INP_P = NULL;
int cmp_custom(const void* s1, const void* s2) {
    int dist_for_s1 = levenshtein(CMP_CUSTOM_USR_INP_P, *(char**)s1);
    int dist_for_s2 = levenshtein(CMP_CUSTOM_USR_INP_P, *(char**)s2);
    return -(dist_for_s1 < dist_for_s2) + (dist_for_s1 > dist_for_s2);
}

int main(void) {

    // prepare stuff:

    // printf("Please wait...\n");
    // int status = system("python get_data.py");
    // if (status != 0) {
    //     return 1;
    // }

    FILE* fp = fopen("data.csv", "r");
    if (fp == NULL) {
        return 1;
    }
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return 1;
    }

    // read from file and store data as keys and values in strmap:
    size_t lines = 0;
    size_t line_len_max = 150;
    while (true) {
        if (lines == 0) {
            lines++;
            continue;
        }
        char buf[line_len_max];
        if (fgets(buf, line_len_max, fp) == NULL) {
            break;
        }
        size_t n = strnlen(buf, 150);
        buf[n - 1] = '\0'; // remove newline

        size_t i = strcspn(buf, ",");
        buf[i] = '\0';

        size_t j;
        if (buf[i + 1] != '\"') {
            j = i + 1 + strcspn(&buf[i + 1], ",");
            buf[j] = '\0';
        } else {
            j = i + 2 + strcspn(&buf[i + 2], "\"") + 1;
            buf[j - 1] = '\0';
            buf[j] = '\0';
            i += 1;
        }

        char* formula_str = &buf[0];
        char* synonym_str = &buf[i + 1];
        char* cas_str = &buf[j + 1];

        char* iter_str = synonym_str;
        while (*iter_str++ != '\0') {
            *iter_str = towlower(*iter_str);
        }

        char buf1[line_len_max];
        sprintf(buf1, "%s%s%s", formula_str, *cas_str != '\0' ? ", CAS: " : "", cas_str);
        strmap_set(strmap_p,
                   synonym_str, // key
                   buf1         // value
        );

        lines++;
    }
    printf("actual: %lu why?:%lu",lines, strmap_p->list_count);

    // create an array of key references:
    char** keys_arr_pp = malloc(sizeof(char*) * lines);
    {
        size_t keys_arr_index = 0;

        // required for iterating strmap:
        size_t list_index;
        StrMapNode* next_p;
        char* key_p;
        char* value_p;

        strmap_for_each(strmap_p, list_index, next_p, key_p, value_p) {
            keys_arr_pp[keys_arr_index++] = key_p; // store reference
        }

        lines = keys_arr_index; // because there are some overlapping keys for some reason. TODO: fix
    }
    // printf("Type your input:\n");

    // actual program:
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;
    while (0 < (len = getline(&line_p, &n, stdin))) {
        CMP_CUSTOM_USR_INP_P = &line_p[0];
        qsort(keys_arr_pp, lines, sizeof(char*), cmp_custom);
        for (size_t i = 0; i < 5; i++) {
            printf(" %s\n  -> %s\n", keys_arr_pp[i], strmap_get(strmap_p, keys_arr_pp[i]));
        }
    }
    // clean up stuff:
    free(line_p);
    free(keys_arr_pp);
    strmap_deinit(&strmap_p);
    fclose(fp);

    return 0;
}
