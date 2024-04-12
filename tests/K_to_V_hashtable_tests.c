#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr
#include <stdlib.h>  // NULL
#include <string.h>  // strcmp

#include "test.h" // is_true, is_false, is_equal

typedef char* charptr;
#define KEY_TYPE charptr
#define VALUE_TYPE int
#define KEY_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(v) (0)
#include "fixed-containers/K_to_V_hashtable.h" // charptr_to_int_hashtable_*

bool four_elements_test(void) {
    charptr_to_int_hashtable_type* hashtable_p = NULL;
    if (!charptr_to_int_hashtable_init(&hashtable_p, 8)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    size_t o = sizeof("AAA");
    char* buf = malloc(4 * o);
    memcpy(buf + 0 * o, "AAA", o);
    memcpy(buf + 1 * o, "BBB", o);
    memcpy(buf + 2 * o, "CCC", o);
    memcpy(buf + 3 * o, "DDD", o);


    charptr_to_int_hashtable_set(hashtable_p, &buf[0 * o], 1);
    res &= is_equal(charptr_to_int_hashtable_get(hashtable_p, "AAA", -1), 1);

    // charptr_to_int_hashtable_set(hashtable_p, &buf[1 * o], 2);
    // charptr_to_int_hashtable_set(hashtable_p, &buf[2 * o], 3);
    // charptr_to_int_hashtable_set(hashtable_p, &buf[3 * o], 4);

    // res &= is_equal(charptr_to_int_hashtable_get(hashtable_p, "AAB", -1), 2);
    // res &= is_equal(charptr_to_int_hashtable_get(hashtable_p, "BBB", -1), 3);
    // res &= is_equal(charptr_to_int_hashtable_get(hashtable_p, "CCC", -1), 4);

    res &= is_true(charptr_to_int_hashtable_deinit(&hashtable_p));
    // free(buf);

    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} bool_f_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    bool_f_plus bool_f_arr[] = {{four_elements_test, "four elements test"}};

    printf(__FILE_NAME__ ":\n");
    bool overall_res = true;
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        bool res = bool_f_arr[i].func();
        overall_res &= res;
        printf(" [%s] %s\n", res ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return overall_res ? 0 : 1;
}
