#pragma once
#include "MosyObject.h"
#include <string>

class _declspec(dllimport) MosyDataPackage:public MosyObject
{
public:
	std::string Data;
	MosyDataPackage();
	~MosyDataPackage();
};

