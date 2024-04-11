/***************************************************************************/
/*                                                                         */
/*  ftsystem.c                                                             */
/*                                                                         */
/*    ANSI-specific FreeType low-level system interface (body).            */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2006, 2008, 2009, 2010 by                   */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* This file contains the default interface used by FreeType to access   */
  /* low-level, i.e. memory management, i/o access as well as thread       */
  /* synchronisation.  It can be replaced by user-specific routines if     */
  /* necessary.                                                            */
  /*                                                                       */
  /*************************************************************************/

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)

#ifdef BASE_PLATFORM_WINDOWS
	#define _CRT_SECURE_NO_DEPRECATE // don't care about secure c functions
	#define _CRT_NONSTDC_NO_DEPRECATE // don't complain about non std c
	#define _CRT_SECURE_NO_WARNINGS // no warnings

	//#define snprintf _snprintf
#endif // BASE_PLATFORM_WINDOWS

#include <stdio.h>

#ifdef BASE_PLATFORM_ANDROID
	#include <android/log.h>

	#define  LOG_TAG    "BASE"
	#define  LOGI(...)  __android_log_print( ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__ )
	//#define  LOGE(...)  __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )
#endif // BASE_PLATFORM_ANDROID

// zzip support
#include <zzip/zzip.h>

#include <ft2build.h>
#include FT_CONFIG_CONFIG_H
//#include <freetype/internal/ftdebug.h>
//#include <freetype/internal/ftstream.h>
#include FT_SYSTEM_H
#include FT_TYPES_H
#include FT_ERRORS_H

#include "Render/FreetypeFontZZip.h"
#include "Render/FreetypeZZipAccess.h"

static zzip_strings_t zip_ext[] = { ".zip", ".ZIP", "", 0 };
static char NewFileName[256+256];
static int o_flags = O_RDONLY;	// rb
static int o_modes = 0664;

  /*************************************************************************/
  /*                                                                       */
  /*                       MEMORY MANAGEMENT INTERFACE                     */
  /*                                                                       */
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* It is not necessary to do any error checking for the                  */
  /* allocation-related functions.  This will be done by the higher level  */
  /* routines like ft_mem_alloc() or ft_mem_realloc().                     */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_alloc                                                           */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The memory allocation function.                                    */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory :: A pointer to the memory object.                          */
  /*                                                                       */
  /*    size   :: The requested size in bytes.                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The address of newly allocated block.                              */
  /*                                                                       */
FT_CALLBACK_DEF( void* )
ft_alloc( FT_Memory  memory, long       size );

FT_CALLBACK_DEF( void* )
ft_alloc( FT_Memory  memory,
            long       size )
{
	FT_UNUSED( memory );

	return ft_smalloc( size );
}


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_realloc                                                         */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The memory reallocation function.                                  */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory   :: A pointer to the memory object.                        */
  /*                                                                       */
  /*    cur_size :: The current size of the allocated memory block.        */
  /*                                                                       */
  /*    new_size :: The newly requested size in bytes.                     */
  /*                                                                       */
  /*    block    :: The current address of the block in memory.            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The address of the reallocated memory block.                       */
  /*                                                                       */
FT_CALLBACK_DEF( void* )
ft_realloc( FT_Memory  memory, long       cur_size, long       new_size, void*      block );

FT_CALLBACK_DEF( void* )
ft_realloc( FT_Memory  memory,
              long       cur_size,
              long       new_size,
              void*      block )
{
	FT_UNUSED( memory );
	FT_UNUSED( cur_size );

	return ft_srealloc( block, new_size );
}


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_free                                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The memory release function.                                       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory  :: A pointer to the memory object.                         */
  /*                                                                       */
  /*    block   :: The address of block in memory to be freed.             */
  /*                                                                       */
FT_CALLBACK_DEF( void )
ft_free( FT_Memory  memory, void*      block );

FT_CALLBACK_DEF( void )
ft_free( FT_Memory  memory, void*      block )
{
	FT_UNUSED( memory );

	ft_sfree( block );
}


  /*************************************************************************/
  /*                                                                       */
  /*                     RESOURCE MANAGEMENT INTERFACE                     */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_io

  /* We use the macro STREAM_FILE for convenience to extract the       */
  /* system-specific stream handle from a given FreeType stream object */
#define STREAM_FILE( stream )  ( (ZZIP_FILE*)stream->descriptor.pointer )


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_ansi_stream_close                                               */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The function to close a stream.                                    */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A pointer to the stream object.                          */
  /*                                                                       */
FT_CALLBACK_DEF( void )
ft_ansi_stream_close( FT_Stream  stream );

FT_CALLBACK_DEF( void )
ft_ansi_stream_close( FT_Stream  stream )
{
	zzip_fclose( STREAM_FILE( stream ) );

	stream->descriptor.pointer = 0;
	stream->size               = 0;
	stream->base               = 0;
}


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_ansi_stream_io                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The function to open a stream.                                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A pointer to the stream object.                          */
  /*                                                                       */
  /*    offset :: The position in the data stream to start reading.        */
  /*                                                                       */
  /*    buffer :: The address of buffer to store the read data.            */
  /*                                                                       */
  /*    count  :: The number of bytes to read from the stream.             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The number of bytes actually read.  If `count' is zero (this is,   */
  /*    the function is used for seeking), a non-zero return value         */
  /*    indicates an error.                                                */
  /*                                                                       */

FT_CALLBACK_DEF( unsigned long )
ft_ansi_stream_io( FT_Stream       stream, unsigned long   offset, unsigned char*  buffer, unsigned long   count );

FT_CALLBACK_DEF( unsigned long )
ft_ansi_stream_io( FT_Stream       stream,
                     unsigned long   offset,
                     unsigned char*  buffer,
                     unsigned long   count )
{
	ZZIP_FILE*  file;

	if ( !count && offset > stream->size )
		return 1;

	file = STREAM_FILE( stream );

	if ( stream->pos != offset )
		zzip_seek( file, offset, SEEK_SET );

	if ( buffer == 0 &&
		count == 0)
	{
		zzip_seek( file, offset, SEEK_SET );
		return 0;
	}

	return (unsigned long)zzip_fread( buffer, 1, count, file );
}


  /* documentation is in ftstream.h */

FT_BASE_DEF( FT_Error )
FT_Stream_Open( FT_Stream    stream,
                  const char*  filepathname )
{
	ZZIP_FILE*  file;

	if ( !stream )
		return FT_Err_Invalid_Stream_Handle;

	//printf(  "FT_Stream_Open: opening %s\n", filepathname );

	snprintf( NewFileName, 256+256, "%s", filepathname );

	file = zzip_fopen( NewFileName, "rb" );
	
	if ( !file )
	{
		if( GetFreeTypeZipFile() != 0 )
			snprintf( NewFileName, 256+256, "%s/%s", GetFreeTypeZipFile(), filepathname );
		else
			snprintf( NewFileName, 256+256, "%s", filepathname );

		// try and load from the zip
		file = zzip_open_shared_io( 0, NewFileName, o_flags, o_modes, zip_ext, 0 );

		if ( !file )
		{
			//FT_ERROR(( "FT_Stream_Open:"
			//           " could not open `%s'\n", filepathname ));

			//printf(  "FT_Stream_Open: could not open %s\n", NewFileName );
	
			return FT_Err_Cannot_Open_Resource;
		}
	}

	zzip_seek( file, 0, SEEK_END );
	stream->size = zzip_tell( file );
	zzip_seek( file, 0, SEEK_SET );

	stream->descriptor.pointer = file;
	stream->pathname.pointer   = (char*)NewFileName;
	stream->pos                = 0;

	stream->read  = ft_ansi_stream_io;
	stream->close = ft_ansi_stream_close;

	//FT_TRACE1(( "FT_Stream_Open:" ));
	//FT_TRACE1(( " opened `%s' (%d bytes) successfully\n",
	//           filepathname, stream->size ));
	//printf(  "FT_Stream_Open: opened %s\n", NewFileName );

	return FT_Err_Ok;
}


#ifdef FT_DEBUG_MEMORY
	extern FT_Int
	ft_mem_debug_init( FT_Memory  memory );

	extern void
	ft_mem_debug_done( FT_Memory  memory );
#endif // FT_DEBUG_MEMORY


/* documentation is in ftobjs.h */

FT_BASE_DEF( FT_Memory )
FT_New_Memory( void )
{
	FT_Memory  memory;

	memory = (FT_Memory)ft_smalloc( sizeof ( *memory ) );
	if ( memory )
	{
		memory->user    = 0;
		memory->alloc   = ft_alloc;
		memory->realloc = ft_realloc;
		memory->free    = ft_free;
#ifdef FT_DEBUG_MEMORY
		ft_mem_debug_init( memory );
#endif // FT_DEBUG_MEMORY
	}

	return memory;
}


/* documentation is in ftobjs.h */

FT_BASE_DEF( void )
FT_Done_Memory( FT_Memory  memory )
{
#ifdef FT_DEBUG_MEMORY
	ft_mem_debug_done( memory );
#endif // FT_DEBUG_MEMORY
	ft_sfree( memory );
}

/* END */

#endif // BASE_SUPPORT_FREETYPE