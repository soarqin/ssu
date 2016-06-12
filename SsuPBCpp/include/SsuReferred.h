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

#ifndef _REFERREDOBJECT_H_
#define _REFERREDOBJECT_H_

#include <cstdlib>

namespace ssu {

template<typename T>
class ReferredObject {
public:
    inline ReferredObject<T>(): _obj(NULL) {}
    inline ~ReferredObject<T>() { delete _obj; }
    inline ReferredObject<T>(ReferredObject<T>& other) {
        _obj = new(std::nothrow) T(*other._obj);
    }
    inline operator const T&() const { return *_obj; }
    inline operator T&() { checkObject(); return *_obj; }
    inline const T* Get() const { return _obj; }
    inline void Unset() { if (_obj) { delete _obj; _obj = NULL; } }
    inline ReferredObject<T>& operator=(const T& other ) { if (_obj) delete _obj; _obj = new(std::nothrow) T(other); return *this; }
    inline T* GetMutable() { checkObject(); return _obj; }

private:
    inline void checkObject() {
        if(!_obj)
            _obj = new(std::nothrow) T;
    }

private:
    T* _obj;
};

}

#endif // _REFERREDOBJECT_H_
