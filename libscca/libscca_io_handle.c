/*
 * Input/Output (IO) handle functions
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
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libscca_definitions.h"
#include "libscca_io_handle.h"
#include "libscca_libbfio.h"
#include "libscca_libcdata.h"
#include "libscca_libcerror.h"
#include "libscca_libcnotify.h"
#include "libscca_libfdatetime.h"
#include "libscca_libfvalue.h"
#include "libscca_libuna.h"
#include "libscca_volume_information.h"

#include "scca_file_header.h"
#include "scca_metrics_array.h"
#include "scca_volume_information.h"

const char *scca_file_signature = "SCCA";

/* Creates an IO handle
 * Make sure the value io_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_initialize(
     libscca_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libscca_io_handle_initialize";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid IO handle value already set.",
		 function );

		return( -1 );
	}
	*io_handle = memory_allocate_structure(
	              libscca_io_handle_t );

	if( *io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create IO handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *io_handle,
	     0,
	     sizeof( libscca_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *io_handle != NULL )
	{
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( -1 );
}

/* Frees a IO handle
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_free(
     libscca_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libscca_io_handle_free";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( 1 );
}

/* Clears the IO handle
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_clear(
     libscca_io_handle_t *io_handle,
     libcerror_error_t **error )
{
	static char *function = "libscca_io_handle_clear";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     io_handle,
	     0,
	     sizeof( libscca_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the file header
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_read_file_header(
     libscca_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t *prefetch_hash,
     libcerror_error_t **error )
{
	scca_file_header_t file_header;

	static char *function = "libscca_io_handle_read_file_header";
	size64_t file_size    = 0;
	ssize_t read_count    = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t value_32bit  = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( libbfio_handle_get_size(
	     file_io_handle,
	     &file_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve file size.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading file header at offset: 0 (0x00000000)\n",
		 function );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     0,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek file header offset: 0.",
		 function );

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              (uint8_t *) &file_header,
	              sizeof( scca_file_header_t ),
	              error );

	if( read_count != (ssize_t) sizeof( scca_file_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read file header data.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: file header data:\n",
		 function );
		libcnotify_print_data(
		 (uint8_t *) &file_header,
		 sizeof( scca_file_header_t ),
		 0 );
	}
#endif
	if( memory_compare(
	     file_header.signature,
	     scca_file_signature,
	     4 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid signature.",
		 function );

		return( -1 );
	}
	byte_stream_copy_to_uint32_little_endian(
	 file_header.format_version,
	 io_handle->format_version );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.file_size,
	 io_handle->file_size );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.prefetch_hash,
	 *prefetch_hash );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: format version\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->format_version );

		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c%c%c\n",
		 function,
		 file_header.signature[ 0 ],
		 file_header.signature[ 1 ],
		 file_header.signature[ 2 ],
		 file_header.signature[ 3 ] );

		byte_stream_copy_to_uint32_little_endian(
		 file_header.unknown1,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown1\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: file size\t\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->file_size );

		libcnotify_printf(
		 "%s: executable filename:\n",
		 function );
		libcnotify_print_data(
		 file_header.executable_filename,
		 60,
		 0 );

		libcnotify_printf(
		 "%s: prefetch hash\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 *prefetch_hash );

		byte_stream_copy_to_uint32_little_endian(
		 file_header.unknown2,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown2\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "\n" );
	}
#endif
	if( file_size != (size64_t) io_handle->file_size )
	{
/* TODO flag mismatch and file as corrupted? */
	}
	return( 1 );
}

