#include "stdafx.h"
#include "ImageHash.h"
#include "MovieExplorer.h"

ImageHash::ImageHash()
{ 
}

ImageHash::~ImageHash()
{
}

ARBYTE* ImageHash::GetImage(RString strName)
{
	std::wstring sz = std::wstring((LPCWSTR)strName);
	mtx_.lock();
	std::unordered_map<std::wstring, ARBYTE>::iterator got = hashtable.find(sz);

	if (got == hashtable.end())  // If not in hashtable return NULL
	{
		mtx_.unlock();
		return NULL;
	}
	else    // In hash table. return image.
	{
		ARBYTE* arImage = &got->second;
		mtx_.unlock();
		return(arImage);
	}
}

void ImageHash::SetImage(RString strName, ARBYTE image)
{
	std::wstring sz = std::wstring((LPCWSTR)strName);
	mtx_.lock();
	hashtable[sz] =  image;
	mtx_.unlock();
}


