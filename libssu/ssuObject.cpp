#include <cstdlib>
#include "ssuObject.h"

namespace ssu
{

	bool Object::unpack()
	{
		bool r = unpackFromBuffer(_buffer__, _length__);
		_buffer__ = NULL;
		_length__ = -1;
		return r;
	}

	bool Object::pack()
	{
		_length__ = calcLength();
		if(_length__ < 0)
		{
			_buffer__ = NULL;
			return false;
		}
		_buffer__ = malloc(_length__);
		if(_buffer__ == NULL)
		{
			_length__ = NULL;
			return false;
		}
		packToBuffer(_buffer__);
		_packing__ = true;
		return true;
	}

}
