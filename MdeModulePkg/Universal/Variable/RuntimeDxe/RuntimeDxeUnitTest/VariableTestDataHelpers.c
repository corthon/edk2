/** @file
Helper functions for working with the declared test data.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "VariableRuntimeDxeUnitTest.h"

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/MemoryAllocationLib.h>

STATIC
UINT8 *
DecodeBase64String (
  IN CONST  CHAR8   *Data,
  OUT       UINT32  *OutputSize
  )
{
  UINTN  SourceSize, DestSize;
  UINT8  *Result;

  ASSERT (Data != NULL);
  ASSERT (OutputSize != NULL);

  SourceSize = AsciiStrLen (Data);
  DestSize   = 0;
  Result     = NULL;

  if (EFI_ERROR (Base64Decode (Data, SourceSize, NULL, &DestSize))) {
    return NULL;
  }

  Result = AllocatePool (DestSize);

  if ((Result != NULL) && EFI_ERROR (Base64Decode (Data, SourceSize, Result, &DestSize))) {
    FreePool (Result);
    Result = NULL;
  } else {
    *OutputSize = (UINT32)DestSize;
  }

  return Result;
}

STATIC
UINT8 *
DecodeHexString (
  IN CONST  CHAR8   *Data,
  OUT       UINT32  *OutputSize
  )
{
  UINTN  SourceSize, DestSize;
  UINT8  *Result;

  ASSERT (Data != NULL);
  ASSERT (OutputSize != NULL);

  SourceSize = AsciiStrLen (Data);
  DestSize   = 0;
  Result     = NULL;

  ASSERT ((SourceSize & 0x1) == 0);
  DestSize = SourceSize >> 1;
  Result   = AllocatePool (DestSize);

  if ((Result != NULL) && EFI_ERROR (AsciiStrHexToBytes (Data, SourceSize, Result, DestSize))) {
    FreePool (Result);
    Result = NULL;
  } else {
    *OutputSize = (UINT32)DestSize;
  }

  return Result;
}

STATIC
UINT8 *
CopyChar8String (
  IN CONST  CHAR8   *Data,
  OUT       UINT32  *OutputSize
  )
{
  UINTN  SourceSize;
  UINT8  *Result;

  ASSERT (Data != NULL);
  ASSERT (OutputSize != NULL);

  // Skip the NULL.
  SourceSize = AsciiStrLen (Data);
  Result     = NULL;

  Result = AllocatePool (SourceSize);
  if (Result != NULL) {
    CopyMem (Result, Data, SourceSize);
    *OutputSize = (UINT32)SourceSize;
  }

  return Result;
}

STATIC
UINT8 *
DecodeDataString (
  IN        UINT32  Encoding,
  IN CONST  CHAR8   *Data,
  OUT       UINT32  *OutputSize
  )
{
  UINT8  *Result;

  switch (Encoding) {
    case DATA_ENC_BASE64:
      Result = DecodeBase64String (Data, OutputSize);
      break;
    case DATA_ENC_CHAR8:
      Result = CopyChar8String (Data, OutputSize);
      break;
    default:
      Result = DecodeHexString (Data, OutputSize);
      break;
  }

  return Result;
}

STATIC
UINT8 *
GetAuthVarTbsBuffer (
  IN CONST TEST_VARIABLE_MODEL  *Model,
  OUT      UINT32               *BufferSize
  )
{
  UINT32  NameWoNullSize, AllocBufferSize;
  UINT8   *Result, *Mark;

  *BufferSize = 0;

  NameWoNullSize  = (UINT32)StrSize (Model->Name) - 2;
  AllocBufferSize = NameWoNullSize + sizeof (EFI_GUID) + sizeof (UINT32) + sizeof (EFI_TIME) + Model->DataSize;
  Result          = AllocatePool (AllocBufferSize);

  if (Result != NULL) {
    *BufferSize = AllocBufferSize;
    Mark        = Result;

    CopyMem (Mark, Model->Name, NameWoNullSize);
    Mark += NameWoNullSize;

    CopyGuid ((EFI_GUID *)Mark, &Model->VendorGuid);
    Mark += sizeof (EFI_GUID);

    *(UINT32 *)Mark = Model->Attributes;
    Mark           += sizeof (UINT32);

    CopyMem (Mark, &Model->Timestamp, sizeof (EFI_TIME));
    Mark += sizeof (EFI_TIME);

    CopyMem (Mark, Model->Data, Model->DataSize);
  }

  return Result;
}

STATIC
BOOLEAN
ShouldHaveSigData (
  IN CONST TEST_VARIABLE_MODEL  *Model
  )
{
  return (Model->VarType == VAR_TYPE_TIME_AUTH);
}

BOOLEAN
SignAuthVar (
  IN OUT TEST_VARIABLE_MODEL  *Model,
  IN     UINT8                SignerId
  )
{
  UINT8   *TbsBuffer;
  UINT32  TbsBufferSize;
  UINT8   *Key;
  UINT32  KeySize;
  UINT8   *Cert;
  UINT32  CertSize;
  UINTN   SigDataSize;

  ASSERT (Model != NULL);
  ASSERT (ShouldHaveSigData (Model));

  TbsBuffer     = NULL;
  TbsBufferSize = 0;

  if (Model->SigData != NULL) {
    FreePool (Model->SigData);
    Model->SigData     = NULL;
    Model->SigDataSize = 0;
  }

  TbsBuffer = GetAuthVarTbsBuffer (Model, &TbsBufferSize);
  ASSERT (TbsBuffer != NULL);

  DUMP_HEX (DEBUG_ERROR, 0, TbsBuffer, TbsBufferSize, "TBS ");

  switch (SignerId) {
    case TEST_SIGNER_1:
      Key      = TestKey1;
      KeySize  = TestKey1Size;
      Cert     = TestCert1;
      CertSize = TestCert1Size;
      break;
    default:
      Key      = TestKey2;
      KeySize  = TestKey2Size;
      Cert     = TestCert2;
      CertSize = TestCert2Size;
      break;
  }

  ASSERT (
    Pkcs7Sign (
      Key,
      KeySize,
      "",
      TbsBuffer,
      TbsBufferSize,
      Cert,
      CertSize,
      NULL,
      &Model->SigData,
      &SigDataSize
      )
    );
  Model->SigDataSize = (UINT32)SigDataSize;

  FreePool (TbsBuffer);

  return TRUE;
}

UINT8 *
AssembleAuthPayload (
  IN CONST TEST_VARIABLE_MODEL  *Model,
  OUT      UINT32               *BufferSize
  )
{
  UINT8                          *Result;
  UINT8                          *Mark;
  UINTN                          ResultSize;
  EFI_VARIABLE_AUTHENTICATION_2  *AuthData;

  ASSERT (Model != NULL);
  ASSERT (BufferSize != NULL);
  ASSERT (ShouldHaveSigData (Model));
  ASSERT (Model->SigData != NULL);

  Result      = NULL;
  Mark        = NULL;
  AuthData    = NULL;
  *BufferSize = 0;

  ResultSize = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) +
               OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData) +
               Model->SigDataSize + Model->DataSize;
  Result = AllocatePool (ResultSize);
  ASSERT (Result != NULL);
  *BufferSize = (UINT32)ResultSize;

  AuthData = (EFI_VARIABLE_AUTHENTICATION_2 *)Result;
  CopyMem (&AuthData->TimeStamp, &Model->Timestamp, sizeof (EFI_TIME));

  AuthData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData) + Model->SigDataSize;
  AuthData->AuthInfo.Hdr.wRevision        = 0x0200; // Defined in MdePkg/Include/Guid/WinCertificate.h
  AuthData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;

  CopyGuid (&AuthData->AuthInfo.CertType, &gEfiCertPkcs7Guid);
  CopyMem (&AuthData->AuthInfo.CertData[0], Model->SigData, Model->SigDataSize);

  Mark = Result + OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) +
         AuthData->AuthInfo.Hdr.dwLength;
  CopyMem (Mark, Model->Data, Model->DataSize);

  return Result;
}

UINT8 *
SignAndAssembleAuthPayload (
  IN OUT TEST_VARIABLE_MODEL  *Model,
  IN     UINT8                SignerId,
  OUT    UINT32               *BufferSize
  )
{
  ASSERT (SignAuthVar (Model, SignerId));
  return AssembleAuthPayload (Model, BufferSize);
}

TEST_VARIABLE_MODEL *
LoadTestVariable (
  IN CONST    CHAR8  *TestName
  )
{
  UINTN                 Index;
  TEST_VARIABLE_HEADER  *FoundVariable;
  TEST_VARIABLE_AUTH    *FoundAuthVariable;
  TEST_VARIABLE_MODEL   *NewModel;
  UINT8                 *Data, *SigData;

  FoundVariable     = NULL;
  FoundAuthVariable = NULL;
  NewModel          = NULL;

  Data    = NULL;
  SigData = NULL;

  for (Index = 0; Index < mGlobalTestVarDbCount; Index++) {
    if (AsciiStrCmp (TestName, mGlobalTestVarDb[Index]->TestName) == 0) {
      FoundVariable = mGlobalTestVarDb[Index];
      break;
    }
  }

  if (FoundVariable != NULL) {
    NewModel = AllocateZeroPool (sizeof (*NewModel));
    if (NewModel == NULL) {
      return NULL;
    }

    CopyMem (NewModel, FoundVariable, OFFSET_OF (TEST_VARIABLE_HEADER, Data));
    NewModel->Data = DecodeDataString (
                       FoundVariable->DataEnc,
                       FoundVariable->Data,
                       &NewModel->DataSize
                       );

    if (FoundVariable->VarType == VAR_TYPE_TIME_AUTH) {
      FoundAuthVariable = (TEST_VARIABLE_AUTH *)FoundVariable;
      CopyMem (&NewModel->Timestamp, &FoundAuthVariable->Timestamp, sizeof (EFI_TIME));
      NewModel->SigData = DecodeDataString (
                            FoundAuthVariable->SigDataEnc,
                            FoundAuthVariable->SigData,
                            &NewModel->SigDataSize
                            );
    }

    if ((NewModel->Data == NULL) ||
        (ShouldHaveSigData (NewModel) && (NewModel->SigData == NULL)))
    {
      FreeTestVariable (NewModel);
      NewModel = NULL;
    }
  }

  return NewModel;
}

VOID
FreeTestVariable (
  IN OUT      TEST_VARIABLE_MODEL  *VarModel
  )
{
  ASSERT (VarModel != NULL);
  if (VarModel->Data != NULL) {
    FreePool (VarModel->Data);
  }

  if (VarModel->SigData != NULL) {
    FreePool (VarModel->SigData);
  }

  FreePool (VarModel);
}

VOID
UpdateVariableData (
  OUT      TEST_VARIABLE_MODEL  *Model,
  IN CONST CHAR8                *NewData,
  IN       UINT32               DataEnc
  )
{
  ASSERT (Model != NULL);
  if (Model->Data != NULL) {
    FreePool (Model->Data);
  }

  Model->Data = DecodeDataString (DataEnc, NewData, &Model->DataSize);
}