/* Reads the metrics array
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_read_metrics_array(
     libscca_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t file_offset,
     uint32_t number_of_entries,
     libcerror_error_t **error )
{
	uint8_t *entry_data         = NULL;
	uint8_t *metrics_array_data = NULL;
	static char *function       = "libscca_io_handle_read_metrics_array";
	size_t entry_data_size      = 0;
	size_t read_size            = 0;
	ssize_t read_count          = 0;
	uint32_t entry_index        = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint64_t value_64bit       = 0;
	uint32_t value_32bit       = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( ( io_handle->format_version != 17 )
	 && ( io_handle->format_version != 23 )
	 && ( io_handle->format_version != 26 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid IO handle - unsupported format version.",
		 function );

		return( -1 );
	}
	if( io_handle->format_version == 17 )
	{
		entry_data_size = sizeof( scca_metrics_array_entry_v17_t );
	}
	else if( ( io_handle->format_version == 23 )
	      || ( io_handle->format_version == 26 ) )
	{
		entry_data_size = sizeof( scca_metrics_array_entry_v23_t );
	}
#if SIZEOF_SIZE_T <= 4
	if( (size_t) number_of_entries > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of values value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif
/* TODO add bounds check number of entries metrics entry_data_size */

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading metrics array at offset: %" PRIu32 " (0x%08" PRIx32 ")\n",
		 function,
		 file_offset,
		 file_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     (off64_t) file_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek metrics array offset: %" PRIu32 ".",
		 function,
		 file_offset );

		goto on_error;
	}
	read_size = number_of_entries * entry_data_size;

	metrics_array_data = (uint8_t *) memory_allocate(
	                                  sizeof( uint8_t ) * read_size );

	if( metrics_array_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create metrics array data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              metrics_array_data,
	              read_size,
	              error );

	if( read_count != (ssize_t) read_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read metrics array data.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: metrics array data:\n",
		 function );
		libcnotify_print_data(
		 metrics_array_data,
		 read_size,
		 0 );
	}
#endif
	entry_data = metrics_array_data;

	for( entry_index = 0;
	     entry_index < number_of_entries;
	     entry_index++ )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: metrics array entry: %" PRIu32 " data:\n",
			 function,
			 entry_index );
			libcnotify_print_data(
			 entry_data,
			 entry_data_size,
			 0 );
		}
#endif

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			byte_stream_copy_to_uint32_little_endian(
			 ( (scca_metrics_array_entry_v17_t *) entry_data )->start_time,
			 value_32bit );
			libcnotify_printf(
			 "%s: start time\t\t\t\t: %" PRIu32 " ms\n",
			 function,
			 value_32bit );

			byte_stream_copy_to_uint32_little_endian(
			 ( (scca_metrics_array_entry_v17_t *) entry_data )->duration,
			 value_32bit );
			libcnotify_printf(
			 "%s: duration\t\t\t\t\t: %" PRIu32 " ms\n",
			 function,
			 value_32bit );

			if( io_handle->format_version == 17 )
			{
				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v17_t *) entry_data )->filename_string_offset,
				 value_32bit );
				libcnotify_printf(
				 "%s: filename string offset\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v17_t *) entry_data )->filename_string_numbers_of_characters,
				 value_32bit );
				libcnotify_printf(
				 "%s: filename string number of characters\t: %" PRIu32 "\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v17_t *) entry_data )->flags,
				 value_32bit );
				libcnotify_printf(
				 "%s: flags\t\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );
			}
			else if( ( io_handle->format_version == 23 )
			      || ( io_handle->format_version == 26 ) )
			{
				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v23_t *) entry_data )->average_duration,
				 value_32bit );
				libcnotify_printf(
				 "%s: average duration\t\t\t\t: %" PRIu32 " ms\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v23_t *) entry_data )->filename_string_offset,
				 value_32bit );
				libcnotify_printf(
				 "%s: filename string offset\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v23_t *) entry_data )->filename_string_numbers_of_characters,
				 value_32bit );
				libcnotify_printf(
				 "%s: filename string number of characters\t: %" PRIu32 "\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_metrics_array_entry_v23_t *) entry_data )->flags,
				 value_32bit );
				libcnotify_printf(
				 "%s: flags\t\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );

				byte_stream_copy_to_uint64_little_endian(
				 ( (scca_metrics_array_entry_v23_t *) entry_data )->file_reference,
				 value_64bit );

				if( value_64bit == 0 )
				{
					libcnotify_printf(
					 "%s: file reference\t\t\t\t: %" PRIu64 "\n",
					 function,
					 value_64bit );
				}
				else
				{
					libcnotify_printf(
					 "%s: file reference\t\t\t\t: MFT entry: %" PRIu64 ", sequence: %" PRIu64 "\n",
					 function,
					 value_64bit & 0xffffffffffffUL,
					 value_64bit >> 48 );
				}
			}
			libcnotify_printf(
			 "\n" );
		}
#endif
		entry_data += entry_data_size;
	}
	memory_free(
	 metrics_array_data );

	return( 1 );

on_error:
	if( metrics_array_data != NULL )
	{
		memory_free(
		 metrics_array_data );
	}
	return( -1 );
}

/* Reads the trace chain array
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_read_trace_chain_array(
     libscca_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t file_offset,
     uint32_t number_of_entries,
     libcerror_error_t **error )
{
	uint8_t *entry_data             = NULL;
	uint8_t *trace_chain_array_data = NULL;
	static char *function           = "libscca_io_handle_read_trace_chain_array";
	size_t read_size                = 0;
	ssize_t read_count              = 0;
	uint32_t entry_index            = 0;
	uint32_t next_table_index       = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t value_32bit            = 0;
	uint16_t value_16bit            = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
#if SIZEOF_SIZE_T <= 4
	if( (size_t) number_of_entries > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of values value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif
/* TODO add bounds check number of entries y entry_data_size */

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading trace chain array at offset: %" PRIu32 " (0x%08" PRIx32 ")\n",
		 function,
		 file_offset,
		 file_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     (off64_t) file_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek trace chain array offset: %" PRIu32 ".",
		 function,
		 file_offset );

		goto on_error;
	}
/* TODO replace hardcoded size by sizeof() */
	read_size = number_of_entries * 12;

	trace_chain_array_data = (uint8_t *) memory_allocate(
	                                      sizeof( uint8_t ) * read_size );

	if( trace_chain_array_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create trace chain array data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              trace_chain_array_data,
	              read_size,
	              error );

	if( read_count != (ssize_t) read_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read trace chain array data.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: trace chain array data:\n",
		 function );
		libcnotify_print_data(
		 trace_chain_array_data,
		 read_size,
		 0 );
	}
#endif
	entry_data = trace_chain_array_data;

	for( entry_index = 0;
	     entry_index < number_of_entries;
	     entry_index++ )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: trace chain array entry: %" PRIu32 " data:\n",
			 function,
			 entry_index );
			libcnotify_print_data(
			 entry_data,
			 12,
			 0 );
		}
#endif
		byte_stream_copy_to_uint32_little_endian(
		 entry_data,
		 next_table_index );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( next_table_index == 0xffffffffUL )
			{
				libcnotify_printf(
				 "%s: next table index\t\t: 0x%08" PRIx32 "\n",
				 function,
				 next_table_index );
			}
			else
			{
				libcnotify_printf(
				 "%s: next table index\t\t: %" PRIu32 "\n",
				 function,
				 next_table_index );
			}
			byte_stream_copy_to_uint32_little_endian(
			 &( entry_data[ 4 ] ),
			 value_32bit );
			libcnotify_printf(
			 "%s: block load count\t\t: %" PRIu32 " blocks (%" PRIu64 " bytes)\n",
			 function,
			 value_32bit,
			 (uint64_t) value_32bit * 512 * 1024 );

			libcnotify_printf(
			 "%s: unknown2\t\t\t: 0x%02" PRIx8 "\n",
			 function,
			 entry_data[ 8 ] );

			libcnotify_printf(
			 "%s: unknown3\t\t\t: 0x%02" PRIx8 "\n",
			 function,
			 entry_data[ 9 ] );

			byte_stream_copy_to_uint16_little_endian(
			 &( entry_data[ 10 ] ),
			 value_16bit );
			libcnotify_printf(
			 "%s: unknown4\t\t\t: 0x%04" PRIx16 "\n",
			 function,
			 value_16bit );

			libcnotify_printf(
			 "\n" );
		}
#endif
/* TODO replace hardcoded size by sizeof() */
		entry_data += 12;
	}
	memory_free(
	 trace_chain_array_data );

	return( 1 );

on_error:
	if( trace_chain_array_data != NULL )
	{
		memory_free(
		 trace_chain_array_data );
	}
	return( -1 );
}

/* Reads the filename strings (array)
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_read_filename_strings(
     libscca_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t filename_string_offset,
     uint32_t filename_string_size,
     libfvalue_value_t *filename_strings,
     libcerror_error_t **error )
{
	uint8_t *filename_strings_data = NULL;
	static char *function          = "libscca_io_handle_read_filename_strings";
	ssize_t value_data_size        = 0;
	ssize_t read_count             = 0;

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
#if SIZEOF_SIZE_T <= 4
	if( (size_t) filename_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid filenames string size value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading filename strings at offset: %" PRIu32 " (0x%08" PRIx32 ")\n",
		 function,
		 filename_string_offset,
		 filename_string_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     (off64_t) filename_string_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek filename strings header offset: %" PRIu32 ".",
		 function,
		 filename_string_offset );

		goto on_error;
	}
	filename_strings_data = (uint8_t *) memory_allocate(
	                                     sizeof( uint8_t ) * filename_string_size );

	if( filename_strings_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create filename strings data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              filename_strings_data,
	              (size_t) filename_string_size,
	              error );

	if( read_count != (ssize_t) filename_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read filename strings data.",
		 function );

		goto on_error;
	}
	value_data_size = libfvalue_value_type_set_data_strings_array(
	                   filename_strings,
	                   filename_strings_data,
	                   (size_t) filename_string_size,
	                   LIBFVALUE_CODEPAGE_UTF16_LITTLE_ENDIAN,
	                   error );

	if( value_data_size == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data of filename strings value.",
		 function );

		goto on_error;
	}
	memory_free(
	 filename_strings_data );

	return( 1 );

on_error:
	if( filename_strings_data != NULL )
	{
		memory_free(
		 filename_strings_data );
	}
	return( -1 );
}

/* Reads the volumes information
 * Returns 1 if successful or -1 on error
 */
int libscca_io_handle_read_volumes_information(
     libscca_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t volumes_information_offset,
     uint32_t volumes_information_size,
     uint32_t number_of_volumes,
     libcdata_array_t *volumes_array,
     libcerror_error_t **error )
{
	libscca_internal_volume_information_t *volume_information = NULL;
	uint8_t *volume_information_data                          = NULL;
	uint8_t *volumes_information_data                         = NULL;
	static char *function                                     = "libscca_io_handle_read_volumes_information";
	ssize_t value_data_size                                   = 0;
	ssize_t volume_information_size                           = 0;
	ssize_t read_count                                        = 0;
	uint32_t directory_strings_array_offset                   = 0;
	uint32_t directory_strings_array_size                     = 0;
	uint32_t device_path_offset                               = 0;
	uint32_t device_path_size                                 = 0;
	uint32_t file_references_index                            = 0;
	uint32_t file_references_offset                           = 0;
	uint32_t file_references_size                             = 0;
	uint32_t number_of_directory_strings                      = 0;
	uint32_t number_of_file_references                        = 0;
	uint32_t version                                          = 0;
	uint32_t volume_index                                     = 0;
	uint32_t volume_information_offset                        = 0;
	int entry_index                                           = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libcstring_system_character_t filetime_string[ 48 ];

	libfdatetime_filetime_t *filetime                         = NULL;
	libcstring_system_character_t *value_string               = NULL;
	size_t value_string_size                                  = 0;
	uint64_t value_64bit                                      = 0;
	uint32_t value_32bit                                      = 0;
	int result                                                = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
#if SIZEOF_SIZE_T <= 4
	if( (size_t) volumes_information_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid volume information size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( (size_t) number_of_volumes > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of volumes value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading volumes information at offset: %" PRIi32 " (0x%08" PRIx32 ")\n",
		 function,
		 volumes_information_offset,
		 volumes_information_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     (off64_t) volumes_information_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek volumes information offset: %" PRIi32 " (0x%08" PRIx32 ").",
		 function,
		 volumes_information_offset,
		 volumes_information_offset );

		goto on_error;
	}
	volumes_information_data = (uint8_t *) memory_allocate(
	                                        sizeof( uint8_t ) * volumes_information_size );

	if( volumes_information_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create volumes information data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              volumes_information_data,
	              volumes_information_size,
	              error );

	if( read_count != (ssize_t) volumes_information_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read volumes information data.",
		 function );

		goto on_error;
	}
	if( io_handle->format_version == 17 )
	{
		volume_information_size = sizeof( scca_volume_information_v17_t );
	}
	else
	{
		volume_information_size = sizeof( scca_volume_information_v23_t );
	}
	for( volume_index = 0;
	     volume_index < number_of_volumes;
	     volume_index++ )
	{
		if( libscca_volume_information_initialize(
		     &volume_information,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create volume: %" PRIu32 " information.",
			 function,
			 volume_index );

			goto on_error;
		}
/* TODO check bounds of volume_information_offset */
		volume_information_data = &( volumes_information_data[ volume_information_offset ] );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: volume: %d information data:\n",
			 function,
			 volume_index );
			libcnotify_print_data(
			 volume_information_data,
			 volume_information_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->device_path_offset,
		 device_path_offset );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->device_path_number_of_characters,
		 device_path_size );

		byte_stream_copy_to_uint64_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->creation_time,
		 volume_information->creation_time );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->serial_number,
		 volume_information->serial_number );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->file_references_offset,
		 file_references_offset );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->file_references_size,
		 file_references_size );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->directory_strings_array_offset,
		 directory_strings_array_offset );

		byte_stream_copy_to_uint32_little_endian(
		 ( (scca_volume_information_v17_t *) volume_information_data )->number_of_directory_strings,
		 number_of_directory_strings );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libfdatetime_filetime_initialize(
			     &filetime,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create filetime.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: device path offset\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 device_path_offset );

			libcnotify_printf(
			 "%s: device path number of characters\t: %" PRIu32 "\n",
			 function,
			 device_path_size );

			if( libfdatetime_filetime_copy_from_byte_stream(
			     filetime,
			     ( (scca_volume_information_v17_t *) volume_information_data )->creation_time,
			     8,
			     LIBFDATETIME_ENDIAN_LITTLE,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream to filetime.",
				 function );

				goto on_error;
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libfdatetime_filetime_copy_to_utf16_string(
				  filetime,
				  (uint16_t *) filetime_string,
				  48,
				  LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
				  error );
#else
			result = libfdatetime_filetime_copy_to_utf8_string(
				  filetime,
				  (uint8_t *) filetime_string,
				  48,
				  LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
				  error );
#endif
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy filetime to string.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: creation time\t\t\t: %" PRIs_LIBCSTRING_SYSTEM " UTC\n",
			 function,
			 filetime_string );

			libcnotify_printf(
			 "%s: serial number\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 volume_information->serial_number );

			libcnotify_printf(
			 "%s: file references offset\t\t: 0x%08" PRIx32 "\n",
			 function,
			 file_references_offset );

			libcnotify_printf(
			 "%s: file references size\t\t: %" PRIu32 "\n",
			 function,
			 file_references_size );

			libcnotify_printf(
			 "%s: directory strings array offset\t: 0x%08" PRIx32 "\n",
			 function,
			 directory_strings_array_offset );

			libcnotify_printf(
			 "%s: number of directory strings\t\t: %" PRIu32 "\n",
			 function,
			 number_of_directory_strings );

			byte_stream_copy_to_uint32_little_endian(
			 ( (scca_volume_information_v17_t *) volume_information_data )->unknown1,
			 value_32bit );
			libcnotify_printf(
			 "%s: unknown1\t\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 value_32bit );

			if( io_handle->format_version >= 23 )
			{
				libcnotify_printf(
				 "%s: unknown2:\n",
				 function );
				libcnotify_print_data(
				 ( (scca_volume_information_v23_t *) volume_information_data )->unknown2,
				 28,
				 0 );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_volume_information_v23_t *) volume_information_data )->unknown3,
				 value_32bit );
				libcnotify_printf(
				 "%s: unknown3\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );

				libcnotify_printf(
				 "%s: unknown4:\n",
				 function );
				libcnotify_print_data(
				 ( (scca_volume_information_v23_t *) volume_information_data )->unknown4,
				 28,
				 0 );

				byte_stream_copy_to_uint32_little_endian(
				 ( (scca_volume_information_v23_t *) volume_information_data )->unknown5,
				 value_32bit );
				libcnotify_printf(
				 "%s: unknown5\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 value_32bit );
			}
			libcnotify_printf(
			 "\n" );

			if( libfdatetime_filetime_free(
			     &filetime,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free filetime.",
				 function );

				goto on_error;
			}
		}
#endif
		volume_information_offset += volume_information_size;

		if( file_references_offset != 0 )
		{
/* TODO add bounds check file_references_offset and file_references_size */
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: file references data:\n",
				 function );
				libcnotify_print_data(
				 &( volumes_information_data[ file_references_offset ] ),
				 file_references_size,
				 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
			}
#endif
			byte_stream_copy_to_uint32_little_endian(
			 &( volumes_information_data[ file_references_offset ] ),
			 version );

			byte_stream_copy_to_uint32_little_endian(
			 &( volumes_information_data[ file_references_offset + 4 ] ),
			 number_of_file_references );

#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: version\t\t\t: %" PRIu32 "\n",
				 function,
				 version );

				libcnotify_printf(
				 "%s: number of file references\t: %" PRIu32 "\n",
				 function,
				 number_of_file_references );

				byte_stream_copy_to_uint64_little_endian(
				 &( volumes_information_data[ file_references_offset + 8 ] ),
				 value_64bit );
				libcnotify_printf(
				 "%s: unknown1\t\t\t: 0x%08" PRIx64 "\n",
				 function,
				 value_64bit );
			}
#endif
			for( file_references_index = 1;
			     file_references_index < number_of_file_references;
			     file_references_index++ )
			{
#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					byte_stream_copy_to_uint64_little_endian(
					 &( volumes_information_data[ file_references_offset + 8 + ( file_references_index * 8 ) ] ),
					 value_64bit );

					if( value_64bit == 0 )
					{
						libcnotify_printf(
						 "%s: file reference: %d\t\t: %" PRIu64 "\n",
						 function,
						 file_references_index,
						 value_64bit );
					}
					else
					{
						libcnotify_printf(
						 "%s: file reference: %d\t\t: MFT entry: %" PRIu64 ", sequence: %" PRIu64 "\n",
						 function,
						 file_references_index,
						 value_64bit & 0xffffffffffffUL,
						 value_64bit >> 48 );
					}
				}
#endif
			}
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "\n" );
			}
#endif
		}
