
#ifndef __JNICORE_H__
#define __JNICORE_H__

#ifdef BASE_PLATFORM_ANDROID

namespace core
{
	namespace aos
	{
		/// SetVM - sets the active java virtual machine
		/// \param vm - virtual machine pointer
		void SetVM(JavaVM* vm);
		/// GetVM - gets the active java virtual machine
		/// \returns JavaVM* - pointer to the active machine
		JavaVM* GetVM();	

	} // namespace aos

} // namespace core

#endif // BASE_PLATFORM_ANDROID


#endif // __JNICORE_H__

