/** @file -- VariableRuntimeDxeUnitTest.c
Host-based unit test for the VariableRuntimeDxe driver. Will
use mocks for all external interfaces.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UnitTestLib.h>
#include <Library/DebugLib.h>

#include "Variable.h"

#define UNIT_TEST_NAME        "RuntimeVariableDxe Host-Based Unit Test"
#define UNIT_TEST_VERSION     "1.0"


BOOLEAN   mTestAtRuntime = FALSE;

///
/// Mock version of the UEFI Runtime Services Table
///
EFI_RUNTIME_SERVICES  MockRuntime = {
  {
    EFI_RUNTIME_SERVICES_SIGNATURE,     // Signature
    EFI_RUNTIME_SERVICES_REVISION,      // Revision
    sizeof (EFI_RUNTIME_SERVICES),      // HeaderSize
    0,                                  // CRC32
    0                                   // Reserved
  },
  NULL,               // GetTime
  NULL,               // SetTime
  NULL,               // GetWakeupTime
  NULL,               // SetWakeupTime
  NULL,               // SetVirtualAddressMap
  NULL,               // ConvertPointer
  NULL,               // GetVariable
  NULL,               // GetNextVariableName
  NULL,               // SetVariable
  NULL,               // GetNextHighMonotonicCount
  NULL,               // ResetSystem
  NULL,               // UpdateCapsule
  NULL,               // QueryCapsuleCapabilities
  NULL                // QueryVariableInfo
};

/**
  Return TRUE if ExitBootServices () has been called.

  @retval TRUE If ExitBootServices () has been called.
**/
BOOLEAN
AtRuntime (
  VOID
  )
{
  return mTestAtRuntime;
}

/**
  Initializes a basic mutual exclusion lock.

  This function initializes a basic mutual exclusion lock to the released state
  and returns the lock.  Each lock provides mutual exclusion access at its task
  priority level.  Since there is no preemption or multiprocessor support in EFI,
  acquiring the lock only consists of raising to the locks TPL.
  If Lock is NULL, then ASSERT().
  If Priority is not a valid TPL value, then ASSERT().

  @param  Lock       A pointer to the lock data structure to initialize.
  @param  Priority   EFI TPL is associated with the lock.

  @return The lock.

**/
EFI_LOCK *
InitializeLock (
  IN OUT EFI_LOCK  *Lock,
  IN     EFI_TPL   Priority
  )
{
  return EfiInitializeLock (Lock, Priority);
}

/**
  Acquires lock only at boot time. Simply returns at runtime.

  This is a temperary function that will be removed when
  EfiAcquireLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiAcquireLock() at boot time, and simply returns
  at runtime.

  @param  Lock         A pointer to the lock to acquire.

**/
VOID
AcquireLockOnlyAtBootTime (
  IN EFI_LOCK  *Lock
  )
{
  if (!AtRuntime ()) {
    EfiAcquireLock (Lock);
  }
}

/**
  Releases lock only at boot time. Simply returns at runtime.

  This is a temperary function which will be removed when
  EfiReleaseLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiReleaseLock() at boot time and simply returns
  at runtime.

  @param  Lock         A pointer to the lock to release.

**/
VOID
ReleaseLockOnlyAtBootTime (
  IN EFI_LOCK  *Lock
  )
{
  if (!AtRuntime ()) {
    EfiReleaseLock (Lock);
  }
}

/**
  Retrieve the Fault Tolerent Write protocol interface.

  @param[out] FtwProtocol       The interface of Ftw protocol

  @retval EFI_SUCCESS           The FTW protocol instance was found and returned in FtwProtocol.
  @retval EFI_NOT_FOUND         The FTW protocol instance was not found.
  @retval EFI_INVALID_PARAMETER SarProtocol is NULL.

**/
EFI_STATUS
GetFtwProtocol (
  OUT VOID  **FtwProtocol
  )
{
  // TODO: Create a mocked version.
  return EFI_UNSUPPORTED;
}

/**
  Retrieve the FVB protocol interface by HANDLE.

  @param[in]  FvBlockHandle     The handle of FVB protocol that provides services for
                                reading, writing, and erasing the target block.
  @param[out] FvBlock           The interface of FVB protocol

  @retval EFI_SUCCESS           The interface information for the specified protocol was returned.
  @retval EFI_UNSUPPORTED       The device does not support the FVB protocol.
  @retval EFI_INVALID_PARAMETER FvBlockHandle is not a valid EFI_HANDLE or FvBlock is NULL.

**/
EFI_STATUS
GetFvbByHandle (
  IN  EFI_HANDLE                          FvBlockHandle,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvBlock
  )
{
  //
  // To get the FVB protocol interface on the handle
  //
  // TODO: Create a mocked version.
  return EFI_UNSUPPORTED;
}

/**
  Function returns an array of handles that support the FVB protocol
  in a buffer allocated from pool.

  @param[out]  NumberHandles    The number of handles returned in Buffer.
  @param[out]  Buffer           A pointer to the buffer to return the requested
                                array of  handles that support FVB protocol.

  @retval EFI_SUCCESS           The array of handles was returned in Buffer, and the number of
                                handles in Buffer was returned in NumberHandles.
  @retval EFI_NOT_FOUND         No FVB handle was found.
  @retval EFI_OUT_OF_RESOURCES  There is not enough pool memory to store the matching results.
  @retval EFI_INVALID_PARAMETER NumberHandles is NULL or Buffer is NULL.

**/
EFI_STATUS
GetFvbCountAndBuffer (
  OUT UINTN       *NumberHandles,
  OUT EFI_HANDLE  **Buffer
  )
{
  //
  // Locate all handles of Fvb protocol
  //
  // TODO: Create a mocked version.
  return EFI_UNSUPPORTED;
}

UNIT_TEST_STATUS
EFIAPI
DummyTest (
  IN UNIT_TEST_CONTEXT           Context
  )
{
  return UNIT_TEST_PASSED;
}

/**
  Main fuction sets up the unit test environment
**/
EFI_STATUS
EFIAPI
UefiTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      GenericTests;

  Framework = NULL;

  DEBUG(( DEBUG_INFO, "%a v%a\n", UNIT_TEST_NAME, UNIT_TEST_VERSION ));

  //
  // Start setting up the test framework for running the tests.
  //
  Status = InitUnitTestFramework (&Framework, UNIT_TEST_NAME, gEfiCallerBaseName, UNIT_TEST_VERSION);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  //
  // Populate the StatusCodeHspDriver Unit Test Suite.
  //
  Status = CreateUnitTestSuite (&GenericTests, Framework, "Generic Tests", "Generic", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for GenericTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  AddTestCase (GenericTests, "Dummy Test", "Dummy", DummyTest, NULL, NULL, NULL);


  // NOTE: This initialization should be performed per-suite, probably.
  //       But to do that optimally, I think we'd need to be able to deinit. Dunno.
  //       We'll play around with it.
  ASSERT_EFI_ERROR (VariableCommonInitialize());
  
  Status = RunAllTestSuites (Framework);

  EXIT:
  if (Framework) {
    FreeUnitTestFramework (Framework);
  }

  return Status;
}

/**
  Standard POSIX C entry point for host based unit test execution.
**/
int
main (
  int argc,
  char *argv[]
  )
{
  return UefiTestMain ();
}
