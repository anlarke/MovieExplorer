#pragma once
#include <unordered_map>
#include <mutex>

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
	std::mutex mtx_;

};

