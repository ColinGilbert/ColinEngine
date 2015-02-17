/* -------------------------------------------- */

// Implementation of the curiously-recurring template pattern
// From the excellent answer posted by TemplateRex in: http://stackoverflow.com/questions/14323595/best-way-to-declare-an-interface-in-c11
// For static polymorphism
/*
   template<typename Derived> class EnableStaticDowncast
   {
   private:
   typedef EnableStaticDowncast Base;
   public:
   Derived const* self() const
   {
   return static_cast<Derived const*>(this);
   }
   Derived* self()
   {
   return static_cast<Derived*>(this);
   }
   protected:
// disable deletion of Derived* through Base*
// enable deletion of Base* though Derived*
~EnableStaticDowncast() = default; // C++11-only
};
*/
/* -------------------------------------------- */

// Example usage. Use to achieve static polymorphism.

/*

   template<typename Impl> class StaticInterface: public EnableStaticDowncast<Impl> // enable static polymorphism
   {
   private:
   using EnableStaticDowncast<Impl>::self // dependent name now in scope
   public:
   void fun() { self()->do_fun(); } // Oh look implicitly-inlined!
   protected:
// Disable deletion of Derived* through Base*
// Enable deletion of Base* through Derived*
~StaticInterface() = default; // C++11-only
};

class StaticImplementation: public StaticInterface<StaticImplementation>
{
private: // Implementation code
friend class StaticInterface<StaticImplementation>;
void do_fun()
{
// Implementation here lolz
}
};

*/

/* ------------------------------------------ */

// As opposed to a dynamic interface:

/*
   class DynamicInterface
   {
   public: // non-virtual interface
   void fun() { do_fun(); } // equivalent to "this->do_fun()"
// enable deletion of a Derived* through a Base*
virtual ~DynamicInterface() = default;
private: // pure virtual implementation
virtual void do_fun() = 0; 
};

class DynamicImplementation: public DynamicInterface
{
private: // Implementation goes in there
virtual void do_fun()
{
// Implementation code...
}
};

*/
#ifndef NOOBWERKZ_BINARY_FILE
#define NOOBWERKZ_BINARY_FILE

#include <mutex>
#include <vector>
#include <cctypes>

class binary_file
{
	public:
		binary_file(std::string _name, uint64_t _size, std::unique_ptr<vector<unsigned char>> _data)
		{
			name = _name;
			size = _size;
			data = std::move(_data);
			usable.set(true);
		}

		std::vector<unsigned char> get_data()
		{
			if (usable.load() == true)
			{
				usable.set(false);
				std::lock_guard<std::mutex> lock(this->data_mutex);
				std::vector<unsigned char> return_value = std::copy(*data);
				usable.set(true);
				return return_value;
			}
			else return new vector<unsigned char>();
		}

		const uint64_t get_size() { return size; }

		const std::string get_name() { return name; }

	private:
		std::atomic<boolean> usable;
		const std::string name;
		const uint64_t size;
		const std::unique_ptr<std::vector<unsigned char>> data;
		std::mutex data_mutex;
};

#endif

#ifndef NOOBWERKZ_ENABLE_STATIC_DOWNCAST
#define NOOBWERKZ_ENABLE_STATIC_DOWNCAST

template<typename derived> class enable_static_downcast
{
	private:
		typedef enable_static_downcast base;
	public:
		derived const* self() const
		{
			return static_cast<derived const*>(this);
		}
		derived* self()
		{
			return static_cast<derived*>(this);
		}
	protected:
		~enable_static_downcast() = default; 
};

#endif

#ifndef NOOBWERKZ_FS_STATIC_INTERFACE
#define NOOBWERKZ_FS_STATIC_INTERFACE

template<typename abstract_filesystem> class filesystem_interface_static: public enable_static_downcast<abstract_filesystem>
{
	private:
		using enable_static_downcast<abstract_filesystem>::self

	public:
			bool mount_raw_binary_file(const std::string& path)
			{
				return self()->mount_binary_blob(path);
			}

			vector<std::string> list_directory(const std::string& path)
			{
				return self()->list_directory(path);
			}

			std::weak_ptr<binary_file> get_binary_file(const std::string& path)
			{
				if map.exists(path)
				{
					std::weak_ptr<binary_file> ptr = map.get(blobs.second(path));
					return ptr;
				}
				else return weak_ptr<nullptr>;
			}

	protected:
			std::map< std::string, std::shared_ptr<binary_file> > blobs;
			~static_interface() = default;
};

#endif

/*

#include <dirent.h>

#include <vector>
#include <string>
using namespace std;

Read a directory listing into a vector of strings, filtered by file extension.
Throws std::exception on error.
std::vector<std::string> readDirectory(const std::string &directoryLocation, const std::string &extension)
{
vector<string> result;
string lowercase_extension( strToLower(extension) );

DIR *dir;
struct dirent *ent;

if ((dir = opendir(directoryLocation.c_str())) == nullptr) {
throw std::exception("readDirectory() - Unable to open directory.");
}

while ((ent = readdir(dir)) != nullptr)
{
string entry( ent->d_name );
string lowercase_entry( strToLower(entry) );

// Check extension matches (case insensitive)
size_t pos = lowercase_entry.rfind(lowercase_extension);
if (pos!=string::npos && pos==lowercase_entry.length()-lowercase_extension.length()) {
result.push_back( entry );
}
}

if (closedir(dir) != 0) {
throw std::exception("readDirectory() - Unable to close directory.");
}

return result;
}

*/ 



#ifndef NOOBWERKZ_FS_STATIC_IMPLEMENTATION_POSIX
#define NOOBWERKZ_FS_STATIC_IMPLEMENTATION_POSIX

#include <dirent.h>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <cctypes>


class filesystem_posix: public filesystem_interface_static<static_implementation>
{
	private:
		friend class filesystem_interface_static<static_implementation>;
	public:
		bool mount_raw_binary_file(const std::string& path)
		{

		}

		std::vector<std::string> list_directory(const std::string& path)
		{
			std::vector<std::string> = new std::vector<std::string>();

			DIR *dir;
			struct dirent *ent;

			if ((dir = opendir(path.c_str())) == nullptr)
			{
				throw std::exception("list_directory() - Unable to open directory!");
			}

			while ((ent = readdir(dir)) != nullptr)
			{
				string entry( ent->d_name );
				string lowercase_entry( strToLower(entry) );

				// Check extension matches (case insensitive)
				size_t pos = lowercase_entry.rfind(lowercase_extension);
				if (pos!=string::npos && pos==lowercase_entry.length())
				{
					result.push_back( entry );
				}
			}

			if (closedir(dir) != 0) {
				throw std::exception("list_directory() - Unable to close directory.");
			}

			return result;
		}

};

#endif
