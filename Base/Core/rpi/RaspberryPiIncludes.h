
#ifndef __RASPBERRYPIINCLUDES_H__
#define __RASPBERRYPIINCLUDES_H__

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "bcm_host.h"

#ifndef _SYS_TIME_H_
	#include <sys/time.h>
#endif // _SYS_TIME_H_

#ifndef _TIME_H_
	#include <time.h>
#endif // _TIME_H_

#ifndef _SIGNAL_H
	#include <signal.h>
#endif // _SIGNAL_H

#include <cstdlib>

#endif // BASE_PLATFORM_RASPBERRYPI

#endif // __RASPBERRYPIINCLUDES_H__