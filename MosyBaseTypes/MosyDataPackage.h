#pragma once
#include "MosyObject.h"
#include <string>

class _declspec(dllexport) MosyDataPackage:public MosyObject
{
public:
	std::string Data;
	MosyDataPackage();
	~MosyDataPackage();
};

