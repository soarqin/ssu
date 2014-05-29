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

#include <stdint.h>
#include "SsuReferred.h"
#include "SsuRepeated.h"

namespace ssu {

class Utils {
public:
    static inline size_t SizeInt32( int val ) {
        if(val < 0)
            return SizeUInt64(static_cast<uint64_t>(val));
        return SizeUInt32(static_cast<uint32_t>(val));
    }

    static inline size_t SizeSInt32( int val ) {
        return SizeUInt32(zzEncode32(val));
    }

    template<typename T>
    static inline size_t SizeEnum( T val ) {
        return SizeUInt32(static_cast<uint32_t>(val));
    }

    static inline size_t SizeUInt32( uint32_t val ) {
        if(val < (1 << 14)) {
            if(val < (1 << 7))
                return 1;
            else
                return 2;
        }
        else {
            if(val < (1 << 21))
                return 3;
            else if(val < (1 << 28))
                return 4;
            else return 5;
        }
    }

    static inline size_t SizeInt64( int64_t val ) {
        return SizeUInt64(static_cast<uint64_t>(val));
    }

    static inline size_t SizeSInt64( int64_t val ) {
        return SizeUInt64(zzEncode64(val));
    }

    static inline size_t SizeUInt64( uint64_t val ) {
        if(val < (1ull << 28)) {
            if(val < (1ull << 14)) {
                if(val < (1ull << 7))
                    return 1;
                else
                    return 2;
            }
            else {
                if(val < (1ull << 21))
                    return 3;
                else
                    return 4;
            }
        }
        else {
            if(val < (1ull << 42)) {
                if(val < (1ull << 35))
                    return 5;
                else
                    return 6;
            }
            else {
                if(val < (1ull << 56)) {
                    if(val < (1ull << 49))
                        return 7;
                    else
                        return 8;
                }
                else {
                    if(val < (1ull << 63))
                        return 9;
                    else
                        return 10;
                }
            }
        }
    }

    static inline size_t SizeFloat( float val ) {
        return sizeof(float);
    }

    static inline size_t SizeDouble( double val ) {
        return sizeof(double);
    }

    static inline size_t SizeFixed32( int64_t val ) {
        return 4;
    }

    static inline size_t SizeFixed64( int64_t val ) {
        return 8;
    }

    static inline size_t SizeBool( bool val ) {
        return 1;
    }

    static inline size_t SizeString( const std::string& val ) {
        uint32_t sz = static_cast<uint32_t>(val.size());
        return SizeUInt32(sz) + sz;
    }

    static inline size_t SizeStringPtr( const std::string* val ) {
        return SizeString(*val);
    }

    static inline size_t SizeBinary( const void* val, size_t len ) {
        return SizeUInt32(static_cast<uint32_t>(len)) + len;
    }

    template<typename T>
    static inline size_t SizeObject(const T* val) {
        size_t sz = val->Size();
        return SizeUInt32(static_cast<uint32_t>(sz)) + sz;
    }

    template<typename T>
    static inline size_t SizeReferred(ReferredObject<T>& val) {
        size_t sz = val.GetMutable()->Size();
        return SizeUInt32(static_cast<uint32_t>(sz)) + sz;
    }

    template<typename T, typename F>
    static inline size_t SizeRepeated(const RepeatedObject<T>& val, F func) {
        size_t sz = 0;
        typename RepeatedObject<T>::const_iterator iter = val.begin(), iter_end = val.end();
        while(iter != iter_end) {
            sz += func(*(iter ++));
        }
        return sz;
    }

    template<typename T, typename F>
    static inline size_t SizeRepeatedPacked(const RepeatedObject<T>& val, F func) {
        size_t sz = SizeRepeated(val, func);
        return sz + SizeUInt32(static_cast<uint32_t>(sz));
    }

    static inline uint8_t* PackInt32( uint8_t* buf, int val ) {
        if(val < 0)
            return PackUInt64(buf, static_cast<uint64_t>(val));
        else
            return PackUInt32(buf, static_cast<uint32_t>(val));
    }

    static inline uint8_t* PackSInt32( uint8_t* buf, int val ) {
        return PackUInt32(buf, zzEncode32(val));
    }

    template<typename T>
    static inline uint8_t* PackEnum( uint8_t* buf, T val ) {
        return PackUInt32(buf, static_cast<uint32_t>(val));
    }

    static inline uint8_t* PackUInt32( uint8_t* buf, uint32_t val ) {
        while(val >= 128) {
            *buf++ = static_cast<uint8_t>(val) | 0x80;
            val >>= 7;
        }
        *buf++ = static_cast<uint8_t>(val & 0x7F);
        return buf;
    }

    static inline uint8_t* PackInt64( uint8_t* buf, int64_t val ) {
        return PackUInt64(buf, static_cast<uint64_t>(val));
    }

    static inline uint8_t* PackSInt64( uint8_t* buf, int64_t val ) {
        return PackUInt64(buf, zzEncode64(val));
    }

    static inline uint8_t* PackUInt64( uint8_t* buf, uint64_t val ) {
        while(val >= 128ull) {
            *buf++ = static_cast<uint8_t>(val) | 0x80;
            val >>= 7;
        }
        *buf++ = static_cast<uint8_t>(val & 0x7F);
        return buf;
    }

    static inline uint8_t* PackFloat( uint8_t* buf, float val ) {
        return PackBinary(buf, &val, sizeof(float));
    }

    static inline uint8_t* PackDouble( uint8_t* buf, double val ) {
        return PackBinary(buf, &val, sizeof(double));
    }

    static inline uint8_t* PackFixed32( uint8_t* buf, int val ) {
        return PackBinary(buf, &val, 4);
    }

    static inline uint8_t* PackFixed64( uint8_t* buf, int64_t val ) {
        return PackBinary(buf, &val, 8);
    }

    static inline uint8_t* PackBool( uint8_t* buf, bool val ) {
        *buf = val ? 1 : 0;
        return buf + 1;
    }

    static inline uint8_t* PackString( uint8_t* buf, const std::string& val ) {
        size_t sz = val.size();
        memcpy(buf, val.c_str(), sz);
        return buf + sz;
    }

    static inline uint8_t* PackBinary( uint8_t* buf, const void* val, size_t len ) {
        memcpy(buf, val, len);
        return buf + len;
    }

    template<typename T>
    static inline uint8_t* PackObject(uint8_t* buf, T* val) {
        return val->PackBuffer(buf);
    }

    template<typename T>
    static inline uint8_t* PackReferred(uint8_t* buf, ReferredObject<T>& val) {
        return PackObject(buf, val.GetMutable());
    }

    template<typename T, typename F>
    static inline uint8_t* PackRepeated(uint8_t* buf, RepeatedObject<T>& val, F func) {
        typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
        while(iter != iter_end) {
            buf = func(buf, *(iter ++));
        }
        return buf;
    }

    static inline uint8_t* PackTag( uint8_t* buf, uint32_t id, uint8_t type ) {
        return PackUInt32(buf, (id << 3) + type);
    }

    static inline uint8_t* PackInt32Tag( uint8_t* buf, uint32_t id, int val ) {
        buf = PackTag(buf, id, 0);
        return PackInt32(buf, val);
    }

    static inline uint8_t* PackSInt32Tag( uint8_t* buf, uint32_t id, int val ) {
        buf = PackTag(buf, id, 0);
        return PackSInt32(buf, val);
    }

    static inline uint8_t* PackUInt32Tag( uint8_t* buf, uint32_t id, uint32_t val ) {
        buf = PackTag(buf, id, 0);
        return PackUInt32(buf, val);
    }

    template<typename T>
    static inline uint8_t* PackEnumTag( uint8_t* buf, uint32_t id, T val ) {
        buf = PackTag(buf, id, 0);
        return PackEnum(buf, val);
    }

    static inline uint8_t* PackInt64Tag( uint8_t* buf, uint32_t id, int64_t val ) {
        buf = PackTag(buf, id, 0);
        return PackInt64(buf, val);
    }

    static inline uint8_t* PackSInt64Tag( uint8_t* buf, uint32_t id, int64_t val ) {
        buf = PackTag(buf, id, 0);
        return PackSInt64(buf, val);
    }

    static inline uint8_t* PackUInt64Tag( uint8_t* buf, uint32_t id, uint64_t val ) {
        buf = PackTag(buf, id, 0);
        return PackUInt64(buf, val);
    }

    static inline uint8_t* PackFloatTag( uint8_t* buf, uint32_t id, float val ) {
        buf = PackTag(buf, id, 5);
        return PackFloat(buf, val);
    }

    static inline uint8_t* PackDoubleTag( uint8_t* buf, uint32_t id, double val ) {
        buf = PackTag(buf, id, 1);
        return PackDouble(buf, val);
    }

    static inline uint8_t* PackFixed32Tag( uint8_t* buf, uint32_t id, int val ) {
        buf = PackTag(buf, id, 5);
        return PackFixed32(buf, val);
    }

    static inline uint8_t* PackFixed32Tag( uint8_t* buf, uint32_t id, int64_t val ) {
        buf = PackTag(buf, id, 1);
        return PackFixed64(buf, val);
    }

    static inline uint8_t* PackBoolTag( uint8_t* buf, uint32_t id, bool val ) {
        buf = PackTag(buf, id, 0);
        return PackBool(buf, val);
    }

    static inline uint8_t* PackStringTag( uint8_t* buf, uint32_t id, const std::string& val ) {
        buf = PackTag(buf, id, 2);
        buf = PackUInt32(buf, static_cast<uint32_t>(val.size()));
        return PackString(buf, val);
    }

    static inline uint8_t* PackStringPtrTag( uint8_t* buf, uint32_t id, const std::string * val ) {
        buf = PackTag(buf, id, 2);
        buf = PackUInt32(buf, static_cast<uint32_t>(val->size()));
        return PackString(buf, *val);
    }

    static inline uint8_t* PackBinaryTag( uint8_t* buf, uint32_t id,    const void* val, size_t len ) {
        buf = PackTag(buf, id, 2);
        buf = PackUInt32(buf, static_cast<uint32_t>(len));
        return PackBinary(buf, val, len);
    }

    template<typename T>
    static inline uint8_t* PackObjectTag( uint8_t* buf, uint32_t id, T* val) {
        buf = PackTag(buf, id, 2);
        buf = PackUInt32(buf, static_cast<uint32_t>(val->Size()));
        return PackObject(buf, val);
    }

    template<typename T>
    static inline uint8_t* PackReferredTag( uint8_t* buf, uint32_t id, ReferredObject<T>& val) {
        return PackObjectTag(buf, id, val.GetMutable());
    }

    template<typename T, typename F>
    static inline uint8_t* PackRepeatedTag( uint8_t* buf, uint32_t id, RepeatedObject<T>& val, F func ) {
        typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
        while(iter != iter_end) {
            buf = func(buf, id, *(iter ++));
        }
        return buf;
    }

    template<typename T, typename F, typename SF>
    static inline uint8_t* PackRepeatedPackedTag( uint8_t* buf, uint32_t id, RepeatedObject<T>& val, F func, SF sizefunc) {
        buf = PackTag(buf, id, 2);
        buf = PackUInt32(buf, static_cast<uint32_t>(SizeRepeated(val, sizefunc)));
        typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
        while(iter != iter_end) {
            buf = func(buf, *(iter ++));
        }
        return buf;
    }

    static inline bool UnpackInt32( const uint8_t*& buf, size_t& leftSize, int& val ) {
        uint64_t n = 0;
        if(!UnpackUInt64(buf, leftSize, n))
            return false;
        val = static_cast<int>(n);
        return true;
    }

    static inline bool UnpackSInt32( const uint8_t*& buf, size_t& leftSize, int& val ) {
        uint32_t n = 0;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        val = zzDecode32(n);
        return true;
    }

    template<typename T>
    static inline bool UnpackEnum( const uint8_t*& buf, size_t& leftSize, T& val ) {
        uint32_t n = 0;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        val = static_cast<T>(n);
        return true;
    }

    static inline bool UnpackUInt32( const uint8_t*& buf, size_t& leftSize, uint32_t& val ) {
        if(leftSize == 0)
            return false;
        val = (*buf) & 0x7F;
        uint32_t shiftn = 7;
        while((*buf) >= 128) {
            if((-- leftSize) == 0) {
                ++ buf;
                return false;
            }
            val += static_cast<uint32_t>((*(++buf)) & 0x7F) << shiftn;
            shiftn += 7;
        }
        -- leftSize;
        ++ buf;
        return true;
    }

    static inline bool UnpackInt64( const uint8_t*& buf, size_t& leftSize, int64_t& val ) {
        return UnpackUInt64(buf, leftSize, reinterpret_cast<uint64_t&>(val));
    }

    static inline bool UnpackSInt64( const uint8_t*& buf, size_t& leftSize, int64_t& val ) {
        uint64_t n;
        if(!UnpackUInt64(buf, leftSize, n))
            return false;
        val = zzDecode64(n);
        return true;
    }

    static inline bool UnpackUInt64( const uint8_t*& buf, size_t& leftSize, uint64_t& val ) {
        if(leftSize == 0)
            return false;
        val = (*buf) & 0x7F;
        uint32_t shiftn = 7;
        while((*buf) >= 128) {
            if((-- leftSize) == 0) {
                ++ buf;
                return false;
            }
            val += static_cast<uint64_t>((*(++buf)) & 0x7F) << shiftn;
            shiftn += 7;
        }
        -- leftSize;
        ++ buf;
        return true;
    }

    static inline bool UnpackFloat( const uint8_t*& buf, size_t& leftSize, float& val ) {
        return UnpackBinary(buf, leftSize, &val, sizeof(float));
    }

    static inline bool UnpackDouble( const uint8_t*& buf, size_t& leftSize, double& val ) {
        return UnpackBinary(buf, leftSize, &val, sizeof(double));
    }

    static inline bool UnpackFixed32( const uint8_t*& buf, size_t& leftSize, int& val ) {
        val = 0;
        return UnpackBinary(buf, leftSize, &val, 4);
    }

    static inline bool UnpackFixed64( const uint8_t*& buf, size_t& leftSize, int64_t& val ) {
        val = 0;
        return UnpackBinary(buf, leftSize, &val, 8);
    }

    static inline bool UnpackBool( const uint8_t*& buf, size_t& leftSize, bool& val ) {
        if(leftSize == 0)
            return false;
        -- leftSize;
        val = (*buf++) != 0;
        return true;
    }

    static inline bool UnpackString( const uint8_t*& buf, size_t& leftSize, std::string& val ) {
        uint32_t length;
        if(!UnpackUInt32(buf, leftSize, length))
            return false;
        if(static_cast<size_t>(length) > leftSize)
            length = static_cast<uint32_t>(leftSize);
        val.resize(length);
        if(length > 0)
            memcpy(&val[0], buf, length);
        leftSize -= static_cast<size_t>(length);
        buf += length;
        return true;
    }

    static inline bool UnpackStringPtr( const uint8_t*& buf, size_t& leftSize, std::string*& val ) {
        uint32_t length;
        if(!UnpackUInt32(buf, leftSize, length))
            return false;
        if(static_cast<size_t>(length) > leftSize)
            length = static_cast<uint32_t>(leftSize);
        val = new std::string(reinterpret_cast<const char*>(buf), length);
        leftSize -= length;
        buf += length;
        return true;
    }

    static inline bool UnpackBinary( const uint8_t*& buf, size_t& leftSize, void* val, size_t len ) {
        if(len > leftSize)
            return false;
        if(len > 0)
            memcpy(val, buf, len);
        leftSize -= len;
        buf += len;
        return true;
    }

    template<typename T>
    static inline bool UnpackObject(const uint8_t*& buf, size_t& leftSize, T* val) {
        uint32_t n;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        const uint8_t* thisbuf = buf;
        size_t lsize = n;
        if(!val->UnpackBuffer(thisbuf, lsize))
            return false;
        buf += n;
        leftSize -= n;
        return true;
    }

    template<typename T>
    static inline bool UnpackObjectPtr(const uint8_t*& buf, size_t& leftSize, T*& val) {
        val = new(std::nothrow) T;
        if(val == NULL) return false;

        if(!val->UnpackBuffer(buf, leftSize)) {
            delete val;
            return false;
        }
        return true;
    }

    template<typename T>
    static inline bool UnpackReferred(const uint8_t*& buf, size_t& leftSize, ReferredObject<T>& val) {
        return UnpackObject(buf, leftSize, val.GetMutable());
    }

    template<typename T, typename F>
    static inline bool UnpackRepeated(const uint8_t*& buf, size_t& leftSize, RepeatedObject<T>& val, F func) {
        T singleVal;
        if(!func(buf, leftSize, singleVal))
            return false;
        val.Add(singleVal);
        return true;
    }

    template<typename T, typename F>
    static inline bool UnpackRepeatedPacked(const uint8_t*& buf, size_t& leftSize, RepeatedObject<T>& val, F func) {
        uint32_t n;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        const uint8_t* thisbuf = buf;
        size_t lsize = n;
        while(lsize > 0) {
            T singleVal;
            if(!func(thisbuf, lsize, singleVal))
                return false;
            val.Add(singleVal);
        }
        buf += n;
        leftSize -= n;
        return true;
    }

    template<typename T, typename F>
    static inline bool UnpackRepeatedPtr(const uint8_t*& buf, size_t& leftSize, RepeatedObject<T>& val, F func) {
        uint32_t n;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        const uint8_t* thisbuf = buf;
        size_t nsize = n;
        T singleVal;
        if(!func(thisbuf, nsize, singleVal))
            return false;
        val.Add(singleVal);
        buf += n;
        leftSize -= n;
        return true;
    }

    template<typename T, typename F>
    static inline bool UnpackRepeatedPackedPtr(const uint8_t*& buf, size_t& leftSize, RepeatedObject<T>& val, F func) {
        uint32_t n;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        const uint8_t* thisbuf = buf;
        size_t nsize = n;
        while(nsize > 0) {
            T singleVal;
            if(!func(thisbuf, nsize, singleVal))
                return false;
            val.Add(singleVal);
        }
        buf += n;
        leftSize -= n;
        return true;
    }

    static inline bool UnpackTag(const uint8_t*& buf, size_t& leftSize, uint32_t& tag, uint8_t& type) {
        uint32_t n;
        if(!UnpackUInt32(buf, leftSize, n))
            return false;
        tag = n >> 3;
        type = n & 7;
        return true;
    }

private:
    static inline uint32_t zzEncode32( int n ) {
        // Note:    the right-shift must be arithmetic
        return (n << 1) ^ (n >> 31);
    }

    static inline int zzDecode32( uint32_t n ) {
        return (n >> 1) ^ -static_cast<int>(n & 1);
    }

    static inline uint64_t zzEncode64( int64_t n ) {
        // Note:    the right-shift must be arithmetic
        return (n << 1) ^ (n >> 63);
    }

    static inline int64_t zzDecode64( uint64_t n ) {
        return (n >> 1) ^ -static_cast<int64_t>(n & 1);
    }
};

}

#endif // _SSUUTILS_H_
