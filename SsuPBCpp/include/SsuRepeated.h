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

#ifndef _REPEATEDOBJECT_H_
#define _REPEATEDOBJECT_H_

#include <cstdlib>
#include <vector>

namespace ssu {

template<typename T>
class RepeatedObject {
public:
    typedef T* iterator;
    typedef const T* const_iterator;
public:
    inline RepeatedObject<T>(): _objs(_initObj) {}
    inline RepeatedObject<T>(const RepeatedObject<T>& other) {
        operator=(other);
    }
    inline ~RepeatedObject<T>() { if(_objs != _initObj) delete[] _objs; }
    inline RepeatedObject<T>& operator=(const RepeatedObject<T>& other) {
        if(_objs != _initObj) delete[] _objs;
        _size = other._size;
        _capacity = other._capacity;
        if(other._initObj != other._objs)
            _objs = new T[_capacity];
        else
            _objs = _initObj;
        if(_size > 0)
            memcpy(_objs, other._objs, sizeof(T) * _size);
        return *this;
    }
    inline const T& operator[](size_t idx) const { return _objs[idx]; }
    inline T& operator[](size_t idx) { return _objs[idx]; }
    inline void Reserve(size_t newsize) {
        if(newsize <= _capacity) return;
        _capacity = std::max(newsize, _capacity * 2);
        T* oldObjs = _objs;
        _objs = new T[_capacity];
        if(_size > 0)
            memcpy(_objs, oldObjs, sizeof(T) * _size);
        if(oldObjs != _initObj) delete[] oldObjs;
    }
    inline void Resize(size_t newsize) {
        Reserve(newsize);
        _size = newsize;
    }
    inline void Add(const T& val) {
        if(_size >= _capacity)
            Reserve(_capacity + 1);
        _objs[_size ++] = val;
    }
    inline T& Add() {
        if(_size >= _capacity)
            Reserve(_capacity + 1);
        return _objs[_size ++];
    }
    inline size_t Size() const { return _size; }
    inline size_t Capacity() const { return _capacity; }
    inline iterator begin() { return _objs; }
    inline iterator end() { return _objs + _size; }
    inline const_iterator begin() const { return _objs; }
    inline const_iterator end() const { return _objs + _size; }
    inline void Clear() { if(_objs != _initObj) delete[] _objs; _objs = _initObj; _size = 0; _capacity = _initSize; }

private:
    static const int _initSize = 8;
    T* _objs = _initObj;
    size_t _size = 0;
    size_t _capacity = _initSize;
    T _initObj[_initSize];
};

}

#endif // _REFERREDOBJECT_H_
