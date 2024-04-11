
#ifndef __ANDROIDTHREAD_H__
#define __ANDROIDTHREAD_H__

#ifdef BASE_PLATFORM_ANDROID

namespace core
{
	namespace aos
	{
		class AndroidThread
		{
			public:
				/// default constructor
				AndroidThread();
				/// default destructor
				~AndroidThread();

				/// Initialise - initialise member data of a thread
				/// \param ThreadFunc - pointer to user callback function for thread
				/// \param pThreadParam - pointer to user thread params
				void Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam );
				/// Release - cleanup any member data of a thread
				void Release( void );

			private:
				/// thread handle structure
				pthread_t m_hThread;
				/// thread attribute structure
				pthread_attr_t m_hThreadAttribs;
				/// intialise flag
				bool m_Initialised;
		};

	} // namespace aos

} // namespace core

#endif // BASE_PLATFORM_ANDROID


#endif // __ANDROIDTHREAD_H__

