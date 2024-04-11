
#ifndef __ANDROIDINCLUDES_H__
#define __ANDROIDINCLUDES_H__

#ifdef BASE_PLATFORM_ANDROID

#ifndef _SYS_TIME_H_
	#include <sys/time.h>
#endif // _SYS_TIME_H_

#ifndef _TIME_H_
	#include <time.h>
#endif // _TIME_H_

#ifndef _JNI_H
	#include <jni.h>
#endif // _JNI_H

#ifndef __JNICORE_H__
	#include "Core/aos/jniCore.h"
#endif // __JNICORE_H__

#endif // BASE_PLATFORM_ANDROID

#endif // __ANDROIDINCLUDES_H__