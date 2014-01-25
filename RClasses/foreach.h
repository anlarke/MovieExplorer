#pragma once

//#if _MSC_VER >= 1600
//	#pragma warning(disable:4258) // loop counter is only valid within loop, not after
//
//	#define foreach(ARRAY, ITEM) \
//		for (INT_PTR i = 0, AfHjB = 1; AfHjB && i < (ARRAY).GetSize(); ++i, AfHjB = !AfHjB) \
//			for (auto &ITEM = (ARRAY)[i]; AfHjB; AfHjB = 0)
//#endif

template <typename T, size_t n>
inline size_t size(const T (&)[n])
	{return n;}

#ifdef _VECTOR_
template <class T>
inline size_t size(const std::vector<T>& vec)
	{return vec.size();}
#endif

#if _MSC_VER >= 1600
	#pragma warning(disable:4258) // loop counter is only valid within loop, not after

	#define foreach_2(ARRAY, ITEM, ...) \
		for (decltype(size(ARRAY)) aJsUY = 0, AfHjB = 1; AfHjB && aJsUY < size(ARRAY); \
				++aJsUY, AfHjB = !AfHjB) \
			for (auto &ITEM = (ARRAY)[aJsUY]; AfHjB; AfHjB = 0)

	#define foreach_3(ARRAY, ITEM, ...) \
		for (decltype(size(ARRAY)) __VA_ARGS__ = 0, AfHjB = 1; AfHjB && __VA_ARGS__ < size(ARRAY); \
				++__VA_ARGS__, AfHjB = !AfHjB) \
			for (auto &ITEM = (ARRAY)[__VA_ARGS__]; AfHjB; AfHjB = 0)

	// Selector from: http://stackoverflow.com/a/12329091/939266
	#define foreach_SELMACRO_IMPL(_1, _2, _3, N, ...) N
	#define foreach_SELMACRO_IMPL_(tuple) foreach_SELMACRO_IMPL tuple
	#define foreach(...) foreach_SELMACRO_IMPL_((__VA_ARGS__, foreach_3(__VA_ARGS__), \
			foreach_2(__VA_ARGS__), ASSERT(false)))
#endif
