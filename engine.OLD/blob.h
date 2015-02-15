#pragma once

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "intrusive_object.h"
#include "refcounting_ptr.h"

#undef min
#undef max

class blob: public intrusive_object
{
public:
	blob(): file_data( NULL ), file_owns_data( true ), file_size( 0 ), file_current_pos( 0 ), allocated_size( 0 ) {}
	virtual ~blob() { delete_block(); }

	/// Set the blob data pointer to some external memory block
	void set_externaldata( void* ptr, size_t sz )
	{
		delete_block();

		file_data = ptr;
		allocated_size = sz;
		file_size = sz;
		file_owns_data = false;
	}

	/// Constant access to the blob's data
	inline void* get_data_const() const { return file_data; }

	/// Direct access to blob's data
	inline void* get_data() { return file_data; }

	/// Alias (setter method) for the reallocator
	void set_size( size_t new_size ) { reallocate( new_size ); }

	/// Get current blob size
	size_t get_size() const { return file_size; }

	/// Check if this blob manages its own contents
	bool owns_data() const { return file_owns_data; }

	/// Change ownership of the memory block
	void set_ownership( bool ownership ) { file_owns_data = ownership; }

	/// Make a local copy of the other blob. Can change memory ownership of this blob on reallocation
	bool copy_blob( const recounting_ptr<blob>& other ) { return copy_memoryblock( other->get_data_const(), other->get_size() ); }

	/// Check if this blob refers to some data
	bool has_data() const  { return ( file_data != NULL ); }

	/**
	   \brief Binary comparison with other blob (strcmp() analogue)
	   Error codes: 0 if equal, -1/+1 if other is longer/shorter than this
	   0xFF - one of the blobs contains null data
	*/
	int compare_contents( const recounting_ptr<blob>& other ) const
	{
		if ( !file_data || !other->has_data() ) { return 0xFF; }

		if ( this->file_size == other->get_size() ) { return memcmp( file_data, other->get_data_const(), this->file_size ); }

		return ( this->file_size > other->get_size() ) ? 1 : -1;
	}

	/// Copy blob contents from external source
	bool copy_memoryblock( const void* from_data, size_t from_size )
	{
		if ( ( !from_data ) || ( from_size <= 0 ) ) { return false; }

		// only re-allocate if not enough space
		reallocate( from_size );

		this->file_size = from_size;

		memcpy( this->file_data, from_data, from_size );

		return true;
	}

	/// Template Set method to avoid inclusion of custom type headers (vec3/mtx etc)
	template<class T>
	void set_POD( size_t offset, T value ) { set_bytes( offset, sizeof( T ), ( char* )&value ); }

	/// Template Get method to avoid inclusion of custom type headers (vec3/mtx etc)
	template<class T>
	T get_PDD( size_t offset ) { T Tmp; get_bytes( offset, sizeof( T ), ( char* )&Tmp ); return Tmp; }

	/// Item access
	void set_byte( size_t offset, char data ) { ( ( char* )file_data )[offset] = data; }

	/// Quick access to the specififed byte. No range checking
	char get_byte( size_t offset ) const { return ( ( char* )file_data )[offset]; }

	void set_bytes( size_t offset, size_t num, const char* src ) { memcpy( ( char* )file_data + offset, src, num ); }
	void get_bytes( size_t offset, size_t num, char* out ) const { memcpy( out, ( char* )file_data + offset, num ); }
	bool append_bytes( void* data, size_t size )
	{
		const size_t blob_initial_size = 8192;

		bool should_grow = ( get_size() + size ) > allocated_size;

		if ( should_grow )
		{
			if ( !file_owns_data ) { return false; }

			size_t new_size1 = get_size() + size;
			size_t new_size2 = new_size1; // allocated_size * 2;

			size_t new_size = std::max( new_size1, new_size2 );

			if ( new_size < blob_initial_size ) { new_size = blob_initial_size; }

			file_data = realloc( file_data, new_size );
			allocated_size = new_size;
		}

		memcpy( ( char* )file_data + file_size, data, size );

		file_size += size;

		return true;
	}

	/// Ensure there is enough space for the byte array
	bool safe_setbytes( size_t offset, size_t num, const char* src )
	{
		if ( offset + num > file_size )
		{
			if ( !safe_resize( offset + num ) ) { return false; }
		}

		set_bytes( offset, num, src );

		return true;
	}

	/// Copy count bytes from another blob
	void copy_from( const recounting_ptr<blob>& other, size_t src_offset, size_t dest_offset, size_t count )
	{
		set_bytes( dest_offset, count, ( ( char* )other->get_data_const() ) + src_offset );
	}

	/// Resize and do not spoil the contents
	bool safe_resize( size_t new_size )
	{
		if ( !file_owns_data ) { return false; }

		if ( !file_data ) { allocate( new_size ); return true; }

		/// No reallocations needed ?
		if ( new_size <= file_size )
		{
			file_size = new_size;

			return true;
		}

		file_data = ::realloc( file_data, new_size );

		if ( !file_data ) { return false; }

		file_size = allocated_size = new_size;

		return true;
	}

private:
	/// Pointer to the blob data
	void*  file_data;

	/// Effective size of the blob
	size_t file_size;

	/// Actually allocated size (used for faster resize and copy operations)
	size_t allocated_size;

	/// True if this Blob manages and deallocates the memory block
	bool   file_owns_data;

	#pragma region Memory management

	/// allocate new block and change ownership type
	inline void allocate( size_t new_size )
	{
		file_data = ::malloc( new_size );
		file_size = allocated_size = new_size;
		file_owns_data = true;
	}

	/// Try to delete the memory block. Not exposed as a public method, because direct access here can cause troubles.
	inline void delete_block()
	{
		if ( !file_owns_data || file_data == NULL ) { return; }

		::free( file_data );
		file_data = NULL;
		file_size = allocated_size = 0;
		file_owns_data = false;
	}

	/// reallocate the data block if it is required
	void reallocate( size_t new_size )
	{
		if ( !file_data ) { allocate( new_size ); }

		if ( file_data != NULL )
		{
			if ( new_size > allocated_size )
			{
				// try to delete
				delete_block();

				// change ownership
				allocate( new_size );
			}
			else if ( new_size <= allocated_size )
			{
				// do nothing, just change the size
				file_size = new_size;
			}
		}
	}

	#pragma endregion

public:
	#pragma region Utility procedures

	/// Restart blob reading
	void restart_read() { file_current_pos = 0; }

	/// Restart blob writing
	void restart_write() { file_size = 0; file_current_pos = 0; }

	char read_byte { return get_byte( file_current_pos++ ); }
	void write_byte( char data ) { set_byte( file_current_pos, data ); file_current_pos++; }

	void read_bytes( char* out, size_t num ) { get_bytes( file_current_pos, num, out ); file_current_pos += num; }
	void write_bytes( const char* src, size_t num ) { if ( safe_setbytes( file_current_pos, num, src ) ) { file_current_pos += num; } }

	/// Current read/write position
	size_t file_current_pos;

	#pragma endregion
};
