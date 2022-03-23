/** @file
Host-based unit test for the VariableRuntimeDxe driver. Will
use mocks for all external interfaces.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _VARIABLE_RUNTIME_DXE_UNIT_TEST_H_
#define _VARIABLE_RUNTIME_DXE_UNIT_TEST_H_

#define VAR_TYPE_STANDARD       0x00
#define VAR_TYPE_TIME_AUTH      0x01

#define DATA_ENC_HEX            0x00
#define DATA_ENC_BASE64         0x01

typedef struct _TEST_VARIABLE_HEADER {
    CHAR8       *TestName;
    CHAR16      *Name;
    EFI_GUID    VendorGuid;
    CHAR8       *Data;
    UINT32      Attributes;
    UINT32      VarType;
    UINT32      DataEnc;
} TEST_VARIABLE_HEADER;

typedef struct _TEST_VARIABLE_AUTH {
    TEST_VARIABLE_HEADER    Header;
    EFI_TIME                Timestamp;
    CHAR8                   *SigData;
    UINT32                  SigDataEnc;
} TEST_VARIABLE_AUTH;

#endif // _VARIABLE_RUNTIME_DXE_UNIT_TEST_H_
