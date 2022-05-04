/** @file
Host-based unit test for the VariableRuntimeDxe driver. Will
use mocks for all external interfaces.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef VARIABLE_RUNTIME_DXE_UNIT_TEST_H_
#define VARIABLE_RUNTIME_DXE_UNIT_TEST_H_

#include <Uefi.h>
#include <Uefi/UefiMultiPhase.h>
#include <Guid/VariableFormat.h>
#include <Guid/GlobalVariable.h>

#define SKIP_SIGDATA        FALSE
#define INCLUDE_SIGDATA     TRUE

#define VAR_TYPE_STANDARD   0x00
#define VAR_TYPE_TIME_AUTH  0x01

#define DATA_ENC_HEX     0x00
#define DATA_ENC_BASE64  0x01
#define DATA_ENC_CHAR8   0x02

#define TEST_SIGNER_1  1
#define TEST_SIGNER_2  2

typedef struct _TEST_VARIABLE_HEADER {
  CHAR8       *TestName;
  CHAR16      *Name;
  EFI_GUID    VendorGuid;
  UINT32      Attributes;
  UINT32      VarType;
  CHAR8       *Data;
  UINT32      DataEnc;
} TEST_VARIABLE_HEADER;

typedef struct _TEST_VARIABLE_AUTH {
  TEST_VARIABLE_HEADER    Header;
  EFI_TIME                Timestamp;
  CHAR8                   *SigData;
  UINT32                  SigDataEnc;
} TEST_VARIABLE_AUTH;

typedef struct _TEST_VARIABLE_MODEL {
  CHAR8       *TestName;
  CHAR16      *Name;
  EFI_GUID    VendorGuid;
  UINT32      Attributes;
  UINT32      VarType;
  UINT8       *Data;            // ALLOCATED
  UINT32      DataSize;
  UINT8       *SigData;         // ALLOCATED, OPTIONAL
  UINT32      SigDataSize;      // OPTIONAL
  EFI_TIME    Timestamp;        // OPTIONAL
} TEST_VARIABLE_MODEL;
#define     T_VAR  TEST_VARIABLE_MODEL

BOOLEAN
SignAuthVar (
  IN OUT TEST_VARIABLE_MODEL  *Model,
  IN     UINT8                SignerId
  );

UINT8 *
AssembleAuthPayload (
  IN CONST TEST_VARIABLE_MODEL  *Model,
  IN       BOOLEAN              IncludeSig,
  OUT      UINT32               *BufferSize
  );

UINT8 *
SignAndAssembleAuthPayload (
  IN OUT TEST_VARIABLE_MODEL  *Model,
  IN     UINT8                SignerId,
  OUT    UINT32               *BufferSize
  );

TEST_VARIABLE_MODEL *
LoadTestVariable (
  IN CONST    CHAR8  *TestName
  );

VOID
FreeTestVariable (
  IN OUT      TEST_VARIABLE_MODEL  *VarModel
  );

VOID
UpdateVariableData (
  OUT      TEST_VARIABLE_MODEL  *Model,
  IN CONST CHAR8                *NewData,
  IN       UINT32               DataEnc
  );

#define DUMP_HEX(ErrorLevel,                                                      \
                 Offset,                                                          \
                 Data,                                                            \
                 DataSize,                                                        \
                 LinePrefixFormat,                                                \
                 ...)                                                             \
    do {                                                                            \
      if (DebugPrintEnabled () && DebugPrintLevelEnabled (ErrorLevel))  {           \
        UINT8 *_DataToDump;                                                         \
        UINT8 _Val[50];                                                             \
        UINT8 _Str[20];                                                             \
        UINT8 _TempByte;                                                            \
        UINTN _Size;                                                                \
        UINTN _DumpHexIndex;                                                        \
        UINTN _LocalOffset;                                                         \
        UINTN _LocalDataSize;                                                       \
        CONST CHAR8 *_Hex = "0123456789ABCDEF";                                     \
        _LocalOffset = (Offset);                                                    \
        _LocalDataSize = (DataSize);                                                \
        _DataToDump = (UINT8 *)(Data);                                              \
                                                                                    \
        ASSERT (_DataToDump != NULL);                                               \
                                                                                    \
        while (_LocalDataSize != 0) {                                               \
          _Size = 16;                                                               \
          if (_Size > _LocalDataSize) {                                             \
            _Size = _LocalDataSize;                                                 \
          }                                                                         \
                                                                                    \
          for (_DumpHexIndex = 0; _DumpHexIndex < _Size; _DumpHexIndex += 1) {      \
            _TempByte            = (UINT8) _DataToDump[_DumpHexIndex];              \
            _Val[_DumpHexIndex * 3 + 0]  = (UINT8) _Hex[_TempByte >> 4];            \
            _Val[_DumpHexIndex * 3 + 1]  = (UINT8) _Hex[_TempByte & 0xF];           \
            _Val[_DumpHexIndex * 3 + 2]  =                                          \
              (CHAR8) ((_DumpHexIndex == 7) ? '-' : ' ');                           \
            _Str[_DumpHexIndex]          =                                          \
              (CHAR8) ((_TempByte < ' ' || _TempByte > '~') ? '.' : _TempByte);     \
          }                                                                         \
                                                                                    \
          _Val[_DumpHexIndex * 3]  = 0;                                             \
          _Str[_DumpHexIndex]      = 0;                                             \
                                                                                    \
          DebugPrint(ErrorLevel, LinePrefixFormat, ##__VA_ARGS__);                  \
          DebugPrint(ErrorLevel, "%08X: %-48a *%a*\r\n", _LocalOffset, _Val, _Str); \
          _DataToDump = (UINT8 *)(((UINTN)_DataToDump) + _Size);                    \
          _LocalOffset += _Size;                                                    \
          _LocalDataSize -= _Size;                                                  \
        }                                                                           \
      }                                                                             \
    } while (FALSE)

//
// Test Data Externs
//

extern TEST_VARIABLE_HEADER  *mGlobalTestVarDb[];
extern UINT32                mGlobalTestVarDbCount;

extern UINT8   TestCert1[];
extern UINT32  TestCert1Size;
extern UINT8   TestKey1[];
extern UINT32  TestKey1Size;
extern UINT8   TestCert2[];
extern UINT32  TestCert2Size;
extern UINT8   TestKey2[];
extern UINT32  TestKey2Size;

#endif // VARIABLE_RUNTIME_DXE_UNIT_TEST_H_
