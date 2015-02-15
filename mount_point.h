#pragma once

#include "Engine.h"
#include "files.h"
#include "Archive.h"
#include <sys/stat.h>

#include <algorithm>

#if defined( _WIN32 )
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif // OS_WINDOWS

inline std::string arch_fix_filename(const std::string& VName)
{
	std::string s(VName);
	std::replace(s.begin(), s.end(), '\\', PATH_SEPARATOR);
	std::replace(s.begin(), s.end(), '/', PATH_SEPARATOR);
	return s;
}

/// Mount point interface for virtual file system
class intrusive_mountpoint: public intrusive_object
{
public:
	intrusive_mountpoint(): file_name("") {}
	virtual ~intrusive_mountpoint() {}

	/// Check if virtual file virtual_name exists at this mount point
	virtual bool file_exists(const std::string& virtual_name) const = 0;
	/// Convert local file virtual_name to global name
	virtual std::string map_name(const std::string& virtual_name) const = 0;
	/// Create appropriate file reader for the specified virtual_name
	virtual intrusive_atomic_counter<intrusive_raw_file>  create_reader(const std::string& virtual_name) const = 0;

	/// Set internal mount point name
	virtual void set_name(const std::string& N)
	{
		file_name = N;
	}
	/// Get internal mount point name
	virtual std::string get_name() const { return file_name; }
private:
	std::string file_name;
};

inline bool is_full_path(const std::string& path )
{
	return ( path.find( ":\\" ) != std::string::npos ||
#if !defined( _WIN32 )
	         ( path.length() && path[0] == '/' ) ||
#endif
	         path.find( ":/" )  != std::string::npos ||
	         path.find( ".\\" ) != std::string::npos );
}

inline std::string validate_path(const std::string& path_name)
{
	std::string result = path_name;

	for ( size_t i = 0; i != result.length(); ++i )
		if ( result[i] == '/' || result[i] == '\\' )
		{
			result[i] = PATH_SEPARATOR;
		}

	return result;
}

inline bool file_physically_exists( const std::string& physical_name )
{
#ifdef _WIN32
	struct _stat buf;
	int result = _stat( validate_fs_path( physical_name ).c_str(), &buf );
#else
	struct stat buf;
	int result = stat( validate_fs_path( physical_name ).c_str(), &buf );
#endif
	return result == 0;
}

/// Mount point implementation for the physical folder
class physical_mountpoint: public intrusive_mountpoint
{
public:
	physical_mountpoint( const std::string& physical_name ): folder_physical_name( physical_name )
	{
		string_add_trailing_char( &folder_physical_name, PATH_SEPARATOR );
	}
	virtual ~physical_mountpoint() {}

	void use_virtual_filenames_set( bool Use ) { folders_use_virtual_filenames = Use; };
	bool use_virtual_filenames() const { return folders_use_virtual_filenames; };

	virtual bool         file_exists( const std::string& virtual_name ) const { return file_physically_exists( map_name( virtual_name ) ); }
	virtual std::string  map_name( const std::string& virtual_name ) const
	{
		return ( !folders_use_virtual_filenames || is_full_filesystem_path( virtual_name ) ) ? virtual_name : ( folder_physical_name + virtual_name );
	}

	virtual intrusive_atomic_counter<intrusive_raw_file>    create_reader( const std::string& virtual_name ) const
	{
		std::string physical_name = is_full_filesystem_path( virtual_name ) ? virtual_name : map_name( virtual_name );

		intrusive_atomic_counter<raw_file> file = new raw_file();
		return !file->Open( validate_fs_path( physical_name ), virtual_name ) ? NULL : file;
	}
private:
	std::string folder_physical_name;
	bool    folders_use_virtual_filenames;
};

/// The decorator to allow file name dereferencing
class alias_mountpoint: public intrusive_mountpoint
{
public:
	alias_mountpoint( const intrusive_atomic_counter<intrusive_mountpoint>& Src ): FMP( Src ) {}
	virtual ~alias_mountpoint() {}

	/// Set alias directory
	void    set_alias( const std::string& N )
	{
		file_alias = N;
		string_add_trailing_char( &file_alias, PATH_SEPARATOR ); // str::append??
	}

	/// Get internal mount point name
	std::string get_alias() const { return file_alias; }

	virtual bool            file_exists( const std::string& virtual_name ) const { return FMP->file_exists( file_alias + virtual_name ); }
	virtual std::string     map_name( const std::string& virtual_name ) const { return FMP->map_name( file_alias + virtual_name ); }
	virtual intrusive_atomic_counter<intrusive_raw_file> create_reader( const std::string& virtual_name ) const { return FMP->create_reader( file_alias + virtual_name ); }
private:
	/// Name to append to each file in this mount point
	std::string file_alias;

	/// The actual file container
	intrusive_atomic_counter<intrusive_mountpoint> FMP;
};

/// Implementation of a mount point for the .zip files
class archive_mount_point: public intrusive_mountpoint
{
public:
	archive_mount_point( const intrusive_atomic_counter<archive_reader>& R ): file_reader( R ) {}
	virtual ~archive_mount_point() {}

	virtual intrusive_atomic_counter<intrusive_raw_file>    create_reader( const std::string& virtual_name ) const
	{
		std::string file_name = arch_fix_file_name( virtual_name );

		mem_raw_file* file = new mem_raw_file();

		file->SetfileName( virtual_name );
		file->SetVirtualfileName( virtual_name );

		const void* data_ptr = file_reader->GetfileData( file_name );
		uint64_t file_size = file_reader->get_filesize( file_name );

		file->create_from_managed_buffer( data_ptr, file_size );
		return file;
	}

	virtual bool file_exists( const std::string& virtual_name ) const { return file_reader->file_exists( arch_fix_file_name( virtual_name ) ); }
	virtual std::string      map_name( const std::string& virtual_name ) const { return virtual_name; }
private:
	intrusive_atomic_counter<archive_reader> file_reader;
};
