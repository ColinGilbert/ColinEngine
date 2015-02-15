#pragma once

#include "Files.h"

#include <vector>

class intrusive_mountpoint;

class filesystem: public intrusive_object
{
public:
	filesystem() {}
	virtual ~filesystem() {}

	refcounting_ptr<intrusive_inputstream> create_reader( const std::string& filename ) const;
	refcounting_ptr<intrusive_inputstream> from_from_string( const std::string& str ) const;
	refcounting_ptr<intrusive_inputstream> reader_from_memory( const void* buffer_ptr, uint64_t buffer_size, bool owns_data ) const;
	refcounting_ptr<intrusive_inputstream> reader_from_blob( const refcounting_ptr<blob>& Blob ) const;

	refcounting_ptr<blob> LoadFileAsBlob( const std::string& name ) const
	{
	refcounting_ptr<intrusive_inputstream> input = create_reader( name );
	refcounting_ptr<blob> resource = new blob();
	resource->copy_memoryblock( input->map_stream(), ( size_t )input->get_size() );
	return resource;
	}

	void mount( const std::string& physical_path );
	void add_alias_mountpoint( const std::string& src_path, const std::string& alias_prefix );
	void add_mountpoint( const refcounting_ptr<intrusive_mountpoint>& MP );

	std::string virtual_name_to_physical( const std::string& path ) const;
	bool file_exists( const std::string& name ) const;
private:
	refcounting_ptr<intrusive_mountpoint> find_mountpoint_by_name( const std::string& path );
	/// Search for a mount point for this file
	refcounting_ptr<intrusive_mountpoint> find_mountpoint( const std::string& filename ) const;
	std::vector< refcounting_ptr<intrusive_mountpoint> > mountpoints;
};

inline refcounting_ptr<memfile_writer> create_memwriter( const std::string& filename, uint64_t initial_size )
{
	refcounting_ptr<blob> B = new blob();
	B->set_size( static_cast<size_t>( initial_size ) );

	refcounting_ptr<memfile_writer> stream = new memfile_writer( B );
	stream->set_maxsize( initial_size * 2 ); // default value
	stream->set_filename( filename );

	return stream;
}
