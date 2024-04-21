#include <stdbool.h> // bool, true, false
#include <stdint.h>  // uint64_t
#include <stdio.h>   // getline, printf, sprintf, stdin, fopen, fclose, FILE, fgets
#include <stdlib.h>  // NULL, free, qsort, size_t, ssize_t
#include <string.h>  // memset, strlen, strnlen, strcspn

#include "strmap.h" // strmap_*

#define max(x, y) ((x) > (y) ? (x) : (y))

#define line_len_max 150
static uint64_t result[line_len_max][line_len_max];

uint64_t longest_common_subsequence(const char* str1, size_t n, const char* str2, size_t m) {
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_subsequence
    memset(result, 0, sizeof(result));
    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            if (str1[i - 1] == str2[j - 1]) {
                result[i][j] = result[i - 1][j - 1] + 1;
            } else {
                result[i][j] = max(result[i - 1][j], result[i][j - 1]);
            }
        }
    }
    return result[n][m];
}

#define abs_diff(x, y) ((x) >= (y) ? (x) - (y) : (y) - (x))

static char* cmp_custom_usr_inp_p = NULL;
int cmp_custom(const void* s1, const void* s2) {
    size_t n = strlen(cmp_custom_usr_inp_p);
    size_t m1 = strlen(*(char**)s1);
    size_t m2 = strlen(*(char**)s2);

    uint64_t lcs1 = longest_common_subsequence(cmp_custom_usr_inp_p, n, *(char**)s1, m1);
    uint64_t lcs2 = longest_common_subsequence(cmp_custom_usr_inp_p, n, *(char**)s2, m2);

    if (lcs1 != lcs2) {
        return -(lcs1 < lcs2) + (lcs1 > lcs2);
    }

    size_t diff1 = abs_diff(m1, n);
    size_t diff2 = abs_diff(m2, n);

    return -(diff2 < diff1) + (diff2 > diff1);
}

int main(void) {

    // prepare stuff:

    printf("Please wait...\n");
    int status = system("python get_data.py");
    if (status != 0) {
        return 1;
    }

    FILE* fp = fopen("data.csv", "r");
    if (fp == NULL) {
        return 1;
    }
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        return 1;
    }

    // read from file and store data as keys and values in strmap:
    char buf[line_len_max];
    fgets(buf, line_len_max, fp); // skip first line
    while (true) {
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

        char buf1[line_len_max * 2];
        sprintf(buf1, "%s%s%s", formula_str, *cas_str != '\0' ? ", CAS: " : "", cas_str);
        strmap_set(strmap_p,
                   synonym_str, // key
                   buf1         // value
        );
    }

    // create an array of key references:
    size_t count = strmap_get_count(strmap_p);
    char** keys_arr_pp = malloc(sizeof(char*) * count);
    {
        size_t keys_arr_index = 0;

        // required for iterating strmap:
        size_t list_index;
        strmap_node_type* next_p = NULL;
        char* key_p = NULL;
        char* value_p = NULL;

        strmap_for_each(strmap_p, list_index, next_p, key_p, value_p) {
            keys_arr_pp[keys_arr_index++] = key_p; // store reference
        }
    }
    printf("Type your input:\n");

    // actual program:
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;
    while (0 < (len = getline(&line_p, &n, stdin))) {
        if (line_p[len - 1] == '\n') {
            line_p[len - 1] = '\0';
        }
        if (strmap_contains(strmap_p, line_p)) {
            printf(" -> %s (%s)\n", line_p, strmap_get(strmap_p, line_p));
            continue;
        }
        cmp_custom_usr_inp_p = &line_p[0];
        qsort(keys_arr_pp, count, sizeof(char*), cmp_custom);
        for (size_t i = count - 1; i >= count - 5; i--) {
            printf(" -> %s (%s)\n", keys_arr_pp[i], strmap_get(strmap_p, keys_arr_pp[i]));
        }
    }
    // clean up stuff:
    free(line_p);
    free(keys_arr_pp);
    strmap_destroy(strmap_p);
    fclose(fp);

    return 0;
}
