
/*===================================================================
	File: jniCore.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "CoreBase.h"

#include "Core/aos/jniCore.h"

namespace
{
	/// global store of VM
	JavaVM* jniVM = 0;
}

/////////////////////////////////////////////////////
/// Function: SetVM
/// Params: [in]vm
///
/////////////////////////////////////////////////////
void core::aos::SetVM(JavaVM* vm)
{
	DBGLOG( "JNICORE: SetVM()" );
	jniVM = vm;
}

/////////////////////////////////////////////////////
/// Function: GetVM
/// Params: None
///
/////////////////////////////////////////////////////
JavaVM* core::aos::GetVM()
{
	return jniVM;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif
	/////////////////////////////////////////////////////
	/// Function: JNI_OnLoad
	/// Params: [in]vm, [in]reserved
	///
	/////////////////////////////////////////////////////
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
	{
		DBGLOG( "JNICORE: JNI_OnLoad" );

		// store the VM as a global
		core::aos::SetVM( vm );

		JNIEnv* env;
		if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
			return -1;

		/* get class with (*env)->FindClass */
		/* register methods with (*env)->RegisterNatives */

		return JNI_VERSION_1_6;
	}


#ifdef  __cplusplus
}
#endif


#endif // BASE_PLATFORM_ANDROID

