#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define JSMN_STRICT
#include "jsmn.h"
#include "jsmn-find.h"
#include "greatest.h"

TEST
check_unescaping(void *p_pair)
{
    char *(*pair)[2] = p_pair;
    char *str = NULL;

    ASSERT(jsmnf_unescape(&str, (*pair)[1], strlen((*pair)[1])) != 0);
    ASSERT_NEQ(NULL, str);
    ASSERT_STR_EQ((*pair)[0], str);

    PASS();
}

SUITE(jsmn_unescape)
{
    char *pairs[][2] = {
        { "áéíóú", "\\u00e1\\u00e9\\u00ed\\u00f3\\u00fa" },
        { "\"quote\"", "\"quote\"" },
        { "😊", "\\ud83d\\ude0a" },
        { "müller", "m\\u00fcller" },
    };
    size_t i;

    for (i = 0; i < sizeof(pairs) / sizeof *pairs; ++i) {
        RUN_TEST1(check_unescaping, pairs + i);
    }
}

TEST
check_find(void)
{
    const char JSON[] = "{\"foo\":{\"bar\":{\"baz\":[true]}}}";

    jsmnf *root, *f;

    root = jsmnf_init();

    ASSERT(jsmnf_start(root, JSON, sizeof(JSON) - 1) >= 0);
    ASSERT((f = jsmnf_find(root, "foo", sizeof("foo") - 1)) != NULL);
    ASSERT_STRN_EQ("foo", JSON + f->key->start, f->key->end - f->key->start);
    ASSERT((f = jsmnf_find(f, "bar", sizeof("bar") - 1)) != NULL);
    ASSERT_STRN_EQ("bar", JSON + f->key->start, f->key->end - f->key->start);
    ASSERT((f = jsmnf_find(f, "baz", sizeof("baz") - 1)) != NULL);
    ASSERT_STRN_EQ("baz", JSON + f->key->start, f->key->end - f->key->start);
    ASSERT((f = jsmnf_find(f, "0", sizeof("0") - 1)) != NULL);
    ASSERT_EQ(NULL, f->key);
    ASSERT_STRN_EQ("true", JSON + f->val->start, f->val->end - f->val->start);
    jsmnf_cleanup(root);

    PASS();
}

TEST
check_find_path(void)
{
    const char JSON[] = "{\"foo\":{\"bar\":{\"baz\":[true]}}}";

    char *path1[] = { "foo" };
    char *path2[] = { "foo", "bar" };
    char *path3[] = { "foo", "bar", "baz" };
    char *path4[] = { "foo", "bar", "baz", "0" };
    jsmnf *root, *f;

    root = jsmnf_init();

    ASSERT(jsmnf_start(root, JSON, sizeof(JSON) - 1) >= 0);
    ASSERT_NEQ(
        NULL, f = jsmnf_find_path(root, path1, sizeof(path1) / sizeof *path1));
    ASSERT_STRN_EQ(path1[0], JSON + f->key->start,
                   f->key->end - f->key->start);
    ASSERT_NEQ(
        NULL, f = jsmnf_find_path(root, path2, sizeof(path2) / sizeof *path2));
    ASSERT_STRN_EQ(path2[1], JSON + f->key->start,
                   f->key->end - f->key->start);
    ASSERT_NEQ(
        NULL, f = jsmnf_find_path(root, path3, sizeof(path3) / sizeof *path3));
    ASSERT_STRN_EQ(path3[2], JSON + f->key->start,
                   f->key->end - f->key->start);
    ASSERT_NEQ(
        NULL, f = jsmnf_find_path(root, path4, sizeof(path4) / sizeof *path4));
    ASSERT_EQ(NULL, f->key);
    ASSERT_STRN_EQ("true", JSON + f->val->start, f->val->end - f->val->start);
    jsmnf_cleanup(root);

    PASS();
}

SUITE(jsmn_find)
{
    RUN_TEST(check_find);
    RUN_TEST(check_find_path);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(jsmn_unescape);
    RUN_SUITE(jsmn_find);

    GREATEST_MAIN_END();
}
