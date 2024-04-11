
#ifndef __RASPBERRYPITHREAD_H__
#define __RASPBERRYPITHREAD_H__

#ifdef BASE_PLATFORM_RASPBERRYPI

namespace core
{
	namespace rpi
	{
		class RaspberryPiThread
		{
			public:
				/// default constructor
				RaspberryPiThread();
				/// default destructor
				~RaspberryPiThread();

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

	} // namespace rpi

} // namespace core

#endif // BASE_PLATFORM_RASPBERRYPI


#endif // __RASPBERRYPITHREAD_H__

