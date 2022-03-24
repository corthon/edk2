/** @file
Alternate ASSERT macros.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _UNIT_TEST_ASSERT_CLEANUP_H_
#define _UNIT_TEST_ASSERT_CLEANUP_H_

#include <Library/UnitTestLib.h>

#define UT_CLEANUP_ASSERT_TRUE(Expression)                                                         \
  if(!UnitTestAssertTrue ((Expression), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #Expression)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                      \
    goto Cleanup;                                                                                  \
  }

#define UT_CLEANUP_ASSERT_FALSE(Expression)                                                         \
  if(!UnitTestAssertFalse ((Expression), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #Expression)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                       \
    goto Cleanup;                                                                                   \
  }

#define UT_CLEANUP_ASSERT_EQUAL(ValueA, ValueB)                                                                                \
  if(!UnitTestAssertEqual ((UINT64)(ValueA), (UINT64)(ValueB), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #ValueA, #ValueB)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                                                  \
    goto Cleanup;                                                                                                              \
  }

#define UT_CLEANUP_ASSERT_MEM_EQUAL(BufferA, BufferB, Length)                                                                                                      \
  if(!UnitTestAssertMemEqual ((VOID *)(UINTN)(BufferA), (VOID *)(UINTN)(BufferB), (UINTN)Length, __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #BufferA, #BufferB)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                                                                                      \
    goto Cleanup;                                                                                                                                                  \
  }

#define UT_CLEANUP_ASSERT_NOT_EQUAL(ValueA, ValueB)                                                                               \
  if(!UnitTestAssertNotEqual ((UINT64)(ValueA), (UINT64)(ValueB), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #ValueA, #ValueB)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                                                     \
    goto Cleanup;                                                                                                                 \
  }

#define UT_CLEANUP_ASSERT_NOT_EFI_ERROR(Status)                                                   \
  if(!UnitTestAssertNotEfiError ((Status), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #Status)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                     \
    goto Cleanup;                                                                                 \
  }

#define UT_CLEANUP_ASSERT_STATUS_EQUAL(Status, Expected)                                                      \
  if(!UnitTestAssertStatusEqual ((Status), (Expected), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #Status)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                                 \
    goto Cleanup;                                                                                             \
  }

#define UT_CLEANUP_ASSERT_NOT_NULL(Pointer)                                                     \
  if(!UnitTestAssertNotNull ((Pointer), __FUNCTION__, DEBUG_LINE_NUMBER, __FILE__, #Pointer)) { \
    TestResult = UNIT_TEST_ERROR_TEST_FAILED;                                                   \
    goto Cleanup;                                                                               \
  }

#endif // _UNIT_TEST_ASSERT_CLEANUP_H_
