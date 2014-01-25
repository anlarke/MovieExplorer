#pragma once

/*
#include <utility>
#define move	std::move
#define forw	std::forward
*/

template <class T>
struct REMOVEREF
{
	typedef T type;
};

template <class T>
struct REMOVEREF<T&>
{
	typedef T type;
};

template <class T>
struct REMOVEREF<T&&>
{
	typedef T type;
};

template <class T>
inline typename REMOVEREF<T>::type&& move(T&& arg)
{
	return ((typename REMOVEREF<T>::type&&)arg);
}

template <class T>
struct IDENTITY
{
	typedef T type;

	const T& operator()(const T& left) const
	{
		return (left);
	}
};

template <class T>
inline T&& forw(typename IDENTITY<T>::type& arg)
{
	return ((T&&)arg);
}
