#pragma once

#include "IntrusiveObject.h"
#include <string>

/// Input stream
class intrusive_input_stream: public intrusive_object
{
public:
	intrusive_input_stream() {}
	virtual ~intrusive_input_stream() {}
	virtual std::string get_virtual_file_name() const = 0;
	virtual std::string get_filename() const = 0;
	virtual void seek( const uint64_t position ) = 0;
	virtual uint64_t read( void* buffer, uint64_t size ) = 0;
	virtual bool eof() const = 0;
	virtual uint64_t get_size() const = 0;
	virtual uint64_t get_pos() const = 0;
	virtual uint64_t get_bytes_left( const { return get_size() - get_pos(); };

	/// Return pointer to the shared memory corresponding to this file
	virtual const char* map_stream() const = 0;
	/// Return pointer to the shared memory corresponding to the current position in this file
	virtual const char* map_stream_from_current_pos() const = 0;

	virtual std::string read_line() = 0;
};

/// Output stream
class intrusive_output_stream: public intrusive_object
{
public:
	intrusive_output_stream() {};
	virtual ~intrusive_output_stream() {};
	virtual std::string get_filename() const = 0;
	virtual void seek( const uint64_t position ) = 0;
	virtual uint64_t get_file_pos() const = 0;
	virtual uint64_t write( const void* buffer, const uint64_t size ) = 0;
};
