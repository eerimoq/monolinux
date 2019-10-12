/*
Narwhal v0.4.4 (https://github.com/vberlier/narwhal)
Amalgamated header file

Generated with amalgamate.py (https://github.com/edlund/amalgamate)

MIT License

Copyright (c) 2019 Valentin Berlier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef NARWHAL_H
#define NARWHAL_H

// #include "narwhal/assertion/assertion.h"
#ifndef NARWHAL_ASSERTION_H
#define NARWHAL_ASSERTION_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "narwhal/types.h"
#ifndef NARWHAL_TYPES_H
#define NARWHAL_TYPES_H

// #include "narwhal/collection/types.h"
#ifndef NARWHAL_COLLECTION_TYPES_H
#define NARWHAL_COLLECTION_TYPES_H

typedef struct NarwhalCollection NarwhalCollection;
typedef struct NarwhalCollectionItem NarwhalCollectionItem;

#endif

// #include "narwhal/diff/types.h"
#ifndef NARWHAL_DIFF_TYPES_H
#define NARWHAL_DIFF_TYPES_H

typedef struct NarwhalDiffMatrix NarwhalDiffMatrix;
typedef enum NarwhalDiffChunkType NarwhalDiffChunkType;
typedef struct NarwhalDiff NarwhalDiff;
typedef struct NarwhalDiffChunk NarwhalDiffChunk;

#endif

// #include "narwhal/discovery/types.h"
#ifndef NARWHAL_DISCOVERY_TYPES_H
#define NARWHAL_DISCOVERY_TYPES_H

typedef struct NarwhalTestDiscoveryQueue NarwhalTestDiscoveryQueue;

#endif

// #include "narwhal/fixture/types.h"
#ifndef NARWHAL_FIXTURE_TYPES_H
#define NARWHAL_FIXTURE_TYPES_H

typedef struct NarwhalTestFixture NarwhalTestFixture;

typedef void (*NarwhalTestFixtureSetup)(void *value, NarwhalTestFixture *test_fixture);
typedef void (*NarwhalTestFixtureCleanup)(void *value, NarwhalTestFixture *test_fixture);

#endif

// #include "narwhal/group/types.h"
#ifndef NARWHAL_GROUP_TYPES_H
#define NARWHAL_GROUP_TYPES_H

typedef struct NarwhalTestGroup NarwhalTestGroup;

typedef void (*NarwhalGroupItemRegistration)(NarwhalTestGroup *test_group);

#endif

// #include "narwhal/param/types.h"
#ifndef NARWHAL_PARAM_TYPES_H
#define NARWHAL_PARAM_TYPES_H

typedef struct NarwhalTestParam NarwhalTestParam;

#endif

// #include "narwhal/result/types.h"
#ifndef NARWHAL_RESULT_TYPES_H
#define NARWHAL_RESULT_TYPES_H

typedef struct NarwhalTestResult NarwhalTestResult;
typedef struct NarwhalTestParamSnapshot NarwhalTestParamSnapshot;

#endif

// #include "narwhal/session/types.h"
#ifndef NARWHAL_SESSION_TYPES_H
#define NARWHAL_SESSION_TYPES_H

typedef struct NarwhalTestSession NarwhalTestSession;
typedef struct NarwhalSessionOutputState NarwhalSessionOutputState;

#endif

// #include "narwhal/test/types.h"
#ifndef NARWHAL_TEST_TYPES_H
#define NARWHAL_TEST_TYPES_H

typedef struct NarwhalTest NarwhalTest;

typedef void (*NarwhalTestModifierRegistration)(NarwhalTest *test,
                                                NarwhalCollection *params,
                                                NarwhalCollection *fixtures);
typedef void (*NarwhalTestFunction)(void);

#endif

// #include "narwhal/test_utils/types.h"
#ifndef NARWHAL_TEST_UTILS_TYPES_H
#define NARWHAL_TEST_UTILS_TYPES_H

typedef struct NarwhalOutputCapture NarwhalOutputCapture;

#endif


#endif


void narwhal_fail_test(NarwhalTest *test, const char *format, ...);
bool narwhal_check_assertion(const NarwhalTest *test,
                             bool assertion_success,
                             const char *assertion,
                             const char *assertion_file,
                             size_t assertion_line);

bool narwhal_check_string_equal(const char *actual, const char *expected);
bool narwhal_check_substring(const char *string, const char *substring);
bool narwhal_check_memory_equal(const void *actual,
                                const void *expected,
                                size_t size,
                                size_t element_size);

const char *narwhal_assertion_process_string(const char *string);

#define _NARWHAL_TEST_FAILURE(...)                             \
    do                                                         \
    {                                                          \
        narwhal_fail_test(_narwhal_current_test, __VA_ARGS__); \
        exit(1);                                               \
    } while (0)

#define FAIL(...)                                                                            \
    do                                                                                       \
    {                                                                                        \
        if (narwhal_check_assertion(_narwhal_current_test, false, NULL, __FILE__, __LINE__)) \
        {                                                                                    \
            _NARWHAL_TEST_FAILURE("" __VA_ARGS__);                                           \
        }                                                                                    \
    } while (0)

#define ASSERT(assertion, ...)                                                       \
    do                                                                               \
    {                                                                                \
        if (narwhal_check_assertion(                                                 \
                _narwhal_current_test, (assertion), #assertion, __FILE__, __LINE__)) \
        {                                                                            \
            _NARWHAL_TEST_FAILURE("" __VA_ARGS__);                                   \
        }                                                                            \
    } while (0)

#define _NARWHAL_PRINT_FORMAT(value) \
    _Generic((value), \
        char: "%c", \
        const char: "%c", \
        signed char: "%hhd", \
        const signed char: "%hhd", \
        unsigned char: "%hhu", \
        const unsigned char: "%hhu", \
        signed short: "%hd", \
        const signed short: "%hd", \
        unsigned short: "%hu", \
        const unsigned short: "%hu", \
        signed int: "%d", \
        const signed int: "%d", \
        unsigned int: "%u", \
        const unsigned int: "%u", \
        long int: "%ld", \
        const long int: "%ld", \
        unsigned long int: "%lu", \
        const unsigned long int: "%lu", \
        long long int: "%lld", \
        const long long int: "%lld", \
        unsigned long long int: "%llu", \
        const unsigned long long int: "%llu", \
        float: "%f", \
        const float: "%f", \
        double: "%f", \
        const double: "%f", \
        long double: "%Lf", \
        const long double: "%Lf", \
        char *: "\"%s\"", \
        const char *: "\"%s\"", \
        bool: "%d", \
        default: "%p")

#define _NARWHAL_PROCESS_VALUE(value) \
    _Generic((value), \
    char *: narwhal_assertion_process_string((char *)(uintptr_t)(value)), \
    const char *: narwhal_assertion_process_string((char *)(uintptr_t)(value)), \
    default: (value))

#define _NARWHAL_BINARY_ASSERTION(left, right, check, assertion, message)               \
    do                                                                                  \
    {                                                                                   \
        __typeof__(left) _narwhal_assert_left = (left);                                 \
        __typeof__(right) _narwhal_assert_right = (right);                              \
        if (narwhal_check_assertion(_narwhal_current_test,                              \
                                    check(_narwhal_assert_left, _narwhal_assert_right), \
                                    assertion,                                          \
                                    __FILE__,                                           \
                                    __LINE__))                                          \
        {                                                                               \
            char _narwhal_assert_message[1024];                                         \
            snprintf(_narwhal_assert_message,                                           \
                     sizeof(_narwhal_assert_message),                                   \
                     message,                                                           \
                     _NARWHAL_PRINT_FORMAT(_narwhal_assert_left),                       \
                     _NARWHAL_PRINT_FORMAT(_narwhal_assert_right));                     \
            _NARWHAL_TEST_FAILURE(_narwhal_assert_message,                              \
                                  _NARWHAL_PROCESS_VALUE(_narwhal_assert_left),         \
                                  _NARWHAL_PROCESS_VALUE(_narwhal_assert_right));       \
        }                                                                               \
    } while (0)

#define _NARWHAL_CHECK_EQ(left, right) \
    _Generic((left), \
    char *: _Generic((right), \
        char *: narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right)), \
        const char *: narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right)), \
        default: false), \
    const char *: _Generic((right), \
        char *: narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right)), \
        const char *: narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right)), \
        default: false), \
    default: (left) == (right))

#define _NARWHAL_CHECK_NE(left, right) \
    _Generic((left), \
    char *: _Generic((right), \
        char *: (!narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right))), \
        const char *: (!narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right))), \
        default: true), \
    const char *: _Generic((right), \
        char *: (!narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right))), \
        const char *: (!narwhal_check_string_equal((char *)(uintptr_t)(left), (char *)(uintptr_t)(right))), \
        default: true), \
    default: (left) != (right))

#define _NARWHAL_CHECK_LT(left, right) (left) < (right)

#define _NARWHAL_CHECK_LE(left, right) (left) <= (right)

#define _NARWHAL_CHECK_GT(left, right) (left) > (right)

#define _NARWHAL_CHECK_GE(left, right) (left) >= (right)

#define _NARWHAL_CHECK_SUBSTRING(left, right) narwhal_check_substring(left, right)

#define _NARWHAL_CHECK_NOT_SUBSTRING(left, right) (!narwhal_check_substring(left, right))

#define ASSERT_EQ(left, right)                     \
    _NARWHAL_BINARY_ASSERTION(left,                \
                              right,               \
                              _NARWHAL_CHECK_EQ,   \
                              #left " == " #right, \
                              "First argument %s is not equal to %s.")

#define ASSERT_NE(left, right)                     \
    _NARWHAL_BINARY_ASSERTION(left,                \
                              right,               \
                              _NARWHAL_CHECK_NE,   \
                              #left " != " #right, \
                              "First argument %s is not different from %s.")

#define ASSERT_LT(left, right)                    \
    _NARWHAL_BINARY_ASSERTION(left,               \
                              right,              \
                              _NARWHAL_CHECK_LT,  \
                              #left " < " #right, \
                              "First argument %s is not less than %s.")

#define ASSERT_LE(left, right)                     \
    _NARWHAL_BINARY_ASSERTION(left,                \
                              right,               \
                              _NARWHAL_CHECK_LE,   \
                              #left " <= " #right, \
                              "First argument %s is not less than or equal to %s.")

#define ASSERT_GT(left, right)                    \
    _NARWHAL_BINARY_ASSERTION(left,               \
                              right,              \
                              _NARWHAL_CHECK_GT,  \
                              #left " > " #right, \
                              "First argument %s is not greater than %s.")

#define ASSERT_GE(left, right)                     \
    _NARWHAL_BINARY_ASSERTION(left,                \
                              right,               \
                              _NARWHAL_CHECK_GE,   \
                              #left " >= " #right, \
                              "First argument %s is not greater or equal to %s.")

#define ASSERT_SUBSTRING(string, substring)                                  \
    _NARWHAL_BINARY_ASSERTION(string,                                        \
                              substring,                                     \
                              _NARWHAL_CHECK_SUBSTRING,                      \
                              "strstr(" #string ", " #substring ") != NULL", \
                              "First argument %s doesn't contain %s.")

#define ASSERT_NOT_SUBSTRING(string, substring)                              \
    _NARWHAL_BINARY_ASSERTION(string,                                        \
                              substring,                                     \
                              _NARWHAL_CHECK_NOT_SUBSTRING,                  \
                              "strstr(" #string ", " #substring ") == NULL", \
                              "First argument %s contains %s.")

#define ASSERT_MEMORY(left, right, size)                                             \
    do                                                                               \
    {                                                                                \
        if (narwhal_check_assertion(                                                 \
                _narwhal_current_test,                                               \
                narwhal_check_memory_equal((left), (right), (size), sizeof(*right)), \
                "memcmp(" #left ", " #right ", " #size ") == 0",                     \
                __FILE__,                                                            \
                __LINE__))                                                           \
        {                                                                            \
            _NARWHAL_TEST_FAILURE("");                                               \
        }                                                                            \
    } while (0)

#endif

// #include "narwhal/collection/collection.h"
#ifndef NARWHAL_COLLECTION_H
#define NARWHAL_COLLECTION_H

#include <stdbool.h>
#include <stdlib.h>

// #include "narwhal/types.h"


struct NarwhalCollection
{
    size_t count;
    NarwhalCollectionItem *first;
    NarwhalCollectionItem *last;
};

struct NarwhalCollectionItem
{
    void *value;
    NarwhalCollectionItem *previous;
    NarwhalCollectionItem *next;
};

NarwhalCollection *narwhal_empty_collection(void);
void narwhal_collection_append(NarwhalCollection *collection, void *value);
void *narwhal_collection_pop(NarwhalCollection *collection);
void narwhal_free_collection(NarwhalCollection *collection);

// Foreach macros

#define NARWHAL_EACH(item_variable, collection)                                             \
    for (NarwhalCollectionItem *_narwhal_loop_item = (collection)->first;                   \
         _narwhal_loop_item != NULL && ((item_variable) = _narwhal_loop_item->value, true); \
         _narwhal_loop_item = _narwhal_loop_item->next)

#define NARWHAL_REVERSED(item_variable, collection)                                         \
    for (NarwhalCollectionItem *_narwhal_loop_item = (collection)->last;                    \
         _narwhal_loop_item != NULL && ((item_variable) = _narwhal_loop_item->value, true); \
         _narwhal_loop_item = _narwhal_loop_item->previous)

#endif

// #include "narwhal/diff/diff.h"
#ifndef NARWHAL_DIFF_H
#define NARWHAL_DIFF_H

#include <stdlib.h>

// #include "narwhal/types.h"


struct NarwhalDiffMatrix
{
    size_t rows;
    size_t columns;
    int *content;
};

enum NarwhalDiffChunkType
{
    NARWHAL_DIFF_CHUNK_TYPE_MATCHED,
    NARWHAL_DIFF_CHUNK_TYPE_ADDED,
    NARWHAL_DIFF_CHUNK_TYPE_REPLACED,
    NARWHAL_DIFF_CHUNK_TYPE_DELETED
};

struct NarwhalDiff
{
    size_t size;
    NarwhalDiffChunk *chunks;
};

struct NarwhalDiffChunk
{
    NarwhalDiffChunkType type;
    size_t original_start;
    size_t original_end;
    size_t modified_start;
    size_t modified_end;
};

NarwhalDiffMatrix *narwhal_new_diff_matrix(size_t rows, size_t columns);
NarwhalDiffMatrix *narwhal_new_diff_matrix_from_lengths(size_t original_length,
                                                        size_t modified_lengths);
void narwhal_diff_matrix_fill_from_strings(NarwhalDiffMatrix *diff_matrix,
                                           const char *original,
                                           const char *modified);
void narwhal_diff_matrix_fill_from_lines(NarwhalDiffMatrix *diff_matrix,
                                         const char *original,
                                         const char *modified);
NarwhalDiff narwhal_diff_matrix_get_diff(const NarwhalDiffMatrix *diff_matrix);

size_t narwhal_diff_matrix_index(const NarwhalDiffMatrix *diff_matrix, size_t row, size_t column);
int narwhal_diff_matrix_get(const NarwhalDiffMatrix *diff_matrix, size_t row, size_t column);
void narwhal_diff_matrix_set(const NarwhalDiffMatrix *diff_matrix,
                             size_t row,
                             size_t column,
                             int value);

NarwhalDiff narwhal_diff_strings_lengths(const char *original,
                                         size_t original_length,
                                         const char *modified,
                                         size_t modified_length);
NarwhalDiff narwhal_diff_strings(const char *original, const char *modified);
NarwhalDiff narwhal_diff_lines(const char *original, const char *modified);

void narwhal_free_diff_matrix(NarwhalDiffMatrix *diff_matrix);

#endif

// #include "narwhal/discovery/discovery.h"
#ifndef NARWHAL_DISCOVERY_H
#define NARWHAL_DISCOVERY_H

// #include "narwhal/concat_macro.h"
#ifndef NARWHAL_CONCAT_MACRO_H
#define NARWHAL_CONCAT_MACRO_H

#define __NARWHAL_CONCAT(A, B) A##B
#define _NARWHAL_CONCAT(A, B) __NARWHAL_CONCAT(A, B)

#endif

// #include "narwhal/types.h"


struct NarwhalTestDiscoveryQueue
{
    NarwhalGroupItemRegistration test_handle;
    NarwhalTestDiscoveryQueue *next;
};

void narwhal_register_test_for_discovery(NarwhalTestDiscoveryQueue *test_entry);
void narwhal_add_test_to_discovery_queue(NarwhalTestDiscoveryQueue *test_entry,
                                         NarwhalTestDiscoveryQueue **queue);

NarwhalTestGroup *narwhal_discover_tests();
NarwhalTestGroup *narwhal_collect_tests_from_discovery_queue(const char *group_name,
                                                             NarwhalTestDiscoveryQueue *queue);

#ifdef __GNUC__

#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY(test_name)                     \
    _NARWHAL_CONCAT(_NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_, \
                    DISABLE_TEST_DISCOVERY)                                 \
    (test_name)

#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_0(test_name)               \
    __attribute__((constructor)) static void _narwhal_test_constructor_##test_name(void) \
    {                                                                                    \
        static NarwhalTestDiscoveryQueue test_entry = { test_name, NULL };               \
        narwhal_register_test_for_discovery(&test_entry);                                \
    }

#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_DISABLE_TEST_DISCOVERY(test_name) \
    _NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_0(test_name)

#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_1(test_name)
#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY_WHEN_DISABLED_IS_(test_name)

#else

#define _NARWHAL_REGISTER_TEST_FOR_DISCOVERY(test_name)

#endif

#endif

// #include "narwhal/fixture/fixture.h"
#ifndef NARWHAL_FIXTURE_H
#define NARWHAL_FIXTURE_H

#include <stdlib.h>

// #include "narwhal/types.h"


extern NarwhalCollection *_narwhal_current_fixtures;

struct NarwhalTestFixture
{
    const char *name;
    size_t size;
    void *value;
    NarwhalTestFixtureSetup setup;
    NarwhalTestFixtureCleanup cleanup;
    NarwhalTest *test;
    NarwhalCollection *accessible_fixtures;
    NarwhalCollection *accessible_params;
};

NarwhalTestFixture *narwhal_new_test_fixture(const char *name,
                                             size_t fixture_size,
                                             NarwhalTestFixtureSetup setup,
                                             NarwhalTest *test,
                                             NarwhalTestModifierRegistration *test_modifiers,
                                             size_t modifier_count);
NarwhalTestFixture *narwhal_get_test_fixture(const NarwhalCollection *fixtures,
                                             const char *fixture_name);
void narwhal_free_test_fixture(NarwhalTestFixture *test_fixture);

#define DECLARE_FIXTURE(fixture_name, fixture_type)            \
    typedef fixture_type _narwhal_fixture_type_##fixture_name; \
    void fixture_name(NarwhalTest *test, NarwhalCollection *params, NarwhalCollection *fixtures)

#define TEST_FIXTURE(fixture_name, fixture_type, ...)                                           \
    DECLARE_FIXTURE(fixture_name, fixture_type);                                                \
    static NarwhalTestModifierRegistration _narwhal_test_fixture_modifiers_##fixture_name[] = { \
        __VA_ARGS__                                                                             \
    };                                                                                          \
    void _narwhal_fixture_##fixture_name##_cleanup(                                             \
        _narwhal_fixture_type_##fixture_name *fixture_name,                                     \
        NarwhalTestFixture *_narwhal_test_fixture);                                             \
    void _narwhal_fixture_##fixture_name##_call_cleanup(void *value,                            \
                                                        NarwhalTestFixture *test_fixture);      \
    static void _narwhal_fixture_##fixture_name##_setup(                                        \
        _narwhal_fixture_type_##fixture_name *fixture_name,                                     \
        NarwhalTestFixture *_narwhal_test_fixture);                                             \
    static void _narwhal_fixture_##fixture_name##_call_setup(void *value,                       \
                                                             NarwhalTestFixture *test_fixture)  \
    {                                                                                           \
        _narwhal_fixture_##fixture_name##_setup((_narwhal_fixture_type_##fixture_name *)value,  \
                                                test_fixture);                                  \
    }                                                                                           \
    void fixture_name(NarwhalTest *test,                                                        \
                      _NARWHAL_UNUSED NarwhalCollection *params,                                \
                      NarwhalCollection *fixtures)                                              \
    {                                                                                           \
        narwhal_register_test_fixture(                                                          \
            test,                                                                               \
            fixtures,                                                                           \
            #fixture_name,                                                                      \
            sizeof(_narwhal_fixture_type_##fixture_name),                                       \
            _narwhal_fixture_##fixture_name##_call_setup,                                       \
            _narwhal_test_fixture_modifiers_##fixture_name,                                     \
            sizeof(_narwhal_test_fixture_modifiers_##fixture_name) /                            \
                sizeof(*_narwhal_test_fixture_modifiers_##fixture_name));                       \
    }                                                                                           \
    static void _narwhal_fixture_##fixture_name##_setup(                                        \
        _NARWHAL_UNUSED _narwhal_fixture_type_##fixture_name *fixture_name,                     \
        _NARWHAL_UNUSED NarwhalTestFixture *_narwhal_test_fixture)

#define GET_FIXTURE(fixture_name)                                                                 \
    _narwhal_fixture_type_##fixture_name fixture_name;                                            \
    do                                                                                            \
    {                                                                                             \
        NarwhalTestFixture *_narwhal_test_fixture_##fixture_name =                                \
            narwhal_get_test_fixture(_narwhal_current_fixtures, #fixture_name);                   \
        if (_narwhal_test_fixture_##fixture_name == NULL)                                         \
        {                                                                                         \
            FAIL("Fixture \"%s\" hasn't been applied to the current context.", #fixture_name);    \
        }                                                                                         \
        fixture_name =                                                                            \
            *(_narwhal_fixture_type_##fixture_name *)_narwhal_test_fixture_##fixture_name->value; \
    } while (0)

#define CLEANUP_FIXTURE(fixture_name)                                                            \
                                                                                                 \
    _narwhal_test_fixture->cleanup = _narwhal_fixture_##fixture_name##_call_cleanup;             \
    }                                                                                            \
    void _narwhal_fixture_##fixture_name##_call_cleanup(void *value,                             \
                                                        NarwhalTestFixture *test_fixture)        \
    {                                                                                            \
        _narwhal_fixture_##fixture_name##_cleanup((_narwhal_fixture_type_##fixture_name *)value, \
                                                  test_fixture);                                 \
    }                                                                                            \
    void _narwhal_fixture_##fixture_name##_cleanup(                                              \
        _NARWHAL_UNUSED _narwhal_fixture_type_##fixture_name *fixture_name,                      \
        _NARWHAL_UNUSED NarwhalTestFixture *_narwhal_test_fixture)                               \
    {
#endif

// #include "narwhal/group/group.h"
#ifndef NARWHAL_GROUP_H
#define NARWHAL_GROUP_H

#include <stdbool.h>
#include <stdlib.h>

// #include "narwhal/types.h"

// #include "narwhal/unused_attribute.h"
#ifndef NARWHAL_UNUSED_ATTRIBUTE_H
#define NARWHAL_UNUSED_ATTRIBUTE_H

#ifdef __GNUC__
#define _NARWHAL_UNUSED __attribute__((unused))
#else
#define _NARWHAL_UNUSED
#endif

#endif


struct NarwhalTestGroup
{
    const char *name;
    bool only;
    NarwhalTestGroup *group;
    NarwhalCollection *subgroups;
    NarwhalCollection *tests;
};

NarwhalTestGroup *narwhal_new_test_group(const char *name,
                                         NarwhalGroupItemRegistration *group_items,
                                         size_t item_count);
void narwhal_register_subgroup(NarwhalTestGroup *test_group,
                               const char *name,
                               NarwhalGroupItemRegistration *group_items,
                               size_t item_count);
void narwhal_register_test(NarwhalTestGroup *test_group,
                           const char *name,
                           const char *filename,
                           size_t line_number,
                           NarwhalTestFunction function,
                           NarwhalTestModifierRegistration *test_modifiers,
                           size_t modifier_count);

void narwhal_free_test_group(NarwhalTestGroup *test_group);

#define DECLARE_GROUP(group_name) void group_name(NarwhalTestGroup *test_group)

#define TEST_GROUP(group_name, ...)                                                        \
    DECLARE_GROUP(group_name);                                                             \
    static NarwhalGroupItemRegistration _narwhal_group_items_##group_name[] = __VA_ARGS__; \
    void group_name(NarwhalTestGroup *test_group)                                          \
    {                                                                                      \
        narwhal_register_subgroup(test_group,                                              \
                                  #group_name,                                             \
                                  _narwhal_group_items_##group_name,                       \
                                  sizeof(_narwhal_group_items_##group_name) /              \
                                      sizeof(*_narwhal_group_items_##group_name));         \
    }

#endif

// #include "narwhal/hexdump/hexdump.h"
#ifndef NARWHAL_HEXDUMP_H
#define NARWHAL_HEXDUMP_H

#include <stdint.h>
#include <stdlib.h>

char *narwhal_hexdump(const uint8_t *buffer, size_t size, size_t bytes_per_row);
size_t narwhal_optimal_bytes_per_row(size_t element_size, size_t target, size_t range);

#endif

// #include "narwhal/output/output.h"
#ifndef NARWHAL_OUTPUT_H
#define NARWHAL_OUTPUT_H

#include <stdio.h>

// #include "narwhal/types.h"


void narwhal_output_string(FILE *stream,
                           const char *string,
                           size_t line_number,
                           const char *indent);

void narwhal_output_session_init(NarwhalTestSession *test_session);
void narwhal_output_session_progress(NarwhalTestSession *test_session);
void narwhal_output_session_result(const NarwhalTestSession *test_session);

#endif

// #include "narwhal/param/param.h"
#ifndef NARWHAL_PARAM_H
#define NARWHAL_PARAM_H

#include <stdlib.h>

// #include "narwhal/types.h"


extern NarwhalCollection *_narwhal_current_params;

struct NarwhalTestParam
{
    const char *name;
    size_t index;
    size_t count;
    const void *values;
    NarwhalTest *test;
};

NarwhalTestParam *narwhal_new_test_param(const char *name,
                                         const void *values,
                                         size_t count,
                                         NarwhalTest *test);
NarwhalTestParam *narwhal_get_test_param(const NarwhalCollection *params, const char *param_name);
void narwhal_free_test_param(NarwhalTestParam *test_param);

#define DECLARE_PARAM(param_name, param_type)            \
    typedef param_type _narwhal_param_type_##param_name; \
    void param_name(NarwhalTest *test, NarwhalCollection *params, NarwhalCollection *fixtures)

#define TEST_PARAM(param_name, param_type, ...)                                          \
    DECLARE_PARAM(param_name, param_type);                                               \
    static _narwhal_param_type_##param_name _narwhal_param_##param_name[] = __VA_ARGS__; \
    void param_name(NarwhalTest *test,                                                   \
                    NarwhalCollection *params,                                           \
                    _NARWHAL_UNUSED NarwhalCollection *fixtures)                         \
    {                                                                                    \
        narwhal_register_test_param(                                                     \
            test,                                                                        \
            params,                                                                      \
            #param_name,                                                                 \
            _narwhal_param_##param_name,                                                 \
            sizeof(_narwhal_param_##param_name) / sizeof(*_narwhal_param_##param_name)); \
    }

#define GET_PARAM(param_name)                                                                  \
    _narwhal_param_type_##param_name param_name;                                               \
    do                                                                                         \
    {                                                                                          \
        NarwhalTestParam *_narwhal_test_param_##param_name =                                   \
            narwhal_get_test_param(_narwhal_current_params, #param_name);                      \
        if (_narwhal_test_param_##param_name == NULL)                                          \
        {                                                                                      \
            FAIL("Parameter \"%s\" hasn't been applied to the current context.", #param_name); \
        }                                                                                      \
        param_name = ((_narwhal_param_type_##param_name *)_narwhal_test_param_##param_name     \
                          ->values)[_narwhal_test_param_##param_name->index];                  \
    } while (0)

#endif

// #include "narwhal/result/result.h"
#ifndef NARWHAL_RESULT_H
#define NARWHAL_RESULT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// #include "narwhal/types.h"


struct NarwhalTestResult
{
    bool success;
    char *failed_assertion;
    char *error_message;
    char *assertion_file;
    size_t assertion_line;
    NarwhalTest *test;
    NarwhalCollection *param_snapshots;
    struct timeval start_time;
    struct timeval end_time;
    int pipe[2];
    int output_pipe[2];
    char *output_buffer;
    size_t output_length;
    char *diff_original;
    size_t diff_original_size;
    char *diff_modified;
    size_t diff_modified_size;
};

NarwhalTestResult *narwhal_new_test_result(void);

bool narwhal_test_result_has_diff(const NarwhalTestResult *test_result);

void narwhal_pipe_test_info(NarwhalTestResult *test_result,
                            struct timeval start_time,
                            struct timeval end_time);
void narwhal_pipe_assertion_failure(NarwhalTestResult *test_result,
                                    const char *failed_assertion,
                                    const char *assertion_file,
                                    size_t assertion_line);
void narwhal_pipe_error_message(NarwhalTestResult *test_result,
                                const char *error_message,
                                size_t message_size);

void narwhal_set_assertion_failure(NarwhalTestResult *test_result,
                                   const char *failed_assertion,
                                   const char *assertion_file,
                                   size_t assertion_line);
void narwhal_set_error_message(NarwhalTestResult *test_result,
                               const char *error_message,
                               size_t message_size);
void narwhal_free_test_result(NarwhalTestResult *test_result);

struct NarwhalTestParamSnapshot
{
    const NarwhalTestParam *param;
    size_t index;
};

NarwhalTestParamSnapshot *narwhal_new_test_param_snapshot(const NarwhalTestParam *test_param);
void narwhal_free_test_param_snapshot(NarwhalTestParamSnapshot *param_snapshot);

#endif

// #include "narwhal/session/session.h"
#ifndef NARWHAL_SESSION_H
#define NARWHAL_SESSION_H

#include <stdbool.h>
#include <sys/time.h>

// #include "narwhal/types.h"


struct NarwhalSessionOutputState
{
    int index;
    int length;
    char string[50];
};

struct NarwhalTestSession
{
    NarwhalCollection *results;
    NarwhalCollection *failures;
    struct timeval start_time;
    struct timeval end_time;
    NarwhalSessionOutputState output_state;
};

NarwhalTestSession *narwhal_new_test_session(void);

void narwhal_test_session_start(NarwhalTestSession *test_session);
void narwhal_test_session_end(NarwhalTestSession *test_session);

void narwhal_test_session_run_test(NarwhalTestSession *test_session, NarwhalTest *test);
void narwhal_test_session_run_parameterized_test(NarwhalTestSession *test_session,
                                                 NarwhalTest *test,
                                                 NarwhalCollectionItem *param_item);
void narwhal_test_session_run_test_group(NarwhalTestSession *test_session,
                                         NarwhalTestGroup *test_group,
                                         bool only);

void narwhal_free_test_session(NarwhalTestSession *test_session);

#endif

// #include "narwhal/test/test.h"
#ifndef NARWHAL_TEST_H
#define NARWHAL_TEST_H

#include <stdbool.h>
#include <stdlib.h>

// #include "narwhal/discovery/discovery.h"

// #include "narwhal/types.h"


extern NarwhalTest *_narwhal_current_test;

struct NarwhalTest
{
    const char *name;
    const char *filename;
    size_t line_number;
    bool only;
    bool skip;
    NarwhalTestGroup *group;
    NarwhalTestFunction function;
    NarwhalCollection *resources;
    NarwhalCollection *fixtures;
    NarwhalCollection *params;
    NarwhalCollection *accessible_fixtures;
    NarwhalCollection *accessible_params;
    NarwhalTestResult *result;
    NarwhalOutputCapture *output_capture;
};

NarwhalTest *narwhal_new_test(const char *name,
                              const char *filename,
                              size_t line_number,
                              NarwhalTestFunction function,
                              NarwhalTestModifierRegistration *test_modifiers,
                              size_t modifier_count);
void narwhal_run_test(NarwhalTest *test);

void narwhal_free_after_test(NarwhalTest *test, void *resource);
void auto_free(void *resource);
void *narwhal_test_resource(NarwhalTest *test, size_t size);
void *test_resource(size_t size);
void narwhal_free_test_resources(NarwhalTest *test);

void narwhal_register_test_fixture(NarwhalTest *test,
                                   NarwhalCollection *access_collection,
                                   const char *name,
                                   size_t fixture_size,
                                   NarwhalTestFixtureSetup setup,
                                   NarwhalTestModifierRegistration *test_modifiers,
                                   size_t modifier_count);
void narwhal_register_test_param(NarwhalTest *test,
                                 NarwhalCollection *access_collection,
                                 const char *name,
                                 const void *values,
                                 size_t count);
void narwhal_test_set_only(NarwhalTest *test,
                           NarwhalCollection *params,
                           NarwhalCollection *fixtures);
void narwhal_test_set_skip(NarwhalTest *test,
                           NarwhalCollection *params,
                           NarwhalCollection *fixtures);

void narwhal_free_test(NarwhalTest *test);

#define DECLARE_TEST(test_name) void test_name(NarwhalTestGroup *test_group)

#define TEST(test_name, ...)                                                                 \
    DECLARE_TEST(test_name);                                                                 \
    NarwhalTestModifierRegistration _narwhal_test_modifiers_##test_name[] = { __VA_ARGS__ }; \
    static void _narwhal_test_function_##test_name(void);                                    \
    void test_name(NarwhalTestGroup *test_group)                                             \
    {                                                                                        \
        narwhal_register_test(test_group,                                                    \
                              #test_name,                                                    \
                              __FILE__,                                                      \
                              __LINE__,                                                      \
                              _narwhal_test_function_##test_name,                            \
                              _narwhal_test_modifiers_##test_name,                           \
                              sizeof(_narwhal_test_modifiers_##test_name) /                  \
                                  sizeof(*_narwhal_test_modifiers_##test_name));             \
    }                                                                                        \
    _NARWHAL_REGISTER_TEST_FOR_DISCOVERY(test_name)                                          \
    static void _narwhal_test_function_##test_name(void)

#define ONLY narwhal_test_set_only
#define SKIP narwhal_test_set_skip

#endif

// #include "narwhal/test_utils/test_utils.h"
#ifndef NARWHAL_TEST_UTILS_H
#define NARWHAL_TEST_UTILS_H

#include <stdbool.h>

// #include "narwhal/types.h"


extern NarwhalOutputCapture _narwhal_default_output_capture;

struct NarwhalOutputCapture
{
    bool initialization_phase;
    int stdout_backup;
    int stderr_backup;
    int pipe[2];
    NarwhalOutputCapture *parent;
};

bool narwhal_capture_output(NarwhalOutputCapture *capture, char **output_buffer);

#define CAPTURE_OUTPUT(buffer_name)                                                        \
    char *buffer_name = NULL;                                                              \
    NarwhalOutputCapture _narwhal_capture_##buffer_name = _narwhal_default_output_capture; \
    while (narwhal_capture_output(&_narwhal_capture_##buffer_name, &buffer_name))

#endif

// #include "narwhal/types.h"


int narwhal_run_tests(NarwhalGroupItemRegistration *tests, size_t test_count);
int narwhal_run_root_group(NarwhalTestGroup *root_group);

#define RUN_TESTS(...)                                                          \
    narwhal_run_tests((NarwhalGroupItemRegistration[]){ __VA_ARGS__ },          \
                      sizeof((NarwhalGroupItemRegistration[]){ __VA_ARGS__ }) / \
                          sizeof(NarwhalGroupItemRegistration))

#endif
