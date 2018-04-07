#pragma once

template <typename T, typename T1>
class jArray
{
public:
	T1 add(T el)
	{
		for(T1 i = 0; i < num_elems; i++)
		{
			if(!active_elems[i])
			{
				_elems[i] = el;
				active_elems[i] = true;
				return i;
			}
		}
		if(!locked)
		{
			_elems = (T*)realloc(_elems, (num_elems + 1) * sizeof(T));
			active_elems = (bool*)realloc(active_elems, (num_elems + 1));
		}
		_elems[num_elems] = el;
		active_elems[num_elems] = true;
		num_elems++;
		return num_elems - 1;
	}

	void set(T1 n, T el)
	{
		if(n >= num_elems)
		{
			if(!locked)
			{
				_elems = (T*)realloc(_elems, (n + 1) * sizeof(T));
				active_elems = (bool*)realloc(active_elems, (n + 1));
				memset(active_elems + num_elems, 0, n - num_elems);
			}
			active_elems[n] = true;
			_elems[n] = el;
			num_elems = n + 1;
			return;
		}
		active_elems[n] = true;
		_elems[n] = el;
	}

	T get(T1 n)
	{
		return _elems[n];
	}

	/*T& operator[](const T1 n)
	{
		return _elems[n];
	}*/

	bool isActive(T1 n)
	{
		return active_elems[n];
	}

	void remove(T1 n)
	{
		active_elems[n] = false;
	}

	void destroy(T1 n)
	{
		active_elems[n] = false;
		::free(_elems[n]);
		_elems[n] = 0;
	}
	T1 size()
	{
		return num_elems;
	}
	void free()
	{
		for(T1 i = 0; i < num_elems; i++)
		{
			if(active_elems[i])
				free(_elems[i]);
		}
		num_elems = 0;
	}
	void alloc(T1 size)
	{
		_elems = (T*)realloc(_elems, size * sizeof(T));
		active_elems = (bool*)realloc(active_elems, size);
		memset(active_elems + num_elems, 0, size - num_elems);
		locked = true;
	}
private:
	T *_elems;
	T1 num_elems;
	bool *active_elems;
	bool locked;
public:
	jArray(void)
	{
		_elems = new T[1];
		num_elems = 0;
		active_elems = new bool[1];
		active_elems[0] = false;
		locked = false;
	}
	~jArray(void)
	{
		::free(_elems);
		::free(active_elems);
	};
};

template <typename T, typename T1>
class jArrayS
{
public:
	T1 add(T el)
	{
		for(T1 i = 0; i < num_elems; i++)
		{
			if(!active_elems[i])
			{
				_elems[i] = el;
				active_elems[i] = true;
				return i;
			}
		}
		if(!locked)
		{
			_elems = (T*)realloc(_elems, (num_elems + 1) * sizeof(T));
			active_elems = (bool*)realloc(active_elems, (num_elems + 1));
		}
		_elems[num_elems] = el;
		active_elems[num_elems] = true;
		num_elems++;
		return num_elems - 1;
	}

	void set(T1 n, T el)
	{
		if(n >= num_elems)
		{
			if(!locked)
			{
				_elems = (T*)realloc(_elems, (n + 1) * sizeof(T));
				active_elems = (bool*)realloc(active_elems, (n + 1));
				memset(active_elems + num_elems, 0, n - num_elems);
			}
			active_elems[n] = true;
			_elems[n] = el;
			num_elems = n + 1;
			return;
		}
		active_elems[n] = true;
		_elems[n] = el;
	}

	T get(T1 n)
	{
		return _elems[n];
	}

	T *getRef(T1 n)
	{
		return &(_elems[n]);
	}

	bool isActive(T1 n)
	{
		return active_elems[n];
	}

	void remove(T1 n)
	{
		active_elems[n] = false;
	}

	void destroyAndCompact(T1 n)
	{
		active_elems[n] = false;
		for(T i = n; i < num_elems - 1; i++)
			_elems[i] = _elems[i + 1];
		num_elems--;
	}

	T1 size()
	{
		return num_elems;
	}
	/*void Free()
	{
		free(_elems);
		num_elems = 0;
	}*/
	void clear()
	{
		::free(_elems);
		_elems = new T[1];
		num_elems = 0;
		active_elems = new bool[1];
		active_elems[0] = false;
		locked = false;
	}
	void alloc(T1 size)
	{
		_elems = (T*)realloc(_elems, size * sizeof(T));
		active_elems = (bool*)realloc(active_elems, size);
		memset(active_elems + num_elems, 0, size - num_elems);
		locked = true;
	}
	void clone(jArrayS<T, T1> *target)
	{
		target->alloc(num_elems);
		memcpy(target->_elems, _elems, num_elems * sizeof(T));
		memcpy(target->active_elems, active_elems, num_elems);
		target->num_elems = num_elems;
	}
private:
	T *_elems;
	bool *active_elems;
	T1 num_elems;
	bool locked;
public:
	jArrayS(void)
	{
		_elems = new T[1];
		num_elems = 0;
		active_elems = new bool[1];
		active_elems[0] = false;
		locked = false;
	}
	~jArrayS(void)
	{
		::free(_elems);
		::free(active_elems);
	};
};

template <typename T, typename T1>
class __declspec(dllexport) jfArray
{
public:
	__forceinline void add(T el)
	{
		realloc(num_elems + 1);
		_elems[num_elems] = el;
		num_elems++;
	}
	__forceinline void set(T1 n, T el)
	{
		if(n >= _size)
		{
			_elems = (T*)::realloc(_elems, (n + 1) * sizeof(T));
			_size = n + 1;
		}
		_elems[n] = el;
		num_elems++;
	}
	__forceinline void alloc(T1 size)
	{
		_elems = new T[size];
		num_elems = 0;
		_size = size;
	}
	__forceinline void realloc(T1 size)
	{
		_elems = (T*)::realloc(_elems, size * sizeof(T));
		_size = size;
	}
	void clone(jfArray<T, T1> *target)
	{
		target->alloc(num_elems);
		memcpy(target->_elems, _elems, num_elems * sizeof(T));
		target->num_elems = num_elems;
	}
	void clear()
	{
		::free(_elems);
		_elems = new T[1];
		num_elems = 0;
		_size = 1;
	}
	void remove(T1 n)
	{
		for(T1 i = n; i < num_elems - 1; i++)
			_elems[i] = _elems[i + 1];
		num_elems--;
	}
	__forceinline T& operator[](T1 n)
	{
		return *(_elems + n);
	}
	__forceinline T1 Size()
	{
		return _size;
	}
public:
	T *_elems;
	T1 num_elems;
	T1 _size;
public:
	jfArray(void)
	{
		_elems = new T[1];
		num_elems = 0;
		_size = 1;
	}
	~jfArray(void)
	{
		::free(_elems);
	};
};
