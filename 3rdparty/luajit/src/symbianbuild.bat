make HOST_CC=d:\QtSDK\mingw\bin\gcc HOST_CFLAGS="-D__symbian__" CROSS=arm-none-symbianelf- TARGET=ARM TARGET_SYS=Symbian TARGET_CFLAGS="-D__symbian__ -D__SYMBIAN32__ -D__GCC32__ -D__GCCE__ -DLUAJIT_USE_SYSMALLOC -Id:\QtSDK\Symbian\SDKs\SymbianSR1Qt474\epoc32\include -Id:\QtSDK\Symbian\SDKs\SymbianSR1Qt474\epoc32\include\libc"
REM @if not exist Symbian mkdir Symbian
REM move libluajit.a Symbian
REM move lj_bcdef.h Symbian
REM move lj_ffdef.h Symbian
REM move lj_folddef.h Symbian
REM move lj_libdef.h Symbian
REM move lj_recdef.h Symbian
REM move lj_vm.s Symbian
REM move buildvm.exe Symbian
REM del *.o
