#ifndef _REFERREDOBJECT_H_
#define _REFERREDOBJECT_H_

#include <cstdlib>

namespace ssu
{

template<typename T>
class ReferredObject
{
public:
	inline ReferredObject<T>(): _obj(NULL) {}
	virtual ~ReferredObject<T>() { delete _obj; }
	inline operator const T&() const { checkObject(); return *_obj; }
	inline operator T&() { checkObject(); return *_obj; }
	inline ReferredObject<T>& operator=(const T& other ) { checkObject(); *_obj = other; return *this; }
	inline T * getMutable() { return _obj; }
private:
	inline void checkObject() const
	{
		if(!_obj)
			_obj = new(std::nothrow) T;
	}
private:
	mutable T * _obj;
};

}

#endif // _REFERREDOBJECT_H_
