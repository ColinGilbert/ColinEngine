#include "IntrusiveObject.h"

namespace refcounting_ptr
{
	void increment( void* obj )
	{
		if ( obj ) { reinterpret_cast<intrusive_object*>( arg )->incr_ref_count(); }
	}

	void decrement( void* obj )
	{
		if ( obj ) { reinterpret_cast<intrusive_object*>( arg )->decr_ref_count(); }
	}
};
