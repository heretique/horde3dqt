make HOST_CC="gcc -m32" HOST_CFLAGS="-D__symbian__"^
     CROSS=arm-none-symbianelf- TARGET=ARM TARGET_SYS=Other^
     TARGET_CFLAGS="-D__symbian__ -D__SYMBIAN32__ -D__GCC32__ -D__GCCE__ -DLUAJIT_USE_SYSMALLOC -I\Symbian\9.1\S60_3rd_MR\Epoc32\include\libc"
@if not exist Symbian mkdir Symbian
move libluajit.a Symbian
move lj_bcdef.h Symbian
move lj_ffdef.h Symbian
move lj_folddef.h Symbian
move lj_libdef.h Symbian
move lj_recdef.h Symbian
move lj_vm.s Symbian
move buildvm.exe Symbian
del *.o
