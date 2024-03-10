#include "../src/strmap/strmap.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

bool one_element_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return false;
    }
    bool res = true;

    res &= strmap_set(strmap_p, "AA", "11");
    res &= strcmp(strmap_get(strmap_p, "AA"), "11") == 0;
    res &= strmap_deinit(&strmap_p);
    return res;
}

bool four_elements_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return false;
    }
    bool res = true;
    res &= strmap_set(strmap_p, "AAA", "1234");
    res &= strmap_set(strmap_p, "BBB", "5678");
    res &= strmap_set(strmap_p, "CCC", "9012");
    res &= strmap_set(strmap_p, "DDD", "3456");
    res &= strcmp(strmap_get(strmap_p, "AAA"), "1234") == 0;
    res &= strcmp(strmap_get(strmap_p, "BBB"), "5678") == 0;
    res &= strcmp(strmap_get(strmap_p, "CCC"), "9012") == 0;
    res &= strcmp(strmap_get(strmap_p, "DDD"), "3456") == 0;
    res &= strmap_deinit(&strmap_p);
    return res;
}

bool del_and_exists_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return false;
    }
    bool res = true;

    res &= strmap_set(strmap_p, "aaa", "1234");
    res &= strmap_set(strmap_p, "aab", "5678");
    res &= strmap_set(strmap_p, "aac", "9012");
    res &= strmap_set(strmap_p, "aad", "3456");

    res &= strmap_del(strmap_p, "aab");
    res &= strmap_del(strmap_p, "aac");

    res &= strmap_exists(strmap_p, "aaa") == true;
    res &= strmap_exists(strmap_p, "aab") == false;
    res &= strmap_exists(strmap_p, "aac") == false;
    res &= strmap_exists(strmap_p, "aad") == true;

    res &= strmap_deinit(&strmap_p);

    return res;
}

bool missing_elements_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return false;
    }
    bool res = true;

    res &= strmap_set(strmap_p, "a", "1234");
    res &= strmap_set(strmap_p, "d", "3456");

    res &= strmap_exists(strmap_p, "a") == true;
    res &= strmap_exists(strmap_p, "b") == false;
    res &= strmap_exists(strmap_p, "c") == false;
    res &= strmap_exists(strmap_p, "d") == true;

    res &= strmap_deinit(&strmap_p);

    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    func_plus bool_f_arr[] = {{one_element_test, "one element test"},
                              {four_elements_test, "four elements test"},
                              {del_and_exists_test, "delete and exists test"},
                              {missing_elements_test, "missing elements test"}};

    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
