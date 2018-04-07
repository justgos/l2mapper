#pragma once

template <typename T>
class Singleton
{
public:
	static T& get_instance()
	{
		static T theInstance;
		return theInstance;
	}

protected:
	// need default ctor for GetInstance.
	// ctor is protected, not private in case you want to derive. <-- from MSDN
	Singleton() {}

private:
	Singleton(const T& o) { }
	Singleton& operator=(const T& o) { }
};