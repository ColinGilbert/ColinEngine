/*
Potential excercise: template this beautiful mess
*/
#pragma once

#include "Streams.h"
#include "Blob.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class intrusive_rawfile: public intrusive_object
{
public:
	intrusive_rawfile() {};
	virtual ~intrusive_rawfile() {};

	void set_virtual_filename( const std::string vname ) { virtual_filename = vname; }
	void set_filename( const std::string name ) { filename = name; }

	std::string get_virtual_filename() const { return virtual_filename; }
	std::string get_filename() const { return filename; }

	virtual const char* get_filedata() const = 0;
	virtual uint64_t get_filesize() const = 0;
protected:
	std::string filename;
	std::string virtual_filename;
};

/// Physical file representation
class raw_file: public intrusive_rawfile
{
public:
	raw_file() {}
	virtual ~raw_file() { close(); }

	bool open( const std::string& filename, const std::string virtual_filename )
	{
		set_filename( filename );
		set_virtual_filename( virtual_filename );

		file_size = 0;
		file_data = nullptr;
		file_handle = open( file_name.c_str(), O_RDONLY );

		struct stat file_info;
		fstat( file_handle, &file_info );
		// Optionally check strerror( errno ), if fstat returns -1

		file_size = static_cast<uint64_t>( file_info.st_size );

		// don't call mmap() for zero-sized files
		if ( file_size )
		{
			// create share/read-only file mapping
			file_data = ( char* )( mmap( nullptr, file_size, PROT_READ, MAP_PRIVATE, file_handle, 0 ) );
		}

		close( file_handle );

		return true;
	}

	void close()
	{
		if ( file_data ) { munmap( reinterpret_cast<size_t*>( file_data ), file_size ); }
	}

	virtual const char* get_filedata() const { return file_data; }
	virtual uint64_t get_filesize() const { return file_size; }
private:

	int file_handle;

	char* file_data;
	uint64_t file_size;
};

class mem_rawfile: public intrusive_rawfile
{
public:
	mem_rawfile()
	{
		buffer = nullptr;
		buffer_size = 0;
		owns_buffer = false;
	}

	virtual ~mem_rawfile() { delete_buffer(); }

	virtual const char* get_filedata() const { return reinterpret_cast<const char*>( buffer ); }
	virtual uint64_t get_filesize() const { return buffer_size; }

	void create_from_string( const std::string& input_string )
	{
		delete_buffer();
		buffer_size = input_string.length();

		if ( !input_string.empty() )
		{
			char* local_buffer = new char[ input_string.length() ];
			memcpy( local_buffer, input_string.c_str(), static_cast<size_t>( buffer_size ) );
			buffer = local_buffer;
			owns_buffer = true;
		}
	}

	void create_from_buffer( const size_t* buffer_ptr, uint64_t buffer_size )
	{
		delete_buffer();
		buffer = buffer_ptr;
		buffer_size = buffer_size;
		owns_buffer = true;
	}

	void create_from_managed_buffer( const size_t* buffer_ptr, uint64_t buffer_size )
	{
		delete_buffer();
		buffer = buffer_ptr;
		buffer_size = buffer_size;
		owns_buffer = false;
	}

private:
	void delete_buffer()
	{
		if ( buffer )
		{
			if ( owns_buffer )
			{
				delete [] reinterpret_cast<const char*>( buffer );
			}

			buffer = nullptr;
		}
	}

	// do we own the buffer ?
	bool owns_buffer;
	std::unique_ptr<size_t> buffer;
	uint64_t buffer_size;
};

class managed_inmem_rawfile: public intrusive_rawfile
{
public:
	managed_inmem_rawfile(): file_blob( nullptr ) {}

	virtual const char* get_filedata() const { return ( const char* )file_blob->get_data(); }
	virtual uint64_t get_filesize() const { return file_blob->get_size(); }

	void set_blob( const shared_ptr<blob> ptr ) { file_blob = ptr; }
private:
	shared_ptr<blob> file_blob;
};

class file_mapper
{
public:
	file_mapper( refcounting_ptr<intrusive_rawfile> File ): file( File ), position( 0 ) {}
	virtual ~file_mapper() {}

	virtual std::string get_virtual_filename() const { return file->get_virtual_filename(); }
	virtual std::string get_filename() const { return file->get_filename(); }
	virtual uint64_t read( void* buf, uint64_t size )
	{
		uint64_t real_size = ( size > get_bytes_remaining() ) ? get_bytes_remaining() : size;
		memcpy( buf, ( file->get_filedata() + position ), static_cast<size_t>( real_size ) );
		position += real_size;
		return real_size;
	}

	virtual void seek( const uint64_t seek_position ){ position = seek_position; }

	virtual uint64_t get_size() const { return file->get_filesize(); }
	virtual uint64_t get_pos()  const { return position; }
	virtual bool eof() const { return ( position >= file->get_filesize() ); }

	virtual const char* map_stream() const { return file->get_filedata(); }
	virtual const char* map_stream_from_current_position() const { return ( file->get_filedata() + position ); }

	virtual std::string read_line()
	{
		const size_t MAX_LINE_WIDTH = 65535;

		char buf[ MAX_LINE_WIDTH + 1 ];

		const char* position_placeholder = map_stream_from_current_position();
		char* out = buf;
		char* end = buf + MAX_LINE_WIDTH;

		while ( !eof() && out < end )
		{
			position_placeholder++;

			char C = ( *current_position++ );
:%s/
			if ( C == 13   ) { continue; }   // kill char

			if ( C == 10   ) { break; }

			*out++ = C;
		}

		( *out ) = 0;

		return std::string( buf );
	}

private:
	refcounting_ptr<intrusive_rawfile> file;
	uint64_t position;
};

class file_writer:
{
public:
	file_writer(): position( 0 ) {}
	virtual ~file_writer() { close(); }

	bool open( const std::string file_name )
	{
		filename = file_name;
		mapped_file = open( filename.c_str(), O_WRONLY | O_CREAT );
		position = 0;
		return !( mapped_file == -1 );
	}

	void close()
	{

		if ( mapped_file != -1 ) { close( mapped_file ); }

	}

	virtual std::string get_filename() const { return filename; }
	virtual uint64_t get_file_position() const { return position; }
	virtual void seek( const uint64_t seek_position )
	{

		if ( mapped_file != -1 ) { lseek( mapped_file, seek_position, SEEK_SET ); }
		position = seek_position;
	}

	virtual uint64_t write( const void* buf, const uint64_t Size )
	{

		if ( mapped_file != -1 ) { write( mapped_file, buf, Size ); }

		position += Size;
		return Size;
	}

private:
	std::string filename;

	int mapped_file;

	uint64_t position;
};

/// File writer for some dynamically-sized blob
class memoryfile_writer: public intrusive_outstream
{
public:
	memoryfile_writer( refcounting_ptr<blob> container_init ): position( 0 ), maxsize(), container( container_init ), filename( "" ) {}
	virtual ~memoryfile_writer() {}

	virtual void set_filename( const std::string name ) { filename = name; }
	virtual std::string get_filename() const { return filename; }
	virtual uint64_t get_file_position() const { return position; }

	/// Get/Set maximum allowed size for the in-mem file
	uint64_t get_maxsize() const { return maxsize; }
	void set_maxsize( uint64_t maxsize_arg ) { maxsize = maxsize_arg; }

	/// change absolute file position to seek_position
	virtual void seek( const uint64_t seek_position )
	{
		if ( seek_position > container->get_size() )
		{
			/// check for oversize
			if ( seek_position > maxsize - 1 ) { return; }

			/// Resize the blob
			if ( !container->safe_resize( static_cast<size_t>( seek_position ) + 1 ) ) { return; }
		}

		position = seek_position;
	}

	/// Write Size bytes from buf
	virtual uint64_t write( const size_t* buf, const uint64_t size )
	{
		uint64_t this_pos = position;

		/// Ensure there is enough space
		seek( this_pos + size );

		if ( position + size > maxsize ) { return 0; }

		size_t* destination_ptr = ( size_t* )( &( ( ( char* )( container->get_data() ) )[this_pos] ) );

		/// Write the data
		memcpy( destination_ptr, buf, static_cast<size_t>( size ) );

		return size;
	}

	/// Access internal data container - NOTE: GOOD OR BAD THING???
	shared_ptr<blob> get_container() const { return container; }
	void set_container( const shared_ptr<blob>& B ) { container = B; }
private:
	/// Actual file contents
	std::shared_ptr<blob> container;
	/// Maximum allowable size
	uint64_t maxsize;
	/// Virtual file name
	std::string filename;
	/// Current position
	uint64_t position;
};
