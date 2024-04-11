
#ifndef __IOSINCLUDES_H__
#define __IOSINCLUDES_H__

#ifdef BASE_PLATFORM_iOS

#ifndef _SYS_TIME_H_
	#include <sys/time.h>
#endif // _SYS_TIME_H_

#ifndef _SYS_TYPES_H_
    #include <sys/types.h>
#endif // _SYS_TYPES_H_

#ifndef _SYS_SYSCTL_H_
    #include <sys/sysctl.h>
#endif // _SYS_SYSCTL_H_

#ifndef _UNISTD_H_
    #include <unistd.h>
#endif // _UNISTD_H_

#ifndef _USER_SIGNAL_H
    #include <signal.h>
#endif // _USER_SIGNAL_H

#ifndef _TIME_H_
	#include <time.h>
#endif // _TIME_H_

#endif // BASE_PLATFORM_iOS

#endif // __IOSINCLUDES_H__