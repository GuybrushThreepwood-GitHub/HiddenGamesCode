# The ARMv7 is significanly faster due to the use of the hardware FPU
#APP_ABI := all # armeabi armeabi-v7a x86 mips
APP_ABI := armeabi-v7a
APP_PLATFORM := android-9
APP_STL := stlport_static
APP_OPTIM := release