// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umockcallrecorder.h"
#include "umockcall.h"

/*
TODO:
- add tests
*/

static UMOCKCALL_HANDLE test_expected_umockcall_1 = (UMOCKCALL_HANDLE)0x4242;
static UMOCKCALL_HANDLE test_expected_umockcall_2 = (UMOCKCALL_HANDLE)0x4243;
static UMOCKCALL_HANDLE test_actual_umockcall_1 = (UMOCKCALL_HANDLE)0x4244;

typedef struct umockcall_are_equal_CALL_TAG
{
    UMOCKCALL_HANDLE left;
    UMOCKCALL_HANDLE right;
} umockcall_are_equal_CALL;

static umockcall_are_equal_CALL* umockcall_are_equal_calls;
static size_t umockcall_are_equal_call_count;
static int umockcall_are_equal_call_result;

typedef struct umockcall_destroy_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_destroy_CALL;

static umockcall_destroy_CALL* umockcall_destroy_calls;
static size_t umockcall_destroy_call_count;
static int umockcall_destroy_call_result;

int umockcall_are_equal(UMOCKCALL_HANDLE left, UMOCKCALL_HANDLE right)
{
    umockcall_are_equal_CALL* new_calls = (umockcall_are_equal_CALL*)realloc(umockcall_are_equal_calls, sizeof(umockcall_are_equal_CALL) * (umockcall_are_equal_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_are_equal_calls = new_calls;
        umockcall_are_equal_calls[umockcall_are_equal_call_count].left = left;
        umockcall_are_equal_calls[umockcall_are_equal_call_count].right = right;
        umockcall_are_equal_call_count++;
    }

    return umockcall_are_equal_call_result;
}

void umockcall_destroy(UMOCKCALL_HANDLE umockcall)
{
    umockcall_destroy_CALL* new_calls = (umockcall_destroy_CALL*)realloc(umockcall_destroy_calls, sizeof(umockcall_destroy_CALL) * (umockcall_destroy_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_destroy_calls = new_calls;
        umockcall_destroy_calls[umockcall_destroy_call_count].umockcall = umockcall;
        umockcall_destroy_call_count++;
    }
}

char* umockcall_stringify(UMOCKCALL_HANDLE umockcall)
{
    return NULL;
}

void reset_umockcall_are_equal_calls(void)
{
    if (umockcall_are_equal_calls != NULL)
    {
        free(umockcall_are_equal_calls);
        umockcall_are_equal_calls = NULL;
    }

    umockcall_are_equal_call_count = 0;
}

void reset_umockcall_destroy_calls(void)
{
    if (umockcall_destroy_calls != NULL)
    {
        free(umockcall_destroy_calls);
        umockcall_destroy_calls = NULL;
    }

    umockcall_destroy_call_count = 0;
}

extern "C"
{
    static size_t malloc_call_count;
    static size_t calloc_call_count;
    static size_t realloc_call_count;
    static size_t free_call_count;

    static size_t when_shall_malloc_fail;
    static size_t when_shall_calloc_fail;
    static size_t when_shall_realloc_fail;

    void* mock_malloc(size_t size)
    {
        void* result;
        malloc_call_count++;
        if (malloc_call_count == when_shall_malloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = malloc(size);
        }
        return result;
    }

    void* mock_calloc(size_t nmemb, size_t size)
    {
        void* result;
        calloc_call_count++;
        if (calloc_call_count == when_shall_calloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = calloc(nmemb, size);
        }
        return result;
    }

    void* mock_realloc(void* ptr, size_t size)
    {
        void* result;
        realloc_call_count++;
        if (realloc_call_count == when_shall_realloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
        }
        return result;
    }

    void mock_free(void* ptr)
    {
        free_call_count++;
        free(ptr);
    }

    void reset_malloc_calls(void)
    {
        malloc_call_count = 0;
        when_shall_malloc_fail = 0;
        calloc_call_count = 0;
        when_shall_calloc_fail = 0;
        realloc_call_count = 0;
        when_shall_realloc_fail = 0;
        free_call_count = 0;
    }
}

TEST_MUTEX_HANDLE test_mutex;

BEGIN_TEST_SUITE(umockcallrecorder_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    TEST_MUTEX_DESTROY(test_mutex);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    ASSERT_ARE_EQUAL(int, 0, TEST_MUTEX_ACQUIRE(test_mutex));

    reset_umockcall_are_equal_calls();
    reset_umockcall_destroy_calls();
    reset_malloc_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    TEST_MUTEX_RELEASE(test_mutex);
}

/* umockcallrecorder_create */

/* Tests_SRS_UMOCKCALLRECORDER_01_001: [ umockcallrecorder_create shall create a new instance of a call recorder and return a non-NULL handle to it on success. ]*/
TEST_FUNCTION(umockcallrecorder_create_succeeds)
{
    // arrange

    // act
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_002: [ If allocating memory for the call recorder fails, umockcallrecorder_create shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_umockcallrecorder_create_fails)
{
    // arrange
    when_shall_malloc_fail = 1;

    // act
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // assert
    ASSERT_IS_NULL(call_recorder);
}

/* umockcallrecorder_destroy */

/* Tests_SRS_UMOCKCALLRECORDER_01_003: [ umockcallrecorder_destroy shall free the resources associated with a the call recorder identified by the umock_call_recorder argument. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_frees_the_call_recorder_resources)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    umockcallrecorder_destroy(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);
    ASSERT_ARE_EQUAL(size_t, 3, free_call_count);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_004: [ If umock_call_recorder is NULL, umockcallrecorder_destroy shall do nothing. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_with_NULL_does_nothing)
{
    // arrange

    // act
    umockcallrecorder_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_003: [ umockcallrecorder_destroy shall free the resources associated with a the call recorder identified by the umock_call_recorder argument. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_with_one_expected_call_frees_the_call_recorder_resources)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;

    umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    umockcallrecorder_destroy(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);
    /* 5 = 1 for actual calls, 1 for expected calls, 2 for the strings and one for the call recorder structure */
    ASSERT_ARE_EQUAL(size_t, 5, free_call_count);
    ASSERT_ARE_EQUAL(size_t, 3, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_1, umockcall_destroy_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_2, umockcall_destroy_calls[1].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_actual_umockcall_1, umockcall_destroy_calls[2].umockcall);
}

/* umockcallrecorder_reset_all_calls */

/* Tests_SRS_UMOCKCALLRECORDER_01_005: [ umockcallrecorder_reset_all_calls shall free all the expected and actual calls for the call recorder identified by umock_call_recorder. ]*/
TEST_FUNCTION(umockcallrecorder_reset_all_calls_frees_all_existing_expected_and_actual_calls)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;

    umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    umockcallrecorder_reset_all_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(size_t, 2, free_call_count);
    ASSERT_ARE_EQUAL(size_t, 3, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_1, umockcall_destroy_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_2, umockcall_destroy_calls[1].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_actual_umockcall_1, umockcall_destroy_calls[2].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

END_TEST_SUITE(umockcallrecorder_unittests)