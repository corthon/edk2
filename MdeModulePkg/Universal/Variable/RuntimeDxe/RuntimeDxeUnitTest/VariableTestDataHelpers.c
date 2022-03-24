/** @file
Helper functions for working with the declared test data.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "VariableRuntimeDxeUnitTest.h"

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

extern TEST_VARIABLE_HEADER    *mGlobalTestVarDb[];
extern UINT32                  mGlobalTestVarDbCount;


TEST_VARIABLE_MODEL*
LoadTestVariable (
    IN CONST    CHAR8      *TestName
    )
{
    UINTN                       Index;
    TEST_VARIABLE_HEADER        *FoundVariable;
    TEST_VARIABLE_AUTH          *FoundAuthVariable;
    TEST_VARIABLE_MODEL         *NewModel;

    FoundVariable = NULL;
    FoundAuthVariable = NULL;
    NewModel = NULL;

    for (Index = 0; Index < mGlobalTestVarDbCount; Index++) {
        if (AsciiStrCmp(TestName, mGlobalTestVarDb[Index]->TestName) == 0) {
            FoundVariable = mGlobalTestVarDb[Index];
            break;
        }
    }

    if (FoundVariable != NULL) {
        // Stuff.
    }

    return NewModel;
}

VOID
FreeTestVariable (
    IN OUT      TEST_VARIABLE_MODEL     *VarModel
    )
{
    ASSERT(VarModel != NULL);
    if (VarModel->Data != NULL) {
        FreePool(VarModel->Data);
    }
    if (VarModel->SigData != NULL) {
        FreePool(VarModel->SigData);
    }
    FreePool(VarModel);
}
