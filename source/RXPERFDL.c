/* RXPERFDL.c - REXX RxQueryPerformance() dynalink library, public release 1.0.0 (build 1)
     Copyright (c) 2001 Takayuki 'January June' Suwa

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#pragma strings(readonly)
#include <stdlib.h>
#define INCL_BASE
#define INCL_ERRORS
#include <os2.h>
#define INCL_REXXSAA
#include <rexxsaa.h>
#include "PERFDLL.h"

/*  */
static const PSZ apszFunctionNames[] =
 {(PSZ)"RxQpLoadFuncs",
  (PSZ)"RxQpDropFuncs",
  (PSZ)"RxQueryPerformance"};

/*  */
ULONG APIENTRY RxQpLoadFuncs(PSZ pszName,
                             ULONG argc,
                             RXSTRING argv[],
                             PSZ pszQueue,
                             PRXSTRING retvar)
{
    ULONG ulIndex;
    if(argc == (ULONG)0)
    {
        for(ulIndex = (ULONG)0;
            ulIndex < (ULONG)(sizeof(apszFunctionNames) / sizeof(PSZ));
            ulIndex++)
            (VOID)RexxRegisterFunctionDll(apszFunctionNames[ulIndex],
                                          (PSZ)"RXPERFDL",
                                          apszFunctionNames[ulIndex]);
        retvar->strlength = (ULONG)0;
        return RXFUNC_OK;
    }
    return RXFUNC_BADTYPE;
}

/*  */
ULONG APIENTRY RxQpDropFuncs(PSZ pszName,
                             ULONG argc,
                             RXSTRING argv[],
                             PSZ pszQueue,
                             PRXSTRING retvar)
{
    ULONG ulIndex;
    if(argc == (ULONG)0)
    {
        for(ulIndex = (ULONG)0;
            ulIndex < (ULONG)(sizeof(apszFunctionNames) / sizeof(PSZ));
            ulIndex++)
            (VOID)RexxDeregisterFunction(apszFunctionNames[ulIndex]);
        retvar->strlength = (ULONG)0;
        return RXFUNC_OK;
    }
    return RXFUNC_BADTYPE;
}

