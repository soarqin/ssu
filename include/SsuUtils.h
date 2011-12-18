/*
 * Copyright (c) 2011, Soar Qin<soarchin@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SSUUTILS_H_
#define _SSUUTILS_H_

#include "SsuReferred.h"
#include "SsuRepeated.h"

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

		template<typename T>
		static inline size_t sizeEnum( T val )
		{
			return sizeUInt32(static_cast<unsigned int>(val));
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

		static inline size_t sizeStringPtr( const std::string * val )
		{
			return sizeUInt32(val->size()) + val->size();
		}

		static inline size_t sizeBinary( const void * val, size_t len )
		{
			return sizeUInt32(static_cast<unsigned int>(len)) + len;
		}

		template<typename T>
		static inline size_t sizeObject(const T * val)
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
		static inline size_t sizeRepeated(const RepeatedObject<T>& val, F func)
		{
			size_t sz = 0;
			typename RepeatedObject<T>::const_iterator iter = val.begin(), iter_end = val.end();
			while(iter != iter_end)
			{
				sz += func(*(iter ++));
			}
			return sz;
		}

		template<typename T, typename F>
		static inline size_t sizeRepeatedPacked(const RepeatedObject<T>& val, F func)
		{
			size_t sz = sizeRepeated(val, func);
			return sz + sizeUInt32(sz);
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

		template<typename T>
		static inline unsigned char * packEnum( unsigned char * buf, T val )
		{
			return packUInt32(buf, static_cast<unsigned int>(val));
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
		static inline unsigned char * packRepeated(unsigned char * buf, RepeatedObject<T>& val, F func)
		{
			typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
			while(iter != iter_end)
			{
				buf = func(buf, *(iter ++));
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

		template<typename T>
		static inline unsigned char * packEnumTag( unsigned char * buf, unsigned int id, T val )
		{
			buf = packTag(buf, id, 0);
			return packEnum(buf, val);
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

		static inline unsigned char * packStringPtrTag( unsigned char * buf, unsigned int id, const std::string * val )
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, val->size());
			return packString(buf, *val);
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
		static inline unsigned char * packRepeatedTag( unsigned char * buf, unsigned int id, RepeatedObject<T>& val, F func )
		{
			typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
			while(iter != iter_end)
			{
				buf = func(buf, id, *(iter ++));
			}
			return buf;
		}

		template<typename T, typename F, typename SF>
		static inline unsigned char * packRepeatedPackedTag( unsigned char * buf, unsigned int id, RepeatedObject<T>& val, F func, SF sizefunc)
		{
			buf = packTag(buf, id, 2);
			buf = packUInt32(buf, sizeRepeated(val, sizefunc));
			typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
			while(iter != iter_end)
			{
				buf = func(buf, *(iter ++));
			}
			return buf;
		}

		static inline bool unpackInt32( const unsigned char *& buf, size_t& leftSize, int& val )
		{
			unsigned long long n = 0;
			if(!unpackUInt64(buf, leftSize, n))
				return false;
			val = static_cast<int>(n);
			return true;
		}

		static inline bool unpackSInt32( const unsigned char *& buf, size_t& leftSize, int& val )
		{
			unsigned int n = 0;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			val = zzDecode32(n);
			return true;
		}

		template<typename T>
		static inline bool unpackEnum( const unsigned char *& buf, size_t& leftSize, T& val )
		{
			unsigned int n = 0;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			val = static_cast<T>(n);
			return true;
		}

		static inline bool unpackUInt32( const unsigned char *& buf, size_t& leftSize, unsigned int& val )
		{
			if(leftSize == 0)
				return false;
			val = (*buf) & 0x7F;
			unsigned int shiftn = 7;
			while((*buf) >= 128)
			{
				if((-- leftSize) == 0)
				{
					++ buf;
					return false;
				}
				val += static_cast<unsigned int>((*(++buf)) & 0x7F) << shiftn;
				shiftn += 7;
			}
			-- leftSize;
			++ buf;
			return true;
		}

		static inline bool unpackInt64( const unsigned char *& buf, size_t& leftSize, long long& val )
		{
			return unpackUInt64(buf, leftSize, reinterpret_cast<unsigned long long&>(val));
		}

		static inline bool unpackSInt64( const unsigned char *& buf, size_t& leftSize, long long& val )
		{
			unsigned long long n;
			if(!unpackUInt64(buf, leftSize, n))
				return false;
			val = zzDecode64(n);
			return true;
		}

		static inline bool unpackUInt64( const unsigned char *& buf, size_t& leftSize, unsigned long long& val )
		{
			if(leftSize == 0)
				return false;
			val = (*buf) & 0x7F;
			-- leftSize;
			unsigned int shiftn = 7;
			while((*buf) >= 128)
			{
				if((-- leftSize) == 0)
				{
					++ buf;
					return false;
				}
				val += static_cast<unsigned long long>((*(++buf)) & 0x7F) << shiftn;
				shiftn += 7;
			}
			-- leftSize;
			++ buf;
			return true;
		}

		static inline bool unpackFloat( const unsigned char *& buf, size_t& leftSize, float& val )
		{
			return unpackBinary(buf, leftSize, &val, sizeof(float));
		}

		static inline bool unpackDouble( const unsigned char *& buf, size_t& leftSize, double& val )
		{
			return unpackBinary(buf, leftSize, &val, sizeof(double));
		}

		static inline bool unpackBool( const unsigned char *& buf, size_t& leftSize, bool& val )
		{
			if(leftSize == 0)
				return false;
			-- leftSize;
			val = (*buf++) != 0;
			return true;
		}

		static inline bool unpackString( const unsigned char *& buf, size_t& leftSize, std::string& val )
		{
			unsigned int length;
			if(!unpackUInt32(buf, leftSize, length))
				return false;
			if(length > leftSize)
				length = leftSize;
			val.resize(length);
			if(length > 0)
				memcpy(&val[0], buf, length);
			leftSize -= length;
			buf += length;
			return true;
		}

		static inline bool unpackStringPtr( const unsigned char *& buf, size_t& leftSize, std::string *& val )
		{
			unsigned int length;
			if(!unpackUInt32(buf, leftSize, length))
				return false;
			if(length > leftSize)
				length = leftSize;
			val = new std::string(reinterpret_cast<const char *>(buf), length);
			leftSize -= length;
			buf += length;
			return true;
		}

		static inline bool unpackBinary( const unsigned char *& buf, size_t& leftSize, void * val, size_t len )
		{
			if(len > leftSize)
				return false;
			if(len > 0)
				memcpy(val, buf, len);
			leftSize -= len;
			buf += len;
			return true;
		}

		template<typename T>
		static inline bool unpackObject(const unsigned char *& buf, size_t& leftSize, T * val)
		{
			unsigned int n;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			const unsigned char * thisbuf = buf;
			size_t lsize = n;
			if(!val->unpackBuffer(thisbuf, lsize))
				return false;
			buf += n;
			leftSize -= n;
			return true;
		}

		template<typename T>
		static inline bool unpackObjectPtr(const unsigned char *& buf, size_t& leftSize, T *& val)
		{
			val = new(std::nothrow) T;
			if(val == NULL) return false;

			if(!val->unpackBuffer(buf, leftSize))
			{
				delete val;
				return false;
			}
			return true;
		}

		template<typename T>
		static inline bool unpackReferred(const unsigned char *& buf, size_t& leftSize, ReferredObject<T>& val)
		{
			return unpackObject(buf, leftSize, val.getMutable());
		}

		template<typename T, typename F>
		static inline bool unpackRepeated(const unsigned char *& buf, size_t& leftSize, RepeatedObject<T>& val, F func)
		{
			T singleVal;
			if(!func(buf, leftSize, singleVal))
				return false;
			val.add(singleVal);
			return true;
		}

		template<typename T, typename F>
		static inline bool unpackRepeatedPacked(const unsigned char *& buf, size_t& leftSize, RepeatedObject<T>& val, F func)
		{
			unsigned int n;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			const unsigned char * thisbuf = buf;
			size_t lsize = n;
			while(lsize > 0)
			{
				T singleVal;
				if(!func(thisbuf, lsize, singleVal))
					return false;
				val.add(singleVal);
			}
			buf += n;
			leftSize -= n;
			return true;
		}

		template<typename T, typename F>
		static inline bool unpackRepeatedPtr(const unsigned char *& buf, size_t& leftSize, RepeatedObject<T>& val, F func)
		{
			unsigned int n;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			const unsigned char * thisbuf = buf;
			size_t nsize = n;
			T singleVal;
			if(!func(thisbuf, nsize, singleVal))
				return false;
			val.add(singleVal);
			buf += n;
			leftSize -= n;
			return true;
		}

		template<typename T, typename F>
		static inline bool unpackRepeatedPackedPtr(const unsigned char *& buf, size_t& leftSize, RepeatedObject<T>& val, F func)
		{
			unsigned int n;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			const unsigned char * thisbuf = buf;
			size_t nsize = n;
			while(nsize > 0)
			{
				T singleVal;
				if(!func(thisbuf, nsize, singleVal))
					return false;
				val.add(singleVal);
			}
			buf += n;
			leftSize -= n;
			return true;
		}

		static inline bool unpackTag(const unsigned char *& buf, size_t& leftSize, unsigned int& tag, unsigned char& type)
		{
			unsigned int n;
			if(!unpackUInt32(buf, leftSize, n))
				return false;
			tag = n >> 3;
			type = n & 7;
			return true;
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
