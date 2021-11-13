#pragma once
template <typename T>
inline void SafeDelete(T *&obj)
{
	delete obj;
	p = nullptr;
}