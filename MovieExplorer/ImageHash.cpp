#include "stdafx.h"
#include "ImageHash.h"
#include "MovieExplorer.h"
#include <unordered_map>

ImageHash::ImageHash()
{
}

ImageHash::~ImageHash()
{
}

ARBYTE* ImageHash::GetImage(RString strName)
{
	std::wstring s = std::wstring((LPCWSTR)strName);
	if (hashtable.count(s) == 0)
		return NULL;
	else
		return(&hashtable[s]);
}

void ImageHash::SetImage(RString strName, ARBYTE image)
{
	std::wstring s = std::wstring((LPCWSTR)strName);
	hashtable[s] =  image;
}


