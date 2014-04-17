; SubDQP.asm - DosQueryPerformance() dynalink library, public release 1.0.0 (build 1)
;   Copyright (c) 2001 Takayuki 'January June' Suwa
;
; This library is free software; you can redistribute it and/or
; modify it under the terms of the GNU Library General Public
; License as published by the Free Software Foundation; either
; version 2 of the License, or (at your option) any later version.
;
; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; Library General Public License for more details.
;
; You should have received a copy of the GNU Library General Public
; License along with this library; if not, write to the
; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
; Boston, MA 02111-1307, USA.

  .386

CODE32  segment  dword use32 public 'CODE'
CODE32  ends
DATA32  segment  dword use32 public 'DATA'
DATA32  ends
CONST32_RO  segment  dword use32 public 'CONST'
CONST32_RO  ends
BSS32  segment  dword use32 public 'BSS'
BSS32  ends
STACK  segment dword use32 stack 'STACK'
STACK  ends
DGROUP  group  BSS32, DATA32

  assume  cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT, fs:nothing, gs:nothing

  extrn  os2entry:near

; constants
CONST32_RO  segment  dword use32 public 'CONST'

dInverseFIXED100  dq  1.5258789062500000e-07  ; 1/6553600

CONST32_RO  ends

; codes
CODE32  segment

CPUUTIL  struc
qwTime  dq  ?
qwIdle  dq  ?
qwBusy  dq  ?
qwIntr  dq  ?
CPUUTIL  ends

PERFORMANCEsub  struc
fxIdleRatio       dd  ?
fxBusyRatio       dd  ?
fxInterruptRatio  dd  ?
PERFORMANCEsub  ends

PERFORMANCE  struc
ulNumCPUs  dd  ?
Overall    PERFORMANCEsub  <>
ByCPU      PERFORMANCEsub  64 dup(<>)
PERFORMANCE  ends

; VOID APIENTRY SubDQP(PPERFORMANCE pxPerfInfo,
;                      PCPUUTIL pxCPUUtilPrevious,
;                      PCPUUTIL pxCPUUtilCurrent,
;                      ULONG ulNumCPUs)
  public  SubDQP
SubDQP  proc  near
ARGLIST  struc
pxPerfInfo         dd  ?  ; (PPERFORMANCE)
pxCPUUtilPrevious  dd  ?  ; (PCPUUTIL)
pxCPUUtilCurrent   dd  ?  ; (PCPUUTIL)
ulNumCPUs          dd  ?  ; (ULONG)
ARGLIST  ends
VARLIST  struc
qwIdleOverall  dq  ?
qwBusyOverall  dq  ?
qwIntrOverall  dq  ?
qwSum          dq  ?
qwIdle         dq  ?
qwBusy         dq  ?
qwIntr         dq  ?
VARLIST  ends
  enter size VARLIST, 0
ARG  equ  ARGLIST[ebp+8]
VAR  equ  VARLIST[ebp-size VARLIST]
  push  ebx
  push  esi
  push  edi

  fld  FLAT:dInverseFIXED100  ; st(0):1/6553600
  mov  ebx, ARG.pxPerfInfo
  mov  esi, ARG.pxCPUUtilCurrent
  mov  edi, ARG.pxCPUUtilPrevious
  xor  eax, eax
  mov  edx, ARG.ulNumCPUs
  mov  dword ptr VAR.qwIdleOverall[0], eax
  mov  dword ptr VAR.qwIdleOverall[4], eax
  mov  dword ptr VAR.qwBusyOverall[0], eax
  mov  dword ptr VAR.qwBusyOverall[4], eax
  mov  dword ptr VAR.qwIntrOverall[0], eax
  mov  dword ptr VAR.qwIntrOverall[4], eax
  mov  PERFORMANCE[ebx].ulNumCPUs, edx
  lea  ebx, PERFORMANCE[ebx].ByCPU

BLBL0:
  mov  eax, dword ptr CPUUTIL[esi].qwIdle[0]
  mov  edx, dword ptr CPUUTIL[esi].qwIdle[4]
  sub  eax, dword ptr CPUUTIL[edi].qwIdle[0]
  sbb  edx, dword ptr CPUUTIL[edi].qwIdle[4]
  mov  dword ptr VAR.qwSum[0], eax
  mov  dword ptr VAR.qwSum[4], edx
  add  dword ptr VAR.qwIdleOverall[0], eax
  adc  dword ptr VAR.qwIdleOverall[4], edx
  mov  dword ptr VAR.qwIdle[0], eax
  mov  dword ptr VAR.qwIdle[4], edx
  mov  eax, dword ptr CPUUTIL[esi].qwIdle[0]
  mov  edx, dword ptr CPUUTIL[esi].qwIdle[4]
  mov  dword ptr CPUUTIL[edi].qwIdle[0], eax
  mov  dword ptr CPUUTIL[edi].qwIdle[4], edx

  mov  eax, dword ptr CPUUTIL[esi].qwBusy[0]
  mov  edx, dword ptr CPUUTIL[esi].qwBusy[4]
  sub  eax, dword ptr CPUUTIL[edi].qwBusy[0]
  sbb  edx, dword ptr CPUUTIL[edi].qwBusy[4]
  add  dword ptr VAR.qwSum[0], eax
  adc  dword ptr VAR.qwSum[4], edx
  add  dword ptr VAR.qwBusyOverall[0], eax
  adc  dword ptr VAR.qwBusyOverall[4], edx
  mov  dword ptr VAR.qwBusy[0], eax
  mov  dword ptr VAR.qwBusy[4], edx
  mov  eax, dword ptr CPUUTIL[esi].qwBusy[0]
  mov  edx, dword ptr CPUUTIL[esi].qwBusy[4]
  mov  dword ptr CPUUTIL[edi].qwBusy[0], eax
  mov  dword ptr CPUUTIL[edi].qwBusy[4], edx

  mov  eax, dword ptr CPUUTIL[esi].qwIntr[0]
  mov  edx, dword ptr CPUUTIL[esi].qwIntr[4]
  sub  eax, dword ptr CPUUTIL[edi].qwIntr[0]
  sbb  edx, dword ptr CPUUTIL[edi].qwIntr[4]
  jns  short BLBL1
  xor  eax, eax
  xor  edx, edx
