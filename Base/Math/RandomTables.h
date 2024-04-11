
#ifndef __RANDOMTABLES_H__
#define __RANDOMTABLES_H__

namespace math
{
	const unsigned int RANDOM_TABLE_SIZE = 1024;

	/// InitRandomSeed - Initialises a random seed from the current time
	void InitRandomSeed( void );
	/// RandomNumber - Gets a random integer between two input integers
	/// \param iMin - minimum range of value
	/// \param iMax - maximum range of value
	/// \return integer - returned random integer number
	int RandomNumber(int iMin, int iMax);
	/// RandomNumber - Gets a random float between two input floats
	/// \param fMin - minimum range of value
	/// \param fMax - maximum range of value
	/// \return float - returned random float number
	float RandomNumber(float fMin, float fMax);

	/// Template based random number generator
	template <class T> class CMinMax
	{
		public:
			/// default constructor
			CMinMax() { m_Min = m_Max = 0; }
			/// constructor
			/// \param tMin - template min value range
			/// \param tMax - template max value range
			CMinMax(T tMin, T tMax) { m_Min = tMin; m_Max = tMax; }
			/// default destructor
			~CMinMax() { }

			/// stored min value of random number output
			T m_Min;
			/// stored max value of random number output
			T m_Max;

			/// GetRandomNumInRange - Gets a random number between initialised range
			/// \return T - template based returned random number
			T GetRandomNumInRange(void) { return(RandomNumber(m_Min, m_Max)); }
	};

} // namespace math

#endif // __RANDOMTABLES_H__


