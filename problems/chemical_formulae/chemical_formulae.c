#include <stdbool.h> // bool, true, false
#include <stdint.h>  // uint64_t
#include <stdio.h>   // getline, printf, sprintf, stdin, fopen, fclose, FILE, fgets
#include <stdlib.h>  // NULL, free, qsort, size_t, ssize_t
#include <string.h>  // memset, strlen, strnlen, strcspn, strcmp

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

#define PREFIX strmap
#define KEY_TYPE char*
#define VALUE_TYPE char*
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#include "hashtable.h" // strmap_*, hashtable_for_each

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
    static char line_buf[line_len_max];
    static char line_buf2[line_len_max * 2];
    fgets(line_buf, line_len_max, fp); // skip first line
    while (true) {
        if (fgets(line_buf, line_len_max, fp) == NULL) {
            break;
        }
        size_t n = strnlen(line_buf, line_len_max);
        line_buf[n - 1] = '\0'; // remove newline

        size_t i = strcspn(line_buf, ",");
        line_buf[i] = '\0';

        size_t j;
        if (line_buf[i + 1] != '\"') {
            j = i + 1 + strcspn(&line_buf[i + 1], ",");
            line_buf[j] = '\0';
        } else {
            j = i + 2 + strcspn(&line_buf[i + 2], "\"") + 1;
            line_buf[j - 1] = '\0';
            line_buf[j] = '\0';
            i += 1;
        }
        char* formula_str = &line_buf[0];
        char* synonym_str = &line_buf[i + 1];
        char* cas_str = &line_buf[j + 1];

        sprintf(line_buf2, "%s%s%s", formula_str, *cas_str != '\0' ? ", CAS: " : "", cas_str);
        strmap_update(strmap_p,
                      strdup(synonym_str), // key
                      strdup(line_buf2)    // value
        );
    }
    fclose(fp);

    // create an array of key references:
    size_t count = strmap_count(strmap_p);
    char** keys_arr_pp = malloc(sizeof(char*) * count);
    {
        size_t keys_arr_index = 0;

        // required for iterating strmap:
        size_t index;
        char* key_p;
        char* value_p;

        hashtable_for_each(strmap_p, index, key_p, value_p) {
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
        if (strmap_contains_key(strmap_p, line_p)) {
            printf(" -> %s (%s)\n", line_p, *strmap_get_mut(strmap_p, line_p));
            continue;
        }
        cmp_custom_usr_inp_p = &line_p[0];
        qsort(keys_arr_pp, count, sizeof(char*), cmp_custom);
        for (size_t i = count - 1; i >= count - 5; i--) {
            printf(" -> %s (%s)\n", keys_arr_pp[i], *strmap_get_mut(strmap_p, keys_arr_pp[i]));
        }
    }
    // clean up stuff:
    free(line_p);
    free(keys_arr_pp);
    strmap_destroy(strmap_p);

    return 0;
}
