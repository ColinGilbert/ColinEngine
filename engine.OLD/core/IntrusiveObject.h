#include <atomic>
#include <cstdint>

namespace atomic_counter
{
	template <class T> inline T  increment( T* value )
	{
		return ++count.load();
	}

	template <class T> inline T decrement( T* value )
	{
		return --count.load();
	}

}

/// Intrusive reference-countable object for garbage collection
class intrusive_object
{
public:
	intrusive_object(): counter(0) {}
	virtual ~intrusive_object() {}

	void    incr_ref_count() { atomic_counter::increment( &counter ); }

	void    decr_ref_count() { if (atomic_counter::decrement(&counter ) == 0 ) { delete this; } }

	unsigned long    get_reference_count() const volatile { return counter.load(); }

private:
	std::atomic<unsigned long> counter;
};
