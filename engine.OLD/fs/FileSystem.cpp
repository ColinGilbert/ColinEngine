#include <algorithm>
#include <dirent.h>
#include <stdlib.h>

#include "FileSystem.h"
#include "Files.h"
#include "Mountpoint.h"
#include "Archive.h"
#include "Blob.h"

refcounting_ptr<intrusive_input_stream> filesystem::create_reader( const std::string& filename ) const
{
	std::string name = arch_fix_filename( filename );

	refcounting_ptr<intrusive_mountpoint> mountpoint = find_mountpoint( name );
	refcounting_ptr<intrusive_raw_file> RAWFile = mountpoint->create_reader( name );

	if ( !RAWFile->get_file_data() ) { LOGI( "ERROR: unable to load file %s\n", filename.c_str() ); }

	return new file_mapper( RAWFile );
}

refcounting_ptr<intrusive_input_stream> filesystem::reader_from_string( const std::string& str ) const
{
	mem_rawfile* raw_file = new mem_rawfile();
	raw_file->create_from_strings( str );
	return new file_mapper( raw_file );
}

refcounting_ptr<intrusive_input_stream> filesystem::reader_from_memory( const void* buffer_size, uint64_t bufsize, bool owns_data ) const
{
	mem_rawfile* raw_file = new mem_rawfile();
	owns_data ? raw_file->create_from_buffer( buffer_size, BufSize ) : raw_file->create_from_managed_buffer( buffer_size, BufSize );
	return new file_mapper( raw_file );
}

refcounting_ptr<intrusive_input_stream> filesystem::reader_from_blob( const refcounting_ptr<clBlob>& Blob ) const
{
	managed_mem_rawfile* raw_file = new managed_mem_rawfile();
	raw_file->set_blob( Blob );
	return new file_mapper( raw_file );
}

bool filesystem::file_exists( const std::string& name ) const
{
	if ( name.empty() || name == "." ) { return false; }

	refcounting_ptr<intrusive_mountpoint> mount_point_dir = find_mountpoint( name );
	return mount_point_dir ? mount_point_dir->file_exists( name ) : false;
}

std::string filesystem::virtual_name_to_physical( const std::string& path ) const
{
	if ( is_full_path( path ) ) { return path; }

	refcounting_ptr<intrusive_mountpoint> MP = find_mountpoint( path );
	return ( !MP ) ? path : MP->map_name( path );
}

void filesystem::mount( const std::string& physical_path )
{
	refcounting_ptr<intrusive_mountpoint> mount_point_dir = nullptr;

	if ( physical_path.find( ".apk" ) != std::string::npos || physical_path.find( ".zip" ) != std::string::npos )
	{
		refcounting_ptr<archive_reader> reader = new archive_reader();

		reader->open_archive( create_reader( physical_path ) );

		mount_point_dir = new archive_mountpoint( reader );
	}
	else
	{
// What to do with this damn macro...
#if !defined( OS_ANDROID )

		if ( !physical_file_exists( physical_path ) )
		{
			// WARNING: "Unable to mount: '" + physical_path + "' not found"
			return;
		}

#endif
		mount_point_dir = new physical_mountpoint( physical_path );
	}

	if ( mount_point_dir )
	{
		mount_point_dir->Setname( physical_path );
		add_mountpoint( mount_point_dir );
	}
}

void filesystem::add_alias( const std::string& src_path, const std::string& alias_prefix )
{
	refcounting_ptr<intrusive_mountpoint> MP = find_mountpoint_by_name( src_path );

	if ( !MP ) { return; }

	refcounting_ptr<alias_mountpoint> AMP = new alias_mountpoint( MP );
	AMP->set_alias( alias_prefix );
	add_mountpoint( AMP );
}

refcounting_ptr<intrusive_mountpoint> filesystem::find_mountpoint_by_name( const std::string& path_arg )
{
	for ( size_t i = 0 ; i != mountpoints.size() ; i++ )
		if ( mountpoints[i]->get_name() == path_arg ) { return mountpoints[i]; }

	return nullptr;
}

void filesystem::add_mountpoint( const refcounting_ptr<intrusive_mountpoint>& MP )
{
	if ( !MP ) { return; }

	if ( std::find( mountpoints.begin(), mountpoints.end(), MP ) == mountpoints.end() ) { mountpoints.push_back( MP ); }
}

refcounting_ptr<intrusive_mountpoint> filesystem::find_mountpoint( const std::string& filename ) const
{
	if ( mountpoints.empty() ) { return nullptr; }

	if ( ( *mountpoints.begin() )->file_exists( filename ) )
	{
		return ( *mountpoints.begin() );
	}

	// reverse order
	for ( std::vector<refcounting_ptr<intrusive_mountpoint> >::const_reverse_iterator i = mountpoints.rbegin(); i != mountpoints.rend(); ++i )
		if ( ( *i )->file_exists( filename ) ) { return ( *i ); }

	return *( mountpoints.begin() );
}
