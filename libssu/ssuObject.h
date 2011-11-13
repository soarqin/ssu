#ifndef _SSUOBJECT_H_
#define _SSUOBJECT_H_

namespace ssu
{

	class Object
	{
	public:
		Object(): _buffer__(NULL), _length__(-1), _packing__(false) {}
		virtual ~Object() { freeBuffer(); }
		inline void resetBuffer() { freeBuffer(); _buffer__ = NULL; _length__ = -1; }
		inline void freeBuffer() { if(_packing__ && _buffer__ != NULL) { free(_buffer__); _packing__ = false; } }
		inline void unpackPrepare(const void * buffer, int length)
		{
			freeBuffer();
			_buffer__ = (void *)buffer;
			_length__ = length;
		}
		bool unpack();
		bool pack();
		inline int getLength()
		{
			return _length__;
		}
		inline const void * getBuffer()
		{
			return _buffer__;
		}
	protected:
		virtual bool unpackFromBuffer(const void * buffer, int length) = 0;
		virtual bool packToBuffer(void * buffer) = 0;
		virtual int calcLength() = 0;
	private:
		void * _buffer__;
		int _length__;
		bool _packing__;
	};

}

#endif // _SSUOBJECT_H_
