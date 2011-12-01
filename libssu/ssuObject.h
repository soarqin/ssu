#ifndef _SSUOBJECT_H_
#define _SSUOBJECT_H_

#include <memory.h>
#include <vector>
#include <string>
#include "ReferredObject.h"

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
		virtual bool pack(std::vector<unsigned char>& buf) = 0;
		virtual bool unpack(const void * buffer, size_t length) = 0;
		virtual size_t size() = 0;

	public:
		inline void packInt32( std::vector<unsigned char>& buf, int val )
		{
			if(val < 0)
				packUInt64(buf, static_cast<unsigned long long>(val));
			else
				packUInt32(buf, static_cast<unsigned int>(val));
		}

		inline void packSInt32( std::vector<unsigned char>& buf, int val )
		{
			packUInt32(buf, zzEncode32(val));
		}

		inline void packUInt32( std::vector<unsigned char>& buf, unsigned int val )
		{
			unsigned char nbuf[5];
			unsigned char * pbuf = nbuf;
			while(val >= 128)
			{
				*pbuf++ = static_cast<unsigned char>(val) | 0x80;
				val >>= 7;
			}
			*pbuf++ = static_cast<unsigned char>(val & 0x7F);
			buf.insert(buf.end(), nbuf, pbuf);
		}

		inline void packInt64( std::vector<unsigned char>& buf, long long val )
		{
			packUInt64(buf, static_cast<unsigned long long>(val));
		}

		inline void packSInt64( std::vector<unsigned char>& buf, long long val )
		{
			packUInt64(buf, zzEncode64(val));
		}

		inline void packUInt64( std::vector<unsigned char>& buf, unsigned long long val )
		{
			unsigned char nbuf[10];
			unsigned char * pbuf = nbuf;
			while(val >= 128ull)
			{
				*pbuf++ = static_cast<unsigned char>(val) | 0x80;
				val >>= 7;
			}
			*pbuf++ = static_cast<unsigned char>(val & 0x7F);
			buf.insert(buf.end(), nbuf, pbuf);
		}

		inline void packFloat( std::vector<unsigned char>& buf, float val )
		{
			packBinary(buf, &val, sizeof(float));
		}

		inline void packDouble( std::vector<unsigned char>& buf, double val )
		{
			packBinary(buf, &val, sizeof(double));
		}

		inline void packBinary( std::vector<unsigned char>& buf, const void * val, size_t len )
		{
			const unsigned char * pbuf = reinterpret_cast<const unsigned char *>(val);
			buf.insert(buf.end(), pbuf, pbuf + len);
		}

		template<typename T>
		inline void packObject(std::vector<unsigned char>& buf, T * val)
		{
			val->pack(buf);
		}

		template<typename T>
		inline void packReferred(std::vector<unsigned char>& buf, ReferredObject<T>& val)
		{
			packObject(buf, val.getMutable());
		}

		template<typename F, typename T>
		inline void packRepeated(std::vector<unsigned char>& buf, std::vector<T>& val)
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				r += F(buf, *it);
			}
		}

		inline void packTag( std::vector<unsigned char>& buf, unsigned int id, unsigned char type )
		{
			packUInt32(buf, (id << 3) + type);
		}

		inline void packInt32Tag( std::vector<unsigned char>& buf, unsigned int id, int val )
		{
			packTag(buf, id, 0);
			packInt32(buf, val);
		}

		inline void packSInt32Tag( std::vector<unsigned char>& buf, unsigned int id,  int val )
		{
			packTag(buf, id, 0);
			packSInt32(buf, val);
		}

		inline void packUInt32Tag( std::vector<unsigned char>& buf, unsigned int id,  unsigned int val )
		{
			packTag(buf, id, 0);
			packUInt32(buf, val);
		}

		inline void packInt64Tag( std::vector<unsigned char>& buf, unsigned int id,  long long val )
		{
			packTag(buf, id, 0);
			packInt64(buf, val);
		}

		inline void packSInt64Tag( std::vector<unsigned char>& buf, unsigned int id,  long long val )
		{
			packTag(buf, id, 0);
			packSInt64(buf, val);
		}

		inline void packUInt64Tag( std::vector<unsigned char>& buf, unsigned int id,  unsigned long long val )
		{
			packTag(buf, id, 0);
			packUInt64(buf, val);
		}

		inline void packFloatTag( std::vector<unsigned char>& buf, unsigned int id,  float val )
		{
			packTag(buf, id, 5);
			packFloat(buf, val);
		}

		inline void packDoubleTag( std::vector<unsigned char>& buf, unsigned int id,  double val )
		{
			packTag(buf, id, 1);
			packDouble(buf, val);
		}

		inline void packBinaryTag( std::vector<unsigned char>& buf, unsigned int id,  const void * val, size_t len )
		{
			packTag(buf, id, 2);
			packUInt32(buf, len);
			packBinary(buf, val, len);
		}

		template<typename T>
		inline void packObjectTag( std::vector<unsigned char>& buf, unsigned int id, T * val)
		{
			packTag(buf, id, 0);
			packUInt32(buf, static_cast<uint32>(val->size()));
			packObject(buf, val);
		}

		template<typename T>
		inline void packReferredTag( std::vector<unsigned char>& buf, unsigned int id, ReferredObject<T>& val)
		{
			packObjectTag(buf, id, val.getMutable());
		}

		template<typename F, typename T>
		inline void packRepeatedTag( std::vector<unsigned char>& buf, unsigned int id, std::vector<T>& val)
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				F(buf, *it);
			}
		}

	private:
		inline static unsigned int zzEncode32( int n )
		{
			// Note:  the right-shift must be arithmetic
			return (n << 1) ^ (n >> 31);
		}

		inline static int zzDecode32( unsigned int n )
		{
			return (n >> 1) ^ -static_cast<int>(n & 1);
		}

		inline static unsigned long long zzEncode64( long long n )
		{
			// Note:  the right-shift must be arithmetic
			return (n << 1) ^ (n >> 63);
		}

		inline static long long zzDecode64( unsigned long long n )
		{
			return (n >> 1) ^ -static_cast<long long>(n & 1);
		}
	};

}

#endif // _SSUOBJECT_H_