BLBL1:
  add  dword ptr VAR.qwSum[0], eax
  adc  dword ptr VAR.qwSum[4], edx
  add  dword ptr VAR.qwIntrOverall[0], eax
  adc  dword ptr VAR.qwIntrOverall[4], edx
  mov  dword ptr VAR.qwIntr[0], eax
  mov  dword ptr VAR.qwIntr[4], edx
  mov  eax, dword ptr CPUUTIL[esi].qwIntr[0]
  mov  edx, dword ptr CPUUTIL[esi].qwIntr[4]
  mov  dword ptr CPUUTIL[edi].qwIntr[0], eax
  mov  dword ptr CPUUTIL[edi].qwIntr[4], edx

  mov  eax, dword ptr VAR.qwSum[0]
  or  eax, dword ptr VAR.qwSum[4]
  jz  short BLBL2
  mov  eax, 6553600
  fild  VAR.qwIdle                             ; st(0):idle  st(1):1/6553600
  fild  VAR.qwSum                              ; st(0):sum  st(1):idle  st(2):1/6553600
  fild  VAR.qwIntr                             ; st(0):intr  st(1):sum  st(2):idle  st(3):1/6553600
  fxch  st(1)                                  ; st(0):sum  st(1):intr  st(2):idle  st(3):1/6553600
  fmul  st(0), st(3)                           ; st(0):sum/6553600  st(1):intr  st(2):idle  st(3):1/6553600
  fxch  st(2)                                  ; st(0):idle  st(1):intr  st(2):sum/6553600  st(3):1/6553600
  fdiv  st(0), st(2)                           ; st(0):6553600*idle/sum  st(1):intr  st(2):sum/6553600  st(3):1/6553600
  fistp  PERFORMANCEsub[ebx].fxIdleRatio       ; st(0):intr  st(1):sum/6553600  st(2):1/6553600
  fdiv  st(0), st(1)                           ; st(0):6553600*intr/sum  st(1):sum/6553600  st(2):1/6553600
  fistp  PERFORMANCEsub[ebx].fxInterruptRatio  ; st(0):sum/6553600  st(1):1/6553600
  fstp  st(0)                                  ; st(0):1/6553600
  sub  eax, PERFORMANCEsub[ebx].fxIdleRatio
  sub  eax, PERFORMANCEsub[ebx].fxInterruptRatio
  jmp  short BLBL3
BLBL2:
  mov  PERFORMANCEsub[ebx].fxIdleRatio, eax
  mov  PERFORMANCEsub[ebx].fxInterruptRatio, eax
BLBL3:
  mov  PERFORMANCEsub[ebx].fxBusyRatio, eax

  mov  eax, ARG.ulNumCPUs
  add  ebx, size PERFORMANCEsub
  add  esi, size CPUUTIL
  add  edi, size CPUUTIL
  dec  eax
  mov  ARG.ulNumCPUs, eax
  jnz  near ptr BLBL0

  mov  eax, dword ptr VAR.qwIdleOverall[0]
  mov  edx, dword ptr VAR.qwIdleOverall[4]
  add  eax, dword ptr VAR.qwBusyOverall[0]
  adc  edx, dword ptr VAR.qwBusyOverall[4]
  add  eax, dword ptr VAR.qwIntrOverall[0]
  adc  edx, dword ptr VAR.qwIntrOverall[4]
  mov  dword ptr VAR.qwSum[0], eax
  mov  dword ptr VAR.qwSum[4], edx

  mov  eax, 6553600
  mov  ebx, ARG.pxPerfInfo
  fild  VAR.qwIdleOverall                           ; st(0):idle  st(1):1/6553600
  fild  VAR.qwIntrOverall                           ; st(0):intr  st(1):idle  st(2):1/6553600
  fild  VAR.qwSum                                   ; st(0):sum  st(1):intr  st(2):idle  st(3):1/6553600
  fmul  st(0), st(3)                                ; st(0):sum/6553600  st(1):intr  st(2):idle  st(3):1/6553600
  fxch  st(2)                                       ; st(0):idle  st(1):intr  st(2):sum/6553600  st(3):1/6553600
  fdiv  st(0), st(2)                                ; st(0):6553600*idle/sum  st(1):intr  st(2):sum/6553600  st(3):1/6553600
  fistp  PERFORMANCE[ebx].Overall.fxIdleRatio       ; st(0):intr  st(1):sum/6553600  st(2):1/6553600
  fdiv  st(0), st(1)                                ; st(0):6553600*intr/sum  st(1):sum/6553600  st(2):1/6553600
  fistp  PERFORMANCE[ebx].Overall.fxInterruptRatio  ; st(0):sum/6553600  st(1):1/6553600
  fcompp
  sub  eax, PERFORMANCE[ebx].Overall.fxIdleRatio
  sub  eax, PERFORMANCE[ebx].Overall.fxInterruptRatio
  mov  PERFORMANCE[ebx].Overall.fxBusyRatio, eax

  pop  edi
  pop  esi
  pop  ebx
  leave
  ret
SubDQP  endp

CODE32  ends

  end  os2entry