/* TODO add bounds check device_path_offset and device_path_size */
		if( ( device_path_offset != 0 )
		 && ( device_path_size != 0 ) )
		{
			device_path_size *= 2;

			volume_information->device_path = (uint8_t *) memory_allocate(
			                                               sizeof( uint8_t ) * device_path_size );

			if( volume_information->device_path == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create device path.",
				 function );

				goto on_error;
			}
			volume_information->device_path_size = device_path_size;

			if( memory_copy(
			     volume_information->device_path,
			     &( volumes_information_data[ device_path_offset ] ),
			     device_path_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy device path.",
				 function );

				goto on_error;
			}
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libuna_utf16_string_size_from_utf16_stream(
					  volume_information->device_path,
					  volume_information->device_path_size,
					  LIBUNA_ENDIAN_LITTLE,
					  &value_string_size,
					  error );
#else
				result = libuna_utf8_string_size_from_utf16_stream(
					  volume_information->device_path,
					  volume_information->device_path_size,
					  LIBUNA_ENDIAN_LITTLE,
					  &value_string_size,
					  error );
#endif
				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine size of volume device path string.",
					 function );

					goto on_error;
				}
				if( ( value_string_size > (size_t) SSIZE_MAX )
				 || ( ( sizeof( libcstring_system_character_t ) * value_string_size ) > (size_t) SSIZE_MAX ) )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
					 "%s: invalid volume device path string size value exceeds maximum.",
					 function );

					goto on_error;
				}
				value_string = libcstring_system_string_allocate(
				                value_string_size );

				if( value_string == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_MEMORY,
					 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
					 "%s: unable to create volume device path string.",
					 function );

					goto on_error;
				}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libuna_utf16_string_copy_from_utf16_stream(
					  (libuna_utf16_character_t *) value_string,
					  value_string_size,
					  volume_information->device_path,
					  volume_information->device_path_size,
					  LIBUNA_ENDIAN_LITTLE,
					  error );
#else
				result = libuna_utf8_string_copy_from_utf16_stream(
					  (libuna_utf8_character_t *) value_string,
					  value_string_size,
					  volume_information->device_path,
					  volume_information->device_path_size,
					  LIBUNA_ENDIAN_LITTLE,
					  error );
#endif
				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set volume device path string.",
					 function );

					goto on_error;
				}
				libcnotify_printf(
				 "%s: volume device path\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
				 function,
				 value_string );

				memory_free(
				 value_string );

				value_string = NULL;
			}
#endif
		}
/* TODO add bounds check directory_strings_array_offset */
		if( directory_strings_array_offset != 0 )
		{
/* TODO fix size calculation */
			directory_strings_array_size = volumes_information_size - directory_strings_array_offset;

			if( libfvalue_value_type_initialize(
			     &( volume_information->directory_strings ),
			     LIBFVALUE_VALUE_TYPE_STRING_UTF16,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create directory strings value.",
				 function );

				goto on_error;
			}
			value_data_size = libfvalue_value_type_set_data_strings_array(
					   volume_information->directory_strings,
					   &( volumes_information_data[ directory_strings_array_offset ] ),
					   directory_strings_array_size,
					   LIBFVALUE_CODEPAGE_UTF16_LITTLE_ENDIAN,
					   error );

			if( value_data_size == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set data of directory strings value.",
				 function );

				goto on_error;
			}
		}
		if( libcdata_array_append_entry(
		     volumes_array,
		     &entry_index,
		     (intptr_t *) volume_information,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append volume: %" PRIu32 " information to array.",
			 function,
			 volume_index );

			goto on_error;
		}
		volume_information = NULL;
	}
	memory_free(
	 volumes_information_data );

	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( value_string != NULL )
	{
		memory_free(
		 value_string );
	}
	if( filetime != NULL )
	{
		libfdatetime_filetime_free(
		 &filetime,
		 NULL );
	}
#endif
	if( volume_information != NULL )
	{
		libscca_internal_volume_information_free(
		 &volume_information,
		 NULL );
	}
	if( volumes_information_data != NULL )
	{
		memory_free(
		 volumes_information_data );
	}
	return( -1 );
}
