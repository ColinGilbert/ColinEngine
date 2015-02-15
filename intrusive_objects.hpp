#include <cstdint>

namespace atomic_counter
{
	template <class T> inline T Inc( T* Value )
	{
		return ++count(load);
	}

	template <class T> inline T Dec( T* Value )
	{
		return --count(load);
	}

}

/// Intrusive reference-countable object for garbage collection
class intrusive_counter
{
public:
	intrusive_object(): intrusive_counter(0) {}
	virtual ~intrusive_object() {}

	void    incr_ref_count() 
	{
		atomic_counter::incr(&counter);
	}

	void    decr_ref_count()
	{
		if (atomic_counter::decr(&counter) == 0) delete this;
	}

	unsigned long    get_reference_count() const volatile
	{
		return counter.load();
	}

private:
	std::atomic<unsigned long> counter;
};
