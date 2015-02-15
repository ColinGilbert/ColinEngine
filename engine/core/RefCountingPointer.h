#pragma once

#ifndef NULL
#  define NULL 0
#endif
// TODO: Potentiall get rid of void* pointers
namespace inner_pointers
{
	void incr_ref_count( void* p );
	void decr_ref_count( void* p );
};

/// Intrusive smart pointer
template <class T> class refcounting_ptr
{
private:
	class protector
	{
	private:
		void operator delete( void* );
	};
public:
	/// default constructor
	refcounting_ptr(): wrapped_object( 0 )
	{
	}
	/// copy constructor
	refcounting_ptr( const refcounting_ptr& ptr ): wrapped_object( ptr.wrapped_object )
	{
		inner_pointers::incr_ref_count( wrapped_object );
	}
	template <typename U> refcounting_ptr( const refcounting_ptr<U>& ptr ): wrapped_object( ptr.get_internal_ptr() )
	{
		inner_pointers::incr_ref_count( wrapped_object );
	}
	/// constructor from T*
	refcounting_ptr( T* const Object ): wrapped_object( Object )
	{
		inner_pointers::incr_ref_count( wrapped_object );
	}
	/// destructor
	~refcounting_ptr()
	{
		inner_pointers::decr_ref_count( wrapped_object );
	}
	/// check consistency
	inline bool IsValid() const
	{
		return wrapped_object != 0;
	}
	/// assignment of refcounting_ptr
	refcounting_ptr& operator = ( const refcounting_ptr& rhs )
	{
		T* temp = wrapped_object;
		wrapped_object = rhs.wrapped_object;

		inner_pointers::incr_ref_count( lhs.wrapped_object );
		inner_pointers::decr_ref_count( temp );

		return *this;
	}
	/// -> operator
	inline T* operator -> () const
	{
		return wrapped_object;
	}
	/// allow "if ( refcounting_ptr )" construction
	inline operator pointer_protector* () const
	{
		if ( !wrapped_object )
		{
			return NULL;
		}

		static pointer_protector protector;

		return &protector;
	}
	/// cast
	template <typename U> inline refcounting_ptr<U> custom_dynamic_cast() const
	{
		return refcounting_ptr<U>( dynamic_cast<U*>( wrapped_object ) );
	}
	/// compare
	template <typename U> inline bool operator == ( const refcounting_ptr<U>& ptr_one ) const
	{
		return wrapped_object == ptr_one.get_internal_ptr();
	}
	template <typename U> inline bool operator == ( const U* ptr_one ) const
	{
		return wrapped_object == ptr_one;
	}
	template <typename U> inline bool operator != ( const refcounting_ptr<U>& ptr_one ) const
	{
		return wrapped_object != ptr_one.get_internal_ptr();
	}
	/// helper
	inline T* get_internal_ptr() const
	{
		return wrapped_object;
	}
private:
	T*    wrapped_object;
};
