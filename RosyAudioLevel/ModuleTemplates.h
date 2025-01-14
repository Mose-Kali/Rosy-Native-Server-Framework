#pragma once

#include "MosyObject.h"
#include "MosyString.h"
#include "MosyValue.h"
#include "MosyModuleManager.h"
#include <map>
#include <vector>
using namespace std;

typedef map<wstring, MosyValue> MosyViewModule;
typedef map<wstring, void*> MosyEnvironment;
typedef map<wstring, MosyValue> MosyControllerParams;
typedef MosyValue(*RestfulControllerTemplate)(map<wstring, void*>, map<wstring, MosyValue>);
typedef MosyViewModule(*ViewControllerTemplate)(map<wstring, void*>, map<wstring, MosyValue>);
typedef map<wstring, vector<MosyValue>>(*DataBaseInterface)(MosyValue);
typedef MosyValue MosyModuleInstance;
typedef MosyValue MosyRestfulResult;