/*
 * File functions
 *
 * Copyright (C) 2011-2014, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libscca_codepage.h"
#include "libscca_debug.h"
#include "libscca_definitions.h"
#include "libscca_io_handle.h"
#include "libscca_file.h"
#include "libscca_file_information.h"
#include "libscca_libbfio.h"
#include "libscca_libcdata.h"
#include "libscca_libcerror.h"
#include "libscca_libcnotify.h"
#include "libscca_libcstring.h"
#include "libscca_libfvalue.h"
#include "libscca_volume_information.h"

/* Creates a file
 * Make sure the value file is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libscca_file_initialize(
     libscca_file_t **file,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_initialize";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( *file != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file value already set.",
		 function );

		return( -1 );
	}
	internal_file = memory_allocate_structure(
	                 libscca_internal_file_t );

	if( internal_file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create file.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_file,
	     0,
	     sizeof( libscca_internal_file_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file.",
		 function );

		memory_free(
		 internal_file );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &( internal_file->volumes_array ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create volumes array.",
		 function );

		goto on_error;
	}
	if( libscca_io_handle_initialize(
	     &( internal_file->io_handle ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create IO handle.",
		 function );

		goto on_error;
	}
	*file = (libscca_file_t *) internal_file;

	return( 1 );

on_error:
	if( internal_file != NULL )
	{
		if( internal_file->volumes_array != NULL )
		{
			libcdata_array_free(
			 &( internal_file->volumes_array ),
			 NULL,
			 NULL );
		}
		memory_free(
		 internal_file );
	}
	return( -1 );
}

/* Frees a file
 * Returns 1 if successful or -1 on error
 */
int libscca_file_free(
     libscca_file_t **file,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_free";
	int result                             = 1;

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( *file != NULL )
	{
		internal_file = (libscca_internal_file_t *) *file;

		if( internal_file->file_io_handle != NULL )
		{
			if( libscca_file_close(
			     *file,
			     error ) != 0 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_CLOSE_FAILED,
				 "%s: unable to close file.",
				 function );

				result = -1;
			}
		}
		*file = NULL;

		if( libcdata_array_free(
		     &( internal_file->volumes_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libscca_internal_volume_information_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free volumes array.",
			 function );

			result = -1;
		}
		if( libscca_io_handle_free(
		     &( internal_file->io_handle ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free IO handle.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_file );
	}
	return( result );
}

/* Signals the file to abort its current activity
 * Returns 1 if successful or -1 on error
 */
int libscca_file_signal_abort(
     libscca_file_t *file,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_signal_abort";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file - missing IO handle.",
		 function );

		return( -1 );
	}
	internal_file->io_handle->abort = 1;

	return( 1 );
}

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
int libscca_file_open(
     libscca_file_t *file,
     const char *filename,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *file_io_handle       = NULL;
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_open";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBSCCA_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	if( libbfio_file_initialize(
	     &file_io_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file IO handle.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libbfio_handle_set_track_offsets_read(
	     file_io_handle,
	     1,
	     error ) != 1 )
	{
                libcerror_error_set(
                 error,
                 LIBCERROR_ERROR_DOMAIN_RUNTIME,
                 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set track offsets read in file IO handle.",
                 function );

		goto on_error;
	}
#endif
	if( libbfio_file_set_name(
	     file_io_handle,
	     filename,
	     libcstring_narrow_string_length(
	      filename ) + 1,
	     error ) != 1 )
	{
                libcerror_error_set(
                 error,
                 LIBCERROR_ERROR_DOMAIN_RUNTIME,
                 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set filename in file IO handle.",
                 function );

		goto on_error;
	}
	if( libscca_file_open_file_io_handle(
	     file,
	     file_io_handle,
	     access_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file: %s.",
		 function,
		 filename );

		goto on_error;
	}
	internal_file->file_io_handle_created_in_library = 1;

	return( 1 );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( -1 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
int libscca_file_open_wide(
     libscca_file_t *file,
     const wchar_t *filename,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *file_io_handle       = NULL;
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_open_wide";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBSCCA_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	if( libbfio_file_initialize(
	     &file_io_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file IO handle.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libbfio_handle_set_track_offsets_read(
	     file_io_handle,
	     1,
	     error ) != 1 )
	{
                libcerror_error_set(
                 error,
                 LIBCERROR_ERROR_DOMAIN_RUNTIME,
                 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set track offsets read in file IO handle.",
                 function );

		goto on_error;
	}
#endif
	if( libbfio_file_set_name_wide(
	     file_io_handle,
	     filename,
	     libcstring_wide_string_length(
	      filename ) + 1,
	     error ) != 1 )
	{
                libcerror_error_set(
                 error,
                 LIBCERROR_ERROR_DOMAIN_RUNTIME,
                 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set filename in file IO handle.",
                 function );

		goto on_error;
	}
	if( libscca_file_open_file_io_handle(
	     file,
	     file_io_handle,
	     access_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file: %ls.",
		 function,
		 filename );

		goto on_error;
	}
	internal_file->file_io_handle_created_in_library = 1;

	return( 1 );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( -1 );
}

#endif

/* Opens a file using a Basic File IO (bfio) handle
 * Returns 1 if successful or -1 on error
 */
int libscca_file_open_file_io_handle(
     libscca_file_t *file,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_open_file_io_handle";
	int bfio_access_flags                  = 0;
	int file_io_handle_is_open             = 0;

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->file_io_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file - file IO handle already set.",
		 function );

		return( -1 );
	}
	if( file_io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBSCCA_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBSCCA_ACCESS_FLAG_WRITE ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBSCCA_ACCESS_FLAG_READ ) != 0 )
	{
		bfio_access_flags = LIBBFIO_ACCESS_FLAG_READ;
	}
	internal_file->file_io_handle = file_io_handle;

	file_io_handle_is_open = libbfio_handle_is_open(
	                          internal_file->file_io_handle,
	                          error );

	if( file_io_handle_is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to determine if file IO handle is open.",
		 function );

		goto on_error;
	}
	else if( file_io_handle_is_open == 0 )
	{
		if( libbfio_handle_open(
		     internal_file->file_io_handle,
		     bfio_access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open file IO handle.",
			 function );

			goto on_error;
		}
		internal_file->file_io_handle_opened_in_library = 1;
	}
	if( libscca_file_open_read(
	     internal_file,
	     file_io_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read from file handle.",
		 function );

		goto on_error;
	}
	internal_file->file_io_handle = file_io_handle;

	return( 1 );

on_error:
	if( ( file_io_handle_is_open == 0 )
	 && ( internal_file->file_io_handle_opened_in_library != 0 ) )
	{
		libbfio_handle_close(
		 file_io_handle,
		 error );

		internal_file->file_io_handle_opened_in_library = 0;
	}
	internal_file->file_io_handle = NULL;

	return( -1 );
}

/* Closes a file
 * Returns 0 if successful or -1 on error
 */
int libscca_file_close(
     libscca_file_t *file,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_close";
	int result                             = 0;

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->file_io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file - missing file IO handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( internal_file->file_io_handle_created_in_library != 0 )
		{
			if( libscca_debug_print_read_offsets(
			     internal_file->file_io_handle,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
				 "%s: unable to print the read offsets.",
				 function );

				result = -1;
			}
		}
	}
