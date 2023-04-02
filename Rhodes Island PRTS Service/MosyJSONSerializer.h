#pragma once
#include <string>
#include <map>
#include <vector>
#include <queue>
#include "MosyString.h"
#include "MosyValue.h"
using namespace std;

class _declspec(dllexport) MosyJSONSerializer
{
protected:
	struct Node {
		wstring Parent;
		wstring Key;
		bool isArray;
		bool isLeaf;
		int deep = 0;
		vector<wstring>Value;
	};
	vector<Node> Tree;
	wstring JSONString;
	int ObjectCount = 0;
	int MaxDeep = 1;
	int GetNextPair(int off, wchar_t Type);
public:
	void AddValue(MosyValue Key, MosyValue Value);
	void AddMap(MosyValue Key, map<wstring, MosyValue> Value);
	void AddArray(MosyValue Key, vector<MosyValue> Value);
	void Remove(MosyValue Key);
	void Remove(MosyValue Key, int Deep);
	void Clear();
	bool isEmpty();
	int GetObjectCount();
	MosyValue GetJSONString();
};

