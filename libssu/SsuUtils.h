#ifndef _SSUUTILS_H_
#define _SSUUTILS_H_

#include "ReferredObject.h"

namespace ssu
{

	class Utils
	{
	public:
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
			return sizeUInt32(val.size()) + val.size();
		}

		static inline size_t sizeVector( const std::vector<unsigned char>& val )
		{
			return sizeUInt32(val.size()) + val.size();
		}

		static inline size_t sizeBinary( const void * val, size_t len )
		{
			return sizeUInt32(static_cast<unsigned int>(len)) + len;
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
			size_t sz = val.getMutable()->size();
			return sizeUInt32(sz) + sz;
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

		static inline unsigned char * packInt32( unsigned char * buf, int val )
		{
			if(val < 0)
				return packUInt64(buf, static_cast<unsigned long long>(val));
			else
				return packUInt32(buf, static_cast<unsigned int>(val));
		}

		static inline unsigned char * packSInt32( unsigned char * buf, int val )
		{
			return packUInt32(buf, zzEncode32(val));
		}

		static inline unsigned char * packUInt32( unsigned char * buf, unsigned int val )
		{
			while(val >= 128)
			{
				*buf++ = static_cast<unsigned char>(val) | 0x80;
				val >>= 7;
			}
			*buf++ = static_cast<unsigned char>(val & 0x7F);
			return buf;
		}

		static inline unsigned char * packInt64( unsigned char * buf, long long val )
		{
			return packUInt64(buf, static_cast<unsigned long long>(val));
		}

		static inline unsigned char * packSInt64( unsigned char * buf, long long val )
		{
			return packUInt64(buf, zzEncode64(val));
		}

		static inline unsigned char * packUInt64( unsigned char * buf, unsigned long long val )
		{
			while(val >= 128ull)
			{
				*buf++ = static_cast<unsigned char>(val) | 0x80;
				val >>= 7;
			}
			*buf++ = static_cast<unsigned char>(val & 0x7F);
			return buf;
		}

		static inline unsigned char * packFloat( unsigned char * buf, float val )
		{
			return packBinary(buf, &val, sizeof(float));
		}

		static inline unsigned char * packDouble( unsigned char * buf, double val )
		{
			return packBinary(buf, &val, sizeof(double));
		}

		static inline unsigned char * packBool( unsigned char * buf, bool val )
		{
			*buf = val ? 1 : 0;
			return buf + 1;
		}

		static inline unsigned char * packString( unsigned char * buf, const std::string& val )
		{
			size_t sz = val.size();
			memcpy(buf, val.c_str(), sz);
			return buf + sz;
		}

		static inline unsigned char * packVector( unsigned char * buf, const std::vector<unsigned char>& val )
		{
			size_t sz = val.size();
			memcpy(buf, &val[0], sz);
			return buf + sz;
		}

		static inline unsigned char * packBinary( unsigned char * buf, const void * val, size_t len )
		{
			memcpy(buf, val, len);
			return buf + len;
		}

		template<typename T>
		static inline unsigned char * packObject(unsigned char * buf, T * val)
		{
			return val->packBuffer(buf);
		}

		template<typename T>
		static inline unsigned char * packReferred(unsigned char * buf, ReferredObject<T>& val)
		{
			return packObject(buf, val.getMutable());
		}

		template<typename T, typename F>
		static inline unsigned char * packRepeated(unsigned char * buf, std::vector<T>& val, F func)
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				buf = func(buf, *iter);
			}
			return buf;
		}

		static inline unsigned char * packTag( unsigned char * buf, unsigned int id, unsigned char type )
		{
			return packUInt32(buf, (id << 3) + type);
		}

		static inline unsigned char * packInt32Tag( unsigned char * buf, unsigned int id, int val )
		{
			buf = packTag(buf, id, 0);
			return packInt32(buf, val);
		}

		static inline unsigned char * packSInt32Tag( unsigned char * buf, unsigned int id, int val )
		{
			buf = packTag(buf, id, 0);
			return packSInt32(buf, val);
		}

		static inline unsigned char * packUInt32Tag( unsigned char * buf, unsigned int id, unsigned int val )
		{
			buf = packTag(buf, id, 0);
			return packUInt32(buf, val);
		}

		static inline unsigned char * packInt64Tag( unsigned char * buf, unsigned int id, long long val )
		{
			buf = packTag(buf, id, 0);
			return packInt64(buf, val);
		}

		static inline unsigned char * packSInt64Tag( unsigned char * buf, unsigned int id, long long val )
		{
			buf = packTag(buf, id, 0);
			return packSInt64(buf, val);
		}

		static inline unsigned char * packUInt64Tag( unsigned char * buf, unsigned int id, unsigned long long val )
		{
			buf = packTag(buf, id, 0);
			return packUInt64(buf, val);
		}

		static inline unsigned char * packFloatTag( unsigned char * buf, unsigned int id, float val )
		{
			buf = packTag(buf, id, 5);
			return packFloat(buf, val);
		}

		static inline unsigned char * packDoubleTag( unsigned char * buf, unsigned int id, double val )
		{
			buf = packTag(buf, id, 1);
			return packDouble(buf, val);
		}

		static inline unsigned char * packBoolTag( unsigned char * buf, unsigned int id, bool val )
		{
			buf = packTag(buf, id, 0);
			return packBool(buf, val);
		}

		static inline unsigned char * packStringTag( unsigned char * buf, unsigned int id, const std::string& val )
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, val.size());
			return packString(buf, val);
		}

		static inline unsigned char * packVectorTag( unsigned char * buf, unsigned int id, const std::vector<unsigned char>& val )
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, val.size());
			return packVector(buf, val);
		}

		static inline unsigned char * packBinaryTag( unsigned char * buf, unsigned int id,  const void * val, size_t len )
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, len);
			return packBinary(buf, val, len);
		}

		template<typename T>
		static inline unsigned char * packObjectTag( unsigned char * buf, unsigned int id, T * val)
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, static_cast<unsigned int>(val->size()));
			return packObject(buf, val);
		}

		template<typename T>
		static inline unsigned char * packReferredTag( unsigned char * buf, unsigned int id, ReferredObject<T>& val)
		{
			return packObjectTag(buf, id, val.getMutable());
		}

		template<typename T, typename F>
		static inline unsigned char * packRepeatedTag( unsigned char * buf, unsigned int id, std::vector<T>& val, F func )
		{
			for(std::vector<T>::iterator iter = val.begin(); iter != val.end(); ++ iter)
			{
				buf = func(buf, id, *iter);
			}
			return buf;
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

#endif // _SSUUTILS_H_
