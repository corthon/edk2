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

STATIC
UINT8*
DecodeBase64String (
    IN CONST  CHAR8     *Data
    )
{
    UINTN   SourceSize, DestSize;
    UINT8   *Result;

    SourceSize = AsciiStrLen(Data);
    DestSize = 0;

    if (EFI_ERROR(Base64Decode(Data, SourceSize, NULL, &DestSize))) {
        return NULL;
    }

    Result = AllocatePool(DestSize);

    if (Result != NULL && EFI_ERROR(Base64Decode(Data, SourceSize, Result, &DestSize))) {
        FreePool(Result);
        Result = NULL;
    }

    return Result;
}

STATIC
UINT8*
DecodeHexString (
    IN CONST  CHAR8     *Data
    )
{
    return NULL;
}

STATIC
UINT8*
DecodeDataString (
    IN        UINT32    Type,
    IN CONST  CHAR8     *Data
    )
{
    UINT8       *Result;

    switch (Type) {
        case DATA_ENC_BASE64:
            Result = DecodeBase64String(Data);
            break;
        default:
            Result = DecodeHexString(Data);
            break;
    }

    return Result;
}

TEST_VARIABLE_MODEL*
LoadTestVariable (
    IN CONST    CHAR8      *TestName
    )
{
    UINTN                       Index;
    TEST_VARIABLE_HEADER        *FoundVariable;
    TEST_VARIABLE_AUTH          *FoundAuthVariable;
    TEST_VARIABLE_MODEL         *NewModel;
    UINT8                       *Data, *SigData;

    FoundVariable = NULL;
    FoundAuthVariable = NULL;
    NewModel = NULL;

    Data = NULL;
    SigData = NULL;

    for (Index = 0; Index < mGlobalTestVarDbCount; Index++) {
        if (AsciiStrCmp(TestName, mGlobalTestVarDb[Index]->TestName) == 0) {
            FoundVariable = mGlobalTestVarDb[Index];
            break;
        }
    }

    if (FoundVariable != NULL) {
        NewModel = AllocateZeroPool(sizeof(*NewModel));
        if (NewModel == NULL) {
            return NULL;
        }
    }

    if (NewModel == NULL) {
        if (Data != NULL) {
            FreePool(Data);
        }
        if (SigData != NULL) {
            FreePool(SigData);
        }
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
