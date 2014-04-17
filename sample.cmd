/* RXPERFDL.DLL, REXX External Function Interface sample */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs
call RxFuncAdd "RxQpLoadFuncs", "RXPERFDL", "RxQpLoadFuncs"
call RxQpLoadFuncs

say "RXPERFDL.DLL, REXX External Function Interface sample"
say

if RxQueryPerformance("stem") = 0 then do
 say "type  percent min------------------------------------------------------max"
 do forever
  call SysSleep 1
  call RxQueryPerformance "stem"
  say "idle" format(stem.Overall.fxIdleRatio/65536, 3, 3)"%" overlay('', copies(' ', 60), , trunc(60*stem.Overall.fxIdleRatio/6553600+0.5), '#')
  say "busy" format(stem.Overall.fxBusyRatio/65536, 3, 3)"%" overlay('', copies(' ', 60), , trunc(60*stem.Overall.fxBusyRatio/6553600+0.5), '#')
  say "inOS" format(stem.Overall.fxInterruptRatio/65536, 3, 3)"%" overlay('', copies(' ', 60), , trunc(60*stem.Overall.fxInterruptRatio/6553600+0.5), '#')
  call charout , "1b"x || "[3A"
 end
end
else
 say "error: This system does not support RxQueryPerformance()."

exit

