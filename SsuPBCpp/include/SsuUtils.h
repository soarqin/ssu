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
namespace utils {

namespace {
inline uint32_t zzEncode32( int32_t n ) {
    // Note:    the right-shift must be arithmetic
    return (n << 1) ^ (n >> 31);
}

inline int32_t zzDecode32( uint32_t n ) {
    return (n >> 1) ^ -static_cast<int32_t>(n & 1);
}

inline uint64_t zzEncode64( int64_t n ) {
    // Note:    the right-shift must be arithmetic
    return (n << 1) ^ (n >> 63);
}

inline int64_t zzDecode64( uint64_t n ) {
    return (n >> 1) ^ -static_cast<int64_t>(n & 1);
}
}

inline size_t sizeUInt32( uint32_t val ) {
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

inline size_t sizeUInt64( uint64_t val ) {
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

inline size_t sizeInt32( int32_t val ) {
    if(val < 0)
        return sizeUInt64(static_cast<uint64_t>(val));
    return sizeUInt32(static_cast<uint32_t>(val));
}

inline size_t sizeSInt32( int32_t val ) {
    return sizeUInt32(zzEncode32(val));
}

template<typename T>
inline size_t sizeEnum( T val ) {
    return sizeUInt32(static_cast<uint32_t>(val));
}

inline size_t sizeInt64( int64_t val ) {
    return sizeUInt64(static_cast<uint64_t>(val));
}

inline size_t sizeSInt64( int64_t val ) {
    return sizeUInt64(zzEncode64(val));
}

inline size_t sizeFloat( float val ) {
    return sizeof(float);
}

inline size_t sizeDouble( double val ) {
    return sizeof(double);
}

inline size_t sizeFixed32( uint32_t val ) {
    return 4;
}

inline size_t sizeFixed64( uint64_t val ) {
    return 8;
}

inline size_t sizeSFixed32( int32_t val ) {
    return 4;
}

inline size_t sizeSFixed64( int64_t val ) {
    return 8;
}

inline size_t sizeBool( bool val ) {
    return 1;
}

inline size_t sizeString( const std::string &val ) {
    uint32_t sz = static_cast<uint32_t>(val.size());
    return sizeUInt32(sz) + sz;
}

inline size_t sizeStringPtr( const std::string* val ) {
    return sizeString(*val);
}

inline size_t sizeBinary( const void* val, size_t len ) {
    return sizeUInt32(static_cast<uint32_t>(len)) + len;
}

template<typename T>
inline size_t sizeObject(const T* val) {
    size_t sz = val->size();
    return sizeUInt32(static_cast<uint32_t>(sz)) + sz;
}

template<typename T>
inline size_t sizeReferred(ReferredObject<T>& val) {
    size_t sz = val.getMutable()->size();
    return sizeUInt32(static_cast<uint32_t>(sz)) + sz;
}

template<typename T, typename F>
inline size_t sizeRepeated(const RepeatedObject<T>& val, F func) {
    size_t sz = 0;
    typename RepeatedObject<T>::const_iterator iter = val.begin(), iter_end = val.end();
    while(iter != iter_end) {
        sz += func(*(iter ++));
    }
    return sz;
}

template<typename T, typename F>
inline size_t sizeRepeatedPacked(const RepeatedObject<T>& val, F func) {
    size_t sz = sizeRepeated(val, func);
    return sz + sizeUInt32(static_cast<uint32_t>(sz));
}

inline uint8_t *packUInt32( uint8_t *buf, uint32_t val ) {
    while(val >= 128) {
        *buf++ = static_cast<uint8_t>(val) | 0x80;
        val >>= 7;
    }
    *buf++ = static_cast<uint8_t>(val & 0x7F);
    return buf;
}

inline uint8_t *packBinary( uint8_t *buf, const void* val, size_t len ) {
    memcpy(buf, val, len);
    return buf + len;
}

inline uint8_t *packUInt64( uint8_t *buf, uint64_t val ) {
    while(val >= 128ull) {
        *buf++ = static_cast<uint8_t>(val) | 0x80;
        val >>= 7;
    }
    *buf++ = static_cast<uint8_t>(val & 0x7F);
    return buf;
}

inline uint8_t *packInt32( uint8_t *buf, int32_t val ) {
    if(val < 0)
        return packUInt64(buf, static_cast<uint64_t>(val));
    else
        return packUInt32(buf, static_cast<uint32_t>(val));
}

inline uint8_t *packSInt32( uint8_t *buf, int32_t val ) {
    return packUInt32(buf, zzEncode32(val));
}

inline uint8_t *packInt64( uint8_t *buf, int64_t val ) {
    return packUInt64(buf, static_cast<uint64_t>(val));
}

inline uint8_t *packSInt64( uint8_t *buf, int64_t val ) {
    return packUInt64(buf, zzEncode64(val));
}

template<typename T>
inline uint8_t *packEnum( uint8_t *buf, T val ) {
    return packUInt32(buf, static_cast<uint32_t>(val));
}

inline uint8_t *packFloat( uint8_t *buf, float val ) {
    return packBinary(buf, &val, sizeof(float));
}

inline uint8_t *packDouble( uint8_t *buf, double val ) {
    return packBinary(buf, &val, sizeof(double));
}

inline uint8_t *packFixed32( uint8_t *buf, uint32_t val ) {
    return packBinary(buf, &val, 4);
}

inline uint8_t *packFixed64( uint8_t *buf, uint64_t val ) {
    return packBinary(buf, &val, 8);
}

inline uint8_t *packSFixed32( uint8_t *buf, int32_t val ) {
    return packBinary(buf, &val, 4);
}

inline uint8_t *packSFixed64( uint8_t *buf, int64_t val ) {
    return packBinary(buf, &val, 8);
}

inline uint8_t *packBool( uint8_t *buf, bool val ) {
    *buf = val ? 1 : 0;
    return buf + 1;
}

inline uint8_t *packString( uint8_t *buf, const std::string &val ) {
    size_t sz = val.size();
    memcpy(buf, val.c_str(), sz);
    return buf + sz;
}

template<typename T>
inline uint8_t *packObject(uint8_t *buf, T* val) {
    return val->packBuffer(buf);
}

template<typename T>
inline uint8_t *packReferred(uint8_t *buf, ReferredObject<T>& val) {
    return packObject(buf, val.getMutable());
}

template<typename T, typename F>
inline uint8_t *packRepeated(uint8_t *buf, RepeatedObject<T>& val, F func) {
    typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
    while(iter != iter_end) {
        buf = func(buf, *(iter ++));
    }
    return buf;
}

inline uint8_t *packTag( uint8_t *buf, uint32_t id, uint8_t type ) {
    return packUInt32(buf, (id << 3) + type);
}

inline uint8_t *packInt32Tag( uint8_t *buf, uint32_t id, int32_t val ) {
    buf = packTag(buf, id, 0);
    return packInt32(buf, val);
}

inline uint8_t *packSInt32Tag( uint8_t *buf, uint32_t id, int32_t val ) {
    buf = packTag(buf, id, 0);
    return packSInt32(buf, val);
}

inline uint8_t *packUInt32Tag( uint8_t *buf, uint32_t id, uint32_t val ) {
    buf = packTag(buf, id, 0);
    return packUInt32(buf, val);
}

template<typename T>
inline uint8_t *packEnumTag( uint8_t *buf, uint32_t id, T val ) {
    buf = packTag(buf, id, 0);
    return packEnum(buf, val);
}

inline uint8_t *packInt64Tag( uint8_t *buf, uint32_t id, int64_t val ) {
    buf = packTag(buf, id, 0);
    return packInt64(buf, val);
}

inline uint8_t *packSInt64Tag( uint8_t *buf, uint32_t id, int64_t val ) {
    buf = packTag(buf, id, 0);
    return packSInt64(buf, val);
}

inline uint8_t *packUInt64Tag( uint8_t *buf, uint32_t id, uint64_t val ) {
    buf = packTag(buf, id, 0);
    return packUInt64(buf, val);
}

inline uint8_t *packFloatTag( uint8_t *buf, uint32_t id, float val ) {
    buf = packTag(buf, id, 5);
    return packFloat(buf, val);
}

inline uint8_t *packDoubleTag( uint8_t *buf, uint32_t id, double val ) {
    buf = packTag(buf, id, 1);
    return packDouble(buf, val);
}

inline uint8_t *packFixed32Tag( uint8_t *buf, uint32_t id, uint32_t val ) {
    buf = packTag(buf, id, 5);
    return packFixed32(buf, val);
}

inline uint8_t *packFixed64Tag( uint8_t *buf, uint32_t id, uint64_t val ) {
    buf = packTag(buf, id, 1);
    return packFixed64(buf, val);
}

inline uint8_t *packSFixed32Tag( uint8_t *buf, uint32_t id, int32_t val ) {
    buf = packTag(buf, id, 5);
    return packFixed32(buf, val);
}

inline uint8_t *packSFixed64Tag( uint8_t *buf, uint32_t id, int64_t val ) {
    buf = packTag(buf, id, 1);
    return packFixed64(buf, val);
}

inline uint8_t *packBoolTag( uint8_t *buf, uint32_t id, bool val ) {
    buf = packTag(buf, id, 0);
    return packBool(buf, val);
}

inline uint8_t *packStringTag( uint8_t *buf, uint32_t id, const std::string &val ) {
    buf = packTag(buf, id, 2);
    buf = packUInt32(buf, static_cast<uint32_t>(val.size()));
    return packString(buf, val);
}

inline uint8_t *packStringPtrTag( uint8_t *buf, uint32_t id, const std::string *val ) {
    buf = packTag(buf, id, 2);
    buf = packUInt32(buf, static_cast<uint32_t>(val->size()));
    return packString(buf, *val);
}

inline uint8_t *packBinaryTag( uint8_t *buf, uint32_t id,    const void* val, size_t len ) {
    buf = packTag(buf, id, 2);
    buf = packUInt32(buf, static_cast<uint32_t>(len));
    return packBinary(buf, val, len);
}

template<typename T>
inline uint8_t *packObjectTag( uint8_t *buf, uint32_t id, T* val) {
    buf = packTag(buf, id, 2);
    buf = packUInt32(buf, static_cast<uint32_t>(val->size()));
    return packObject(buf, val);
}

template<typename T>
inline uint8_t *packReferredTag( uint8_t *buf, uint32_t id, ReferredObject<T>& val) {
    return packObjectTag(buf, id, val.getMutable());
}

template<typename T, typename F>
inline uint8_t *packRepeatedTag( uint8_t *buf, uint32_t id, RepeatedObject<T>& val, F func ) {
    typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
    while(iter != iter_end) {
        buf = func(buf, id, *(iter ++));
    }
    return buf;
}

template<typename T, typename F, typename SF>
inline uint8_t *packRepeatedPackedTag( uint8_t *buf, uint32_t id, RepeatedObject<T>& val, F func, SF sizefunc) {
    buf = packTag(buf, id, 2);
    buf = packUInt32(buf, static_cast<uint32_t>(sizeRepeated(val, sizefunc)));
    typename RepeatedObject<T>::iterator iter = val.begin(), iter_end = val.end();
    while(iter != iter_end) {
        buf = func(buf, *(iter ++));
    }
    return buf;
}

inline bool unpackBinary( const uint8_t *&buf, size_t &leftSize, void* val, size_t len ) {
    if(len > leftSize)
        return false;
    if(len > 0)
        memcpy(val, buf, len);
    leftSize -= len;
    buf += len;
    return true;
}

inline bool unpackUInt32( const uint8_t *&buf, size_t &leftSize, uint32_t& val ) {
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

inline bool unpackUInt64( const uint8_t *&buf, size_t &leftSize, uint64_t& val ) {
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

inline bool unpackInt32( const uint8_t *&buf, size_t &leftSize, int32_t& val ) {
    uint64_t n = 0;
    if(!unpackUInt64(buf, leftSize, n))
        return false;
    val = static_cast<int32_t>(n);
    return true;
}

inline bool unpackSInt32( const uint8_t *&buf, size_t &leftSize, int32_t& val ) {
    uint32_t n = 0;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    val = zzDecode32(n);
    return true;
}

inline bool unpackInt64( const uint8_t *&buf, size_t &leftSize, int64_t& val ) {
    return unpackUInt64(buf, leftSize, reinterpret_cast<uint64_t&>(val));
}

inline bool unpackSInt64( const uint8_t *&buf, size_t &leftSize, int64_t& val ) {
    uint64_t n;
    if(!unpackUInt64(buf, leftSize, n))
        return false;
    val = zzDecode64(n);
    return true;
}

template<typename T>
inline bool unpackEnum( const uint8_t *&buf, size_t &leftSize, T& val ) {
    uint32_t n = 0;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    val = static_cast<T>(n);
    return true;
}

inline bool unpackFloat( const uint8_t *&buf, size_t &leftSize, float& val ) {
    return unpackBinary(buf, leftSize, &val, sizeof(float));
}

inline bool unpackDouble( const uint8_t *&buf, size_t &leftSize, double& val ) {
    return unpackBinary(buf, leftSize, &val, sizeof(double));
}

inline bool unpackFixed32( const uint8_t *&buf, size_t &leftSize, uint32_t& val ) {
    val = 0;
    return unpackBinary(buf, leftSize, &val, 4);
}

inline bool unpackFixed64( const uint8_t *&buf, size_t &leftSize, uint64_t& val ) {
    val = 0;
    return unpackBinary(buf, leftSize, &val, 8);
}

inline bool unpackSFixed32( const uint8_t *&buf, size_t &leftSize, int32_t& val ) {
    val = 0;
    return unpackBinary(buf, leftSize, &val, 4);
}

inline bool unpackSFixed64( const uint8_t *&buf, size_t &leftSize, int64_t& val ) {
    val = 0;
    return unpackBinary(buf, leftSize, &val, 8);
}

inline bool unpackBool( const uint8_t *&buf, size_t &leftSize, bool& val ) {
    if(leftSize == 0)
        return false;
    -- leftSize;
    val = (*buf++) != 0;
    return true;
}

inline bool unpackString( const uint8_t *&buf, size_t &leftSize, std::string &val ) {
    uint32_t length;
    if(!unpackUInt32(buf, leftSize, length))
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

inline bool unpackStringPtr( const uint8_t *&buf, size_t &leftSize, std::string*& val ) {
    uint32_t length;
    if(!unpackUInt32(buf, leftSize, length))
        return false;
    if(static_cast<size_t>(length) > leftSize)
        length = static_cast<uint32_t>(leftSize);
    val = new std::string(reinterpret_cast<const char*>(buf), length);
    leftSize -= length;
    buf += length;
    return true;
}

template<typename T>
inline bool unpackObject(const uint8_t *&buf, size_t &leftSize, T* val) {
    uint32_t n;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    const uint8_t *thisbuf = buf;
    size_t lsize = n;
    if(!val->unpackBuffer(thisbuf, lsize))
        return false;
    buf += n;
    leftSize -= n;
    return true;
}

template<typename T>
inline bool unpackObjectPtr(const uint8_t *&buf, size_t &leftSize, T*& val) {
    val = new(std::nothrow) T;
    if(val == NULL) return false;

    if(!val->unpackBuffer(buf, leftSize)) {
        delete val;
        return false;
    }
    return true;
}

template<typename T>
inline bool unpackReferred(const uint8_t *&buf, size_t &leftSize, ReferredObject<T>& val) {
    return unpackObject(buf, leftSize, val.getMutable());
}

template<typename T, typename F>
inline bool unpackRepeated(const uint8_t *&buf, size_t &leftSize, RepeatedObject<T>& val, F func) {
    T singleVal;
    if(!func(buf, leftSize, singleVal))
        return false;
    val.add(singleVal);
    return true;
}

template<typename T, typename F>
inline bool unpackRepeatedPacked(const uint8_t *&buf, size_t &leftSize, RepeatedObject<T>& val, F func) {
    uint32_t n;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    const uint8_t *thisbuf = buf;
    size_t lsize = n;
    while(lsize > 0) {
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
inline bool unpackRepeatedPtr(const uint8_t *&buf, size_t &leftSize, RepeatedObject<T>& val, F func) {
    uint32_t n;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    const uint8_t *thisbuf = buf;
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
inline bool unpackRepeatedPackedPtr(const uint8_t *&buf, size_t &leftSize, RepeatedObject<T>& val, F func) {
    uint32_t n;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    const uint8_t *thisbuf = buf;
    size_t nsize = n;
    while(nsize > 0) {
        T singleVal;
        if(!func(thisbuf, nsize, singleVal))
            return false;
        val.add(singleVal);
    }
    buf += n;
    leftSize -= n;
    return true;
}

inline bool unpackTag(const uint8_t *&buf, size_t &leftSize, uint32_t& tag, uint8_t& type) {
    uint32_t n;
    if(!unpackUInt32(buf, leftSize, n))
        return false;
    tag = n >> 3;
    type = n & 7;
    return true;
}

}
}

#endif // _SSUUTILS_H_
