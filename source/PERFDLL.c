/* PERFDLL.c - DosQueryPerformance() dynalink library, public release 1.0.0 (build 1)
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
#define INCL_BASE
#define INCL_ERRORS
#include <os2.h>
#include "PERFDLL.h"

#if !defined(CMD_KI_RDCNT)

/* constants for DosPerfSysCall() */
#define ORD_DOS32PERFSYSCALL 976
#define CMD_KI_RDCNT 0x63

/* structures for DosPerfSysCall() */
typedef struct _CPUUTIL
{
    QWORD qwTime;  /* time stamp     */
    QWORD qwIdle;  /* idle time      */
    QWORD qwBusy;  /* busy time      */
    QWORD qwIntr;  /* interrupt time */
} CPUUTIL, * PCPUUTIL;

#endif

/* constants for DosQuerySysInfo() */
#if !defined(QSV_NUMPROCESSORS)
#define QSV_NUMPROCESSORS 26
#endif

/* internal per-process variables */
APIRET (* APIENTRY G_pfDosPerfSysCall)(ULONG, ULONG, ULONG, ULONG);  /* pointer to DosPerfSysCall() if exists */
CPUUTIL G_axCPUUtilPrevious[64];                                     /* previously queried CPUUTIL data */
HMTX G_hmtx;                                                         /* mutex semaphore handle */
ULONG G_ulNumCPUs;                                                   /* # of available CPUs */

/* DLL entrypoint for initialization/termination */
ULONG APIENTRY os2entry(HMODULE hmodLibrary,
                        BOOL bTerminate)
{
    HMODULE hmodDOSCALLS;
    if(bTerminate == (BOOL)FALSE)
    {
        if(DosQueryModuleHandle((PSZ)"DOSCALLS",
                                &hmodDOSCALLS) == NO_ERROR &&
           DosQueryProcAddr(hmodDOSCALLS,
                            ORD_DOS32PERFSYSCALL,
                            (PSZ)NULL,
                            (PFN*)&G_pfDosPerfSysCall) == NO_ERROR &&
           G_pfDosPerfSysCall(CMD_KI_RDCNT,
                              (ULONG)&G_axCPUUtilPrevious[0],
                              (ULONG)0,
                              (ULONG)0) == NO_ERROR)
        {
            (VOID)DosCreateMutexSem((PSZ)NULL,
                                    &G_hmtx,
                                    (ULONG)0,
                                    (BOOL32)FALSE);
            G_ulNumCPUs = (ULONG)1;
            (VOID)DosQuerySysInfo(QSV_NUMPROCESSORS,
                                  QSV_NUMPROCESSORS,
                                  (PVOID)&G_ulNumCPUs,
                                  (ULONG)sizeof(G_ulNumCPUs));
        }
    }
    else
    {
        if(G_ulNumCPUs != (ULONG)0)
            (VOID)DosCloseMutexSem(G_hmtx);
    }
    return (ULONG)-1;
}

/* assembler part */
extern VOID APIENTRY SubDQP(PPERFORMANCE pxPerfInfo,
                            PCPUUTIL pxCPUUtilPrevious,
                            PCPUUTIL pxCPUUtilCurrent,
                            ULONG ulNumCPUs);

/* queries performance information */
APIRET DosQueryPerformance(PPERFORMANCE pxPerfInfo)
{
    CPUUTIL axCPUUtil[64];
    if(G_ulNumCPUs != (ULONG)0)
    {
        (VOID)G_pfDosPerfSysCall(CMD_KI_RDCNT,
                                 (ULONG)&axCPUUtil[0],
                                 (ULONG)0,
                                 (ULONG)0);
        (VOID)DosRequestMutexSem(G_hmtx,
                                 SEM_INDEFINITE_WAIT);
        SubDQP(pxPerfInfo,
               &G_axCPUUtilPrevious[0],
               &axCPUUtil[0],
               G_ulNumCPUs);
        (VOID)DosReleaseMutexSem(G_hmtx);
        return NO_ERROR;
    }
    return ERROR_INVALID_FUNCTION;
}

