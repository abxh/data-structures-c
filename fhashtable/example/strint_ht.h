#pragma once

#include <string.h>
#include "fnvhash.h"

#define NAME               strint_ht
#define KEY_TYPE           char *
#define VALUE_TYPE         int
#define TYPE_DEFINITIONS
#include "fhashtable_template.h"
