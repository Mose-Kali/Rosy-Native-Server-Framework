#pragma once
#include "MosyObject.h"
#include "MosyString.h"
#include <string>
#ifdef DLL_FILE
const class _declspec(dllexport)
MosyValue :
	public MosyObject
#else
const class _declspec(dllexport)
MosyValue :
	public MosyObject
#endif
{
protected:
	std::string Value;
public:
	MosyValue();
	MosyValue(std::wstring s);
	MosyValue(std::string s);
	MosyValue(double s);
	int GetInteger();
	long long int GetLLInteger();
	double GetDouble();
	std::string GetAString();
	std::wstring GetString();
	bool GetBoolean();
	virtual MosyObject* obj()override;
	bool operator <(const MosyValue& MV)const;
	bool operator >(const MosyValue& MV)const;
	bool operator <=(const MosyValue& MV)const;
	bool operator >=(const MosyValue& MV)const;
	bool operator()(const MosyValue& MV)const;
};

