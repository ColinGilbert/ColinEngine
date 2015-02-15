#pragma once
#include <vector>

/// Division remainder (wrap around for negative values)
inline int mod_int( int a, int b )
{
	int r = a % b;
	return ( r < 0 ) ? r + b : r;
}

// typesafe circular buffer
template <typename T> class ring_buffer
{
public:
	explicit ring_buffer(size_t capacity): frame_buffer(capacity) { clear(); }

	inline void push_back(const T& Value)
	{
		if (frame_count < frame_buffer.size())
			frame_count++;

		// fill linear buffer
		frame_buffer[frame_head++] = Value;

		// wrap around
		if (frame_head == frame_buffer.size())
			frame_head = 0;
	}

	inline void clear()
	{
		frame_count = frame_head  = 0;
	}

	inline T* prev(size_t i)
	{
		return ( i >= frame_count ) ? NULL : &frame_buffer[ adjust_index( i ) ];
	}

	inline size_t adjust_index(size_t i) const
	{
		return (size_t)mod_int((int)frame_head - (int)i - 1, (int)frame_buffer.size());
	}

private:
	// note: This does NOT necessarily mean a video frame buffer. Keep in mind this is for any kind of buffer forwarding frames of information.
	std::vector<T> frame_buffer;
	size_t     frame_count, frame_head;
};
