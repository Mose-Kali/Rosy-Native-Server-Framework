// MosyModuleDebugger.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "MosyJSONSerializer.h"

int main()
{
	MosyJSONSerializer Serializer;
	Serializer.AddValue(MosyValue(L"Test"), MosyValue(L"Test"));
	vector<MosyValue> v;
	v.push_back(MosyValue(L"Test1"));
	v.push_back(MosyValue(L"Test2"));
	Serializer.AddArray(MosyValue(L"Tests\\TestArray"), v);
	map<wstring, MosyValue> m;
	m.insert_or_assign(L"Test3Key", MosyValue(L"Test3"));
	m.insert_or_assign(L"Test4Key", MosyValue(L"Test4"));
	Serializer.AddMap(MosyValue(L"TestMap"), m);
	Serializer.AddMap(MosyValue(L"Tests\\TestsMap"), m);
	vector<map<wstring, MosyValue>> ValueSet;
	ValueSet.push_back(m);
	//Serializer.AddSet(MosyValue(L"Tests\\TestSet"), ValueSet);
	Serializer.AddSet(MosyValue(L"TestSet"), ValueSet);
	MosyValue Json = Serializer.GetJSONString();
	wprintf(Json.GetString().c_str());
}