#endif
	if( internal_file->file_io_handle_opened_in_library != 0 )
	{
		if( libbfio_handle_close(
		     internal_file->file_io_handle,
		     error ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_CLOSE_FAILED,
			 "%s: unable to close file IO handle.",
			 function );

			result = -1;
		}
		internal_file->file_io_handle_opened_in_library = 0;
	}
	if( internal_file->file_io_handle_created_in_library != 0 )
	{
		if( libbfio_handle_free(
		     &( internal_file->file_io_handle ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free file IO handle.",
			 function );

			result = -1;
		}
		internal_file->file_io_handle_created_in_library = 0;
	}
	internal_file->file_io_handle = NULL;

	if( libscca_io_handle_clear(
	     internal_file->io_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		result = -1;
	}
	internal_file->prefetch_hash = 0; 

	if( internal_file->file_information != NULL )
	{
		if( libscca_file_information_free(
		     &( internal_file->file_information ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free file information.",
			 function );

			result = -1;
		}
	}
	if( internal_file->filename_strings != NULL )
	{
		if( libfvalue_value_free(
		     &( internal_file->filename_strings ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free filename strings.",
			 function );

			result = -1;
		}
	}
	if( libcdata_array_resize(
	     internal_file->volumes_array,
	     0,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libscca_internal_volume_information_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize volumes array.",
		 function );

		result = -1;
	}
	return( result );
}

/* Opens a file for reading
 * Returns 1 if successful or -1 on error
 */
int libscca_file_open_read(
     libscca_internal_file_t *internal_file,
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error )
{
	static char *function = "libscca_file_open_read";

	if( internal_file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( internal_file->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( internal_file->file_information != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file - file information value already set.",
		 function );

		return( -1 );
	}
	if( internal_file->filename_strings != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file - filename strings value already set.",
		 function );

		return( -1 );
	}
	if( internal_file->io_handle->abort != 0 )
	{
		internal_file->io_handle->abort = 0;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "Reading file header:\n" );
	}
#endif
	if( libscca_io_handle_read_file_header(
	     internal_file->io_handle,
	     file_io_handle,
	     &( internal_file->prefetch_hash ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read file header.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "Reading file information:\n" );
	}
#endif
	if( libscca_file_information_initialize(
	     &( internal_file->file_information ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file information.",
		 function );

		goto on_error;
	}
	if( libscca_file_information_read(
	     internal_file->file_information,
	     file_io_handle,
	     internal_file->io_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read file information.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( internal_file->file_information->metrics_array_offset != 0 )
		{
/* TODO check bounds metrics_array_offset < file_header, metrics_array_offset > trace_chain_array_offset */
			if( libscca_io_handle_read_metrics_array(
			     internal_file->io_handle,
			     file_io_handle,
			     internal_file->file_information->metrics_array_offset,
			     internal_file->file_information->number_of_metrics_entries,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read x array.",
				 function );

				goto on_error;
			}
		}
		if( internal_file->file_information->trace_chain_array_offset != 0 )
		{
/* TODO check bounds trace_chain_array_offset < file_header, trace_chain_array_offset > file_size */
			if( libscca_io_handle_read_trace_chain_array(
			     internal_file->io_handle,
			     file_io_handle,
			     internal_file->file_information->trace_chain_array_offset,
			     internal_file->file_information->number_of_trace_chain_array_entries,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read y array.",
				 function );

				goto on_error;
			}
		}
	}
#endif
/* TODO move the following section to a separate function */
	if( internal_file->file_information->filename_strings_offset != 0 )
	{
/* TODO check bounds filename_strings_offset < file_header, filename_strings_offset > file_size */
		if( libfvalue_value_type_initialize(
		     &( internal_file->filename_strings ),
		     LIBFVALUE_VALUE_TYPE_STRING_UTF16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create filename strings value.",
			 function );

			goto on_error;
		}
		if( libscca_io_handle_read_filename_strings(
		     internal_file->io_handle,
		     file_io_handle,
		     internal_file->file_information->filename_strings_offset,
		     internal_file->file_information->filename_strings_size,
		     internal_file->filename_strings,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read filename strings.",
			 function );

			goto on_error;
		}
	}
	if( internal_file->file_information->volumes_information_offset != 0 )
	{
		if( libscca_io_handle_read_volumes_information(
		     internal_file->io_handle,
		     internal_file->file_io_handle,
		     internal_file->file_information->volumes_information_offset,
		     internal_file->file_information->volumes_information_size,
		     internal_file->file_information->number_of_volumes,
		     internal_file->volumes_array,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read volumes information.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( internal_file->filename_strings != NULL )
	{
		libfvalue_value_free(
		 &( internal_file->filename_strings ),
		 NULL );
	}
	if( internal_file->file_information != NULL )
	{
		libscca_file_information_free(
		 &( internal_file->file_information ),
		 NULL );
	}
	libcdata_array_resize(
	 internal_file->volumes_array,
	 0,
	 (int (*)(intptr_t **, libcerror_error_t **)) &libscca_internal_volume_information_free,
	 NULL );

	return( -1 );
}

/* Retrieves the format version
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_format_version(
     libscca_file_t *file,
     uint32_t *format_version,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_format_version";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( format_version == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid format version.",
		 function );

		return( -1 );
	}
	*format_version = internal_file->io_handle->format_version;

	return( 1 );
}

/* Retrieves the prefetch hash
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_prefetch_hash(
     libscca_file_t *file,
     uint32_t *prefetch_hash,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_prefetch_hash";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( prefetch_hash == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid prefetch hash.",
		 function );

		return( -1 );
	}
	*prefetch_hash = internal_file->prefetch_hash;

	return( 1 );
}

/* Retrieves the number of filenames
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_number_of_filenames(
     libscca_file_t *file,
     int *number_of_filenames,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_number_of_filenames";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libfvalue_value_get_number_of_value_entries(
	     internal_file->filename_strings,
	     number_of_filenames,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of filename strings.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of a specific UTF-8 encoded filename
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_utf8_filename_size(
     libscca_file_t *file,
     int filename_index,
     size_t *utf8_string_size,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_utf8_filename_size";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libfvalue_value_get_utf8_string_size(
	     internal_file->filename_strings,
	     filename_index,
	     utf8_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve filename: %d UTF-8 string size.",
		 function,
		 filename_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific UTF-8 encoded filename
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_utf8_filename(
     libscca_file_t *file,
     int filename_index,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_utf8_filename";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libfvalue_value_copy_to_utf8_string(
	     internal_file->filename_strings,
	     filename_index,
	     utf8_string,
	     utf8_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy filename: %d to UTF-8 string.",
		 function,
		 filename_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of a specific UTF-16 encoded filename
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_utf16_filename_size(
     libscca_file_t *file,
     int filename_index,
     size_t *utf16_string_size,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_utf16_filename_size";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libfvalue_value_get_utf16_string_size(
	     internal_file->filename_strings,
	     filename_index,
	     utf16_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve filename: %d UTF-16 string size.",
		 function,
		 filename_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific UTF-16 encoded filename
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_utf16_filename(
     libscca_file_t *file,
     int filename_index,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_utf16_filename";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libfvalue_value_copy_to_utf16_string(
	     internal_file->filename_strings,
	     filename_index,
	     utf16_string,
	     utf16_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy filename: %d to UTF-16 string.",
		 function,
		 filename_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of volumes
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_number_of_volumes(
     libscca_file_t *file,
     int *number_of_volumes,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_number_of_volumes";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( libcdata_array_get_number_of_entries(
	     internal_file->volumes_array,
	     number_of_volumes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of volumes.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific volume information
 * Returns 1 if successful or -1 on error
 */
int libscca_file_get_volume_information(
     libscca_file_t *file,
     int volume_index,
     libscca_volume_information_t **volume_information,
     libcerror_error_t **error )
{
	libscca_internal_file_t *internal_file = NULL;
	static char *function                  = "libscca_file_get_volume_information";

	if( file == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libscca_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing file information.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_file->volumes_array,
	     volume_index,
	     (intptr_t **) volume_information,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve volume: %d information.",
		 function,
		 volume_index );

		return( -1 );
	}
	return( 1 );
}

