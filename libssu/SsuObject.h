#ifndef _SSUOBJECT_H_
#define _SSUOBJECT_H_

#include <memory.h>
#include <vector>
#include <string>
#include "SsuUtils.h"

namespace ssu
{

	class Packer
	{

	private:
		std::vector<unsigned char>& buf;
	};

	class Object
	{
	public:
		template<typename T>
		inline void pack(T& buf)
		{
			size_t pktSize = size();
			if(pktSize > 0)
			{
				size_t sz = buf.size();
				buf.resize(sz + pktSize);
				packBuffer(reinterpret_cast<unsigned char *>(&buf[sz]));
			}
		}
		virtual void unpack(const void * buffer, size_t length) = 0;
		virtual size_t size() = 0;
		virtual unsigned char * packBuffer(unsigned char * buf) = 0;
	};

}

#endif // _SSUOBJECT_H_