/*  */
static char* _cpy(char* d,
                  const char* s)
{
    char c;
    while((c = *s) != '\0')
    {
        s++;
        if(c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        *d++ = c;
    }
    *d = '\0';
    return d;
}

/*  */
static PVOID MakeSHVBLOCK(PVOID pvBuffer,
                          PSZ apszVariableNames[4],
                          PSZ pszVariableValue,
                          PSHVBLOCK pxChainSHVBLOCK)
{
    PSHVBLOCK pxSHVBLOCK;
    PSZ pszWork;
    ULONG ulIndex;
    pxSHVBLOCK = (PSHVBLOCK)pvBuffer;
    pvBuffer = (PVOID)&((PSHVBLOCK)pvBuffer)[1];
    pxSHVBLOCK->shvnext = pxChainSHVBLOCK;
    pxSHVBLOCK->shvcode = RXSHV_SET;
    pszWork = (PSZ)pvBuffer;
    for(ulIndex = (ULONG)0;
        ulIndex < (ULONG)4;
        ulIndex++)
        if(apszVariableNames[ulIndex] != (PSZ)NULL)
            pszWork = (PSZ)_cpy((char*)pszWork,
                                (const char*)apszVariableNames[ulIndex]);
    pxSHVBLOCK->shvnamelen = pxSHVBLOCK->shvname.strlength = (ULONG)(pszWork - (PSZ)pvBuffer);
    pxSHVBLOCK->shvname.strptr = (PCH)pvBuffer;
    pvBuffer = (PVOID)&pszWork[1];
    pszWork = (PSZ)_cpy((char*)pvBuffer,
                        (const char*)pszVariableValue);
    pxSHVBLOCK->shvvaluelen = pxSHVBLOCK->shvvalue.strlength = (ULONG)(pszWork - (PSZ)pvBuffer);
    pxSHVBLOCK->shvvalue.strptr = (PCH)pvBuffer;
    return (PVOID)&pszWork[1];
}

/*  */
ULONG APIENTRY RxQueryPerformance(PSZ pszName,
                                  ULONG argc,
                                  RXSTRING argv[],
                                  PSZ pszQueue,
                                  PRXSTRING retvar)
{
    PERFORMANCE xPerfInfo;
    PSHVBLOCK pxChainSHVBLOCK;
    UCHAR aucBuffer[8192];
    PVOID pvBuffer, pvBufferNew;
    PSZ apszVariableNames[4];
    UCHAR aucValue[8];
    ULONG ulIndex;
    UCHAR aucIndex[4];
    if(argc == (ULONG)1)
    {
        retvar->strlength = (ULONG)1;
        if(DosQueryPerformance(&xPerfInfo) == NO_ERROR)
        {
            pxChainSHVBLOCK = (PSHVBLOCK)NULL;
            pvBuffer = (PVOID)&aucBuffer[0];
            apszVariableNames[0] = (PSZ)argv[0].strptr;  /* stem.ulNumCPUs */
            apszVariableNames[1] = (PSZ)".ulNumCPUs";
            apszVariableNames[2] = (PSZ)NULL;
            apszVariableNames[3] = (PSZ)NULL;
            (void)_ultoa((unsigned long)xPerfInfo.ulNumCPUs,
                         (char*)&aucValue[0],
                         10);
            pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                       apszVariableNames,
                                       (PSZ)&aucValue[0],
                                       pxChainSHVBLOCK);
            pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
            pvBuffer = pvBufferNew;
            apszVariableNames[1] = (PSZ)".Overall.fxIdleRatio";  /* stem.Overall.fxIdleRatio */
            (void)_ultoa((unsigned long)xPerfInfo.Overall.fxIdleRatio,
                         (char*)&aucValue[0],
                         10);
            pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                       apszVariableNames,
                                       (PSZ)&aucValue[0],
                                       pxChainSHVBLOCK);
            pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
            pvBuffer = pvBufferNew;
            apszVariableNames[1] = (PSZ)".Overall.fxBusyRatio";  /* stem.Overall.fxBusyRatio */
            (void)_ultoa((unsigned long)xPerfInfo.Overall.fxBusyRatio,
                         (char*)&aucValue[0],
                         10);
            pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                       apszVariableNames,
                                       (PSZ)&aucValue[0],
                                       pxChainSHVBLOCK);
            pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
            pvBuffer = pvBufferNew;
            apszVariableNames[1] = (PSZ)".Overall.fxInterruptRatio";  /* stem.Overall.fxInterruptRatio */
            (void)_ultoa((unsigned long)xPerfInfo.Overall.fxInterruptRatio,
                         (char*)&aucValue[0],
                         10);
            pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                       apszVariableNames,
                                       (PSZ)&aucValue[0],
                                       pxChainSHVBLOCK);
            pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
            pvBuffer = pvBufferNew;
            apszVariableNames[1] = (PSZ)".ByCPU.";
            apszVariableNames[2] = (PSZ)&aucIndex[0];
            for(ulIndex = (ULONG)0;
                ulIndex < xPerfInfo.ulNumCPUs;
                ulIndex++)
            {
                (void)_ultoa((unsigned long)ulIndex,
                             (char*)&aucIndex[0],
                             10);
                apszVariableNames[3] = (PSZ)".fxIdleRatio";  /* stem.ByCPU.n.fxIdleRatio */
                (void)_ultoa((unsigned long)xPerfInfo.ByCPU[ulIndex].fxIdleRatio,
                             (char*)&aucValue[0],
                             10);
                pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                           apszVariableNames,
                                           (PSZ)&aucValue[0],
                                           pxChainSHVBLOCK);
                pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
                pvBuffer = pvBufferNew;
                apszVariableNames[3] = (PSZ)".fxBusyRatio";  /* stem.ByCPU.n.fxBusyRatio */
                (void)_ultoa((unsigned long)xPerfInfo.ByCPU[ulIndex].fxBusyRatio,
                             (char*)&aucValue[0],
                             10);
                pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                           apszVariableNames,
                                           (PSZ)&aucValue[0],
                                           pxChainSHVBLOCK);
                pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
                pvBuffer = pvBufferNew;
                apszVariableNames[3] = (PSZ)".fxInterruptRatio";  /* stem.ByCPU.n.fxInterruptRatio */
                (void)_ultoa((unsigned long)xPerfInfo.ByCPU[ulIndex].fxInterruptRatio,
                             (char*)&aucValue[0],
                             10);
                pvBufferNew = MakeSHVBLOCK(pvBuffer,
                                           apszVariableNames,
                                           (PSZ)&aucValue[0],
                                           pxChainSHVBLOCK);
                pxChainSHVBLOCK = (PSHVBLOCK)pvBuffer;
                pvBuffer = pvBufferNew;
            }
            (VOID)RexxVariablePool(pxChainSHVBLOCK);
            *retvar->strptr = (UCHAR)'0';  /* NO_ERROR */
        }
        else
            *retvar->strptr = (UCHAR)'1';  /* ERROR_INVALID_FUNCTION */
        return RXFUNC_OK;
    }
    return RXFUNC_BADTYPE;
}

