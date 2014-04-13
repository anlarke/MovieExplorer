#pragma once
#include <unordered_map>

typedef RArray<BYTE> ARBYTE;

class ImageHash
{
public:
	ImageHash();
	~ImageHash();

	ARBYTE *GetImage(RString strName);
	void SetImage(RString strName, ARBYTE image);

protected:
	std::unordered_map<std::wstring, ARBYTE> hashtable;
};

