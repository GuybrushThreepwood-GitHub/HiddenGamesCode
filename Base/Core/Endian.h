
#ifndef __ENDIAN_H__
#define __ENDIAN_H__

namespace core
{
	enum EMachineEndian
	{
		MACHINE_LITTLE_ENDIAN = 0,
		MACHINE_BIG_ENDIAN = 1
	};

	/// EndianCheck - Checks what endian the current machine is and fills the global eMachineEndian variable
	void EndianCheck( void );
	/// LittleToBigEndianShort - Converts a little endian ushort to a big endian ushort
	/// \param littleEndian - ushort to convert
	/// \return unsigned short - converted ushort
	unsigned short LittleToBigEndianShort( unsigned short littleEndian );
	/// LittleToBigEndianInt - Converts a little endian uint to a big endian uint
	/// \param littleEndian - uint to convert
	/// \return unsigned int - converted uint
	unsigned int LittleToBigEndianInt( unsigned int littleEndian );
	/// LittleToBigEndianFloat - Converts a little endian float to a big endian float
	/// \param littleEndian - float to convert
	/// \return float - converted float
	float LittleToBigEndianFloat( float littleEndian );

	/// EndianSwapShort - Converts a unsigned short to specified endian in the 2nd param
	/// \param nEndianValue - unsigned short to convert
	/// \param eEndian - passes which endian the first parameter should be converted to
	/// \return unsigned short - converted ushort
	unsigned short EndianSwapShort( unsigned short nEndianValue, EMachineEndian eEndian );
	/// EndianSwapInt - Converts a little endian uint to a big endian uint
	/// \param nEndianValue - unsigned int to convert
	/// \param eEndian - passes which endian the first parameter should be converted to
	/// \return unsigned int - converted uint
	unsigned int EndianSwapInt( unsigned int nEndianValue, EMachineEndian eEndian );
	/// EndianSwapFloat - Converts a little endian float to a big endian float
	/// \param fEndianValue - float to convert
	/// \param eEndian - passes which endian the first parameter should be converted to
	/// \return float - converted float
	float EndianSwapFloat( float fEndianValue, EMachineEndian eEndian );

	/// Global machine endian lookup
	extern EMachineEndian eMachineEndian;

} // namespace core

#endif // __ENDIAN_H__

