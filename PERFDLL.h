/* PERFDLL.h - DosQueryPerformance() dynalink library, public release 1.0.0 (build 1)
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

#if !defined(__PERFDLL)
#define __PERFDLL

#if defined(__cplusplus)
extern "C" {
#endif

/* performance information structure */
typedef struct _PERFORMANCE
{
    ULONG ulNumCPUs;             /* # of available CPUs */
    struct
    {
        FIXED fxIdleRatio;       /* idle ratio in FIXED percentage (65536 means 1.0) */
        FIXED fxBusyRatio;       /* busy ratio in FIXED percentage (65536 means 1.0) */
        FIXED fxInterruptRatio;  /* interrupt ratio in FIXED percentage (65536 means 1.0) */
    } Overall, ByCPU[64];        /* performance informations, by overall, and by each CPUs */
} PERFORMANCE, * PPERFORMANCE;

/* queries performance information
   return : !NO_ERROR -> error occurred
            NO_ERROR  -> successful completion */
extern APIRET APIENTRY DosQueryPerformance(PPERFORMANCE pxPerfInfo  /* (output) pointer to structure that performance information will be set to */);

#if defined(__cplusplus)
}
#endif

#endif

