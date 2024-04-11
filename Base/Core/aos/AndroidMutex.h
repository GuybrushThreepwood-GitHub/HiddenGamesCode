
#ifndef __ANDROIDMUTEX_H__
#define __ANDROIDMUTEX_H__

#ifdef BASE_PLATFORM_ANDROID

namespace core
{
	namespace aos
	{
		class AndroidMutex
		{
			public:
				/// default constructor
				AndroidMutex();
				/// default destructor
				~AndroidMutex();

				/// Initialise - initialise member data of a mutex
				void Initialise( void );
				/// Release - cleanup any member data of a mutex
				void Release( void );
				/// Lock - lock the mutex for the current thread
				/// \return integer ( SUCCESS: OK or FAIL: FAIL )
				int Lock( void );
				/// UnLock - unlock the mutex from the thread
				/// \return integer ( SUCCESS: OK or FAIL: FAIL )
				int UnLock( void );
				/// IsLocked - Check to see if this mutex currently is locked
				/// \return boolean ( SUCCESS: true or FAIL: false )
				bool IsLocked( void )	{ return m_Locked; }

			private:
				/// mutex handle structure
				pthread_mutex_t m_hMutex;
				/// mutex attribute structure
				pthread_attr_t m_hMutexAttribs;
				/// initialisation flag
				bool m_Initialised;
				/// locked flag
				bool m_Locked;
		};

	} // namespace aos

} // namespace core

#endif // BASE_PLATFORM_ANDROID


#endif // __ANDROIDMUTEX_H__

