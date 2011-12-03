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
		virtual void pack(std::vector<unsigned char>& buf) = 0;
		virtual void unpack(const void * buffer, size_t length) = 0;
		virtual size_t size() = 0;

	protected:
		static inline size_t sizeInt32( int val )
		{
			if(val < 0)
				return sizeUInt64(static_cast<unsigned long long>(val));
			return sizeUInt32(static_cast<unsigned int>(val));
		}

		static inline size_t sizeSInt32( int val )
		{
			return sizeUInt32(zzEncode32(val));
		}

		static inline size_t sizeUInt32( unsigned int val )
		{
			if(val < (1 << 14))
			{
				if(val < (1 << 7))
					return 1;
				else
					return 2;
			}
			else
			{
				if(val < (1 << 21))
					return 3;
				else if(val < (1 << 28))
					return 4;
				else return 5;
			}
		}

		static inline size_t sizeInt64( long long val )
		{
			return sizeUInt64(static_cast<unsigned long long>(val));
		}

		static inline size_t sizeSInt64( long long val )
		{
			return sizeUInt64(zzEncode64(val));
		}

		static inline size_t sizeUInt64( unsigned long long val )
		{
			if(val < (1ull << 28))
			{
				if(val < (1ull << 14))
				{
					if(val < (1ull << 7))
						return 1;
					else
						return 2;
				}
				else
				{
					if(val < (1ull << 21))
						return 3;
					else
						return 4;
				}
			}
			else
			{
				if(val < (1ull << 42))
				{
					if(val < (1ull << 35))
						return 5;
					else
						return 6;
				}
				else
				{
					if(val < (1ull << 56))
					{
						if(val < (1ull << 49))
							return 7;
						else
							return 8;
					}
					else
					{
						if(val < (1ull << 63))
							return 9;
						else
							return 10;
					}
				}
			}
		}

		static inline size_t sizeFloat( float val )
		{
			return sizeof(float);
		}

		static inline size_t sizeDouble( double val )
		{
			return sizeof(double);
		}

		static inline size_t sizeBool( bool val )
		{
			return 1;
		}

		static inline size_t sizeString( const std::string& val )
		{
			return val.size();
		}

		static inline size_t sizeVector( const std::vector<unsigned char>& val )
		{
			return val.size();
		}

		static inline size_t sizeBinary( const void * val, size_t len )
		{
			return len;
		}

		template<typename T>
		static inline size_t sizeObject(T * val)
		{
			size_t sz = val->size();
			return sizeUInt32(sz) + sz;
		}

		template<typename T>
		static inline size_t sizeReferred(ReferredObject<T>& val)
		{
			return val.getMutable()->size();
		}

		template<typename T, typename F>
		static inline size_t sizeRepeated(std::vector<T>& val, F func)
		{
			size_t sz = 0;
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				sz += func(*iter);
			}
			return sz;
		}

		static inline void packInt32( std::vector<unsigned char>& buf, int val )
		{
			if(val < 0)
				packUInt64(buf, static_cast<unsigned long long>(val));
			else
				packUInt32(buf, static_cast<unsigned int>(val));
		}

		static inline void packSInt32( std::vector<unsigned char>& buf, int val )
		{
			packUInt32(buf, zzEncode32(val));
		}

		static inline void packUInt32( std::vector<unsigned char>& buf, unsigned int val )
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

		static inline void packInt64( std::vector<unsigned char>& buf, long long val )
		{
			packUInt64(buf, static_cast<unsigned long long>(val));
		}

		static inline void packSInt64( std::vector<unsigned char>& buf, long long val )
		{
			packUInt64(buf, zzEncode64(val));
		}

		static inline void packUInt64( std::vector<unsigned char>& buf, unsigned long long val )
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

		static inline void packFloat( std::vector<unsigned char>& buf, float val )
		{
			packBinary(buf, &val, sizeof(float));
		}

		static inline void packDouble( std::vector<unsigned char>& buf, double val )
		{
			packBinary(buf, &val, sizeof(double));
		}

		static inline void packBool( std::vector<unsigned char>& buf, bool val )
		{
			buf.push_back(val ? 1 : 0);
		}

		static inline void packString( std::vector<unsigned char>& buf, const std::string& val )
		{
			buf.insert(buf.end(), val.begin(), val.end());
		}

		static inline void packVector( std::vector<unsigned char>& buf, const std::vector<unsigned char>& val )
		{
			buf.insert(buf.end(), val.begin(), val.end());
		}

		static inline void packBinary( std::vector<unsigned char>& buf, const void * val, size_t len )
		{
			const unsigned char * pbuf = reinterpret_cast<const unsigned char *>(val);
			buf.insert(buf.end(), pbuf, pbuf + len);
		}

		template<typename T>
		static inline void packObject(std::vector<unsigned char>& buf, T * val)
		{
			val->pack(buf);
		}

		template<typename T>
		static inline void packReferred(std::vector<unsigned char>& buf, ReferredObject<T>& val)
		{
			packObject(buf, val.getMutable());
		}

		template<typename T, typename F>
		static inline void packRepeated(std::vector<unsigned char>& buf, std::vector<T>& val, F func)
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				func(buf, *iter);
			}
		}

		static inline void packTag( std::vector<unsigned char>& buf, unsigned int id, unsigned char type )
		{
			packUInt32(buf, (id << 3) + type);
		}

		static inline void packInt32Tag( std::vector<unsigned char>& buf, unsigned int id, int val )
		{
			packTag(buf, id, 0);
			packInt32(buf, val);
		}

		static inline void packSInt32Tag( std::vector<unsigned char>& buf, unsigned int id, int val )
		{
			packTag(buf, id, 0);
			packSInt32(buf, val);
		}

		static inline void packUInt32Tag( std::vector<unsigned char>& buf, unsigned int id, unsigned int val )
		{
			packTag(buf, id, 0);
			packUInt32(buf, val);
		}

		static inline void packInt64Tag( std::vector<unsigned char>& buf, unsigned int id, long long val )
		{
			packTag(buf, id, 0);
			packInt64(buf, val);
		}

		static inline void packSInt64Tag( std::vector<unsigned char>& buf, unsigned int id, long long val )
		{
			packTag(buf, id, 0);
			packSInt64(buf, val);
		}

		static inline void packUInt64Tag( std::vector<unsigned char>& buf, unsigned int id, unsigned long long val )
		{
			packTag(buf, id, 0);
			packUInt64(buf, val);
		}

		static inline void packFloatTag( std::vector<unsigned char>& buf, unsigned int id, float val )
		{
			packTag(buf, id, 5);
			packFloat(buf, val);
		}

		static inline void packDoubleTag( std::vector<unsigned char>& buf, unsigned int id, double val )
		{
			packTag(buf, id, 1);
			packDouble(buf, val);
		}

		static inline void packBoolTag( std::vector<unsigned char>& buf, unsigned int id, bool val )
		{
			packTag(buf, id, 0);
			packBool(buf, val);
		}

		static inline void packStringTag( std::vector<unsigned char>& buf, unsigned int id, const std::string& val )
		{
			packTag(buf, id, 2);
			packUInt32(buf, val.size());
			packString(buf, val);
		}

		static inline void packVectorTag( std::vector<unsigned char>& buf, unsigned int id, const std::vector<unsigned char>& val )
		{
			packTag(buf, id, 2);
			packUInt32(buf, val.size());
			packVector(buf, val);
		}

		static inline void packBinaryTag( std::vector<unsigned char>& buf, unsigned int id,  const void * val, size_t len )
		{
			packTag(buf, id, 2);
			packUInt32(buf, len);
			packBinary(buf, val, len);
		}

		template<typename T>
		static inline void packObjectTag( std::vector<unsigned char>& buf, unsigned int id, T * val)
		{
			packTag(buf, id, 2);
			packUInt32(buf, static_cast<unsigned int>(val->size()));
			packObject(buf, val);
		}

		template<typename T>
		static inline void packReferredTag( std::vector<unsigned char>& buf, unsigned int id, ReferredObject<T>& val)
		{
			packObjectTag(buf, id, val.getMutable());
		}

		template<typename T, typename F>
		static inline void packRepeatedTag( std::vector<unsigned char>& buf, unsigned int id, std::vector<T>& val, F func )
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				func(buf, id, *iter);
			}
		}

	private:
		static inline unsigned int zzEncode32( int n )
		{
			// Note:  the right-shift must be arithmetic
			return (n << 1) ^ (n >> 31);
		}

		static inline int zzDecode32( unsigned int n )
		{
			return (n >> 1) ^ -static_cast<int>(n & 1);
		}

		static inline unsigned long long zzEncode64( long long n )
		{
			// Note:  the right-shift must be arithmetic
			return (n << 1) ^ (n >> 63);
		}

		static inline long long zzDecode64( unsigned long long n )
		{
			return (n >> 1) ^ -static_cast<long long>(n & 1);
		}
	};

}

#endif // _SSUOBJECT_H_
