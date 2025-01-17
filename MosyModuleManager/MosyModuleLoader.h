#pragma once
#include "MosyLogger.h"
#include "MosyModuleLoader.h"
#include "MosyObject.h"
#include "MosyString.h"
#include "MosyValue.h"
#include <map>
#include <vector>
using namespace std;

typedef map<wstring, MosyValue> MosyViewModule;
typedef map<wstring, void*> MosyEnvironment;
typedef map<wstring, MosyValue> MosyControllerParams;
typedef map<wstring, vector<MosyValue>>(*MosyInterceptor)(map<wstring, void*>, map<wstring, MosyValue>);
typedef MosyValue(*RestfulControllerTemplate)(map<wstring, void*>, map<wstring, MosyValue>);
typedef MosyViewModule(*ViewControllerTemplate)(map<wstring, void*>, map<wstring, MosyValue>);
typedef map<wstring, vector<MosyValue>>(*MosyFunctionTemplate)(MosyEnvironment, MosyControllerParams);
typedef int(*MosyModuleInitialize)(MosyEnvironment*);
typedef map<wstring, vector<MosyValue>> MosyFunctionResult;
typedef MosyValue MosyModuleInstance;

class _declspec(dllexport) MosyModuleLoader
{
public:
	enum MosyModuleLoaderErrorCode
	{
		MOSY_MODULE_LOADER_FILE_NOT_FOUND,
		MOSY_MODULE_LOADER_FAILED_TO_LOAD_MODULE,
		MOSY_MODULE_LOADER_FAILED_TO_LOAD_RESTFUL_CONTROLLER,
		MOSY_MODULE_LOADER_FAILED_TO_LOAD_VIEW_CONTROLLER,
		MOSY_MODULE_LOADER_FAILED_TO_LOAD_FUNCTION,
		MOSY_MODULE_LOADER_FAILED_TO_LOAD_INTERCEPTOR,
	};
	struct MosyModuleLoaderException :public exception
	{
		MosyModuleLoaderErrorCode ErrorCode;
		MosyModuleLoaderException(MosyModuleLoaderErrorCode MosyErrorCode)
		{
			ErrorCode = MosyErrorCode;
		}
		const char * what() const throw ()
		{
			switch (ErrorCode)
			{
			case MOSY_MODULE_LOADER_FILE_NOT_FOUND:
				return "Failed to Find Module,Make Sure the Path you have Registried is Right.";
				break;
			case MOSY_MODULE_LOADER_FAILED_TO_LOAD_MODULE:
				return "Failed to Load Module,Make Sure the Path you have Registried is Right.";
				break;
			case MOSY_MODULE_LOADER_FAILED_TO_LOAD_RESTFUL_CONTROLLER:
				return "Failed to Load Restful Controller,Make Sure the Controller Name you have Registried is Right.";
				break;
			case MOSY_MODULE_LOADER_FAILED_TO_LOAD_VIEW_CONTROLLER:
				return "Failed to Load View Controller,Make Sure the Controller Name you have Registried is Right.";
				break;
			case MOSY_MODULE_LOADER_FAILED_TO_LOAD_INTERCEPTOR:
				return "Failed to Load Interceptor,Make Sure the Controller Name you have Registried is Right.";
				break;
			case MOSY_MODULE_LOADER_FAILED_TO_LOAD_FUNCTION:
				return "Failed to Load Function,Make Sure the Interface you have Registried is Right.";
				break;
			default:
				return "Failed to Load Module,Unkonw Error. :(";
				break;
			}
		}
		MosyModuleLoaderErrorCode GetErrorCode()
		{
			return ErrorCode;
		}
	};
	HINSTANCE LoadMosyModule(MosyValue Path);
	ViewControllerTemplate GetViewController(MosyModuleInstance Instance, MosyValue ControllerName);
	RestfulControllerTemplate GetRestfulController(MosyModuleInstance Instance, MosyValue ControllerName);
	MosyFunctionTemplate GetFunction(MosyModuleInstance Instance, MosyValue DatabaseInterfaceName);
	MosyInterceptor GetInterceptor(MosyModuleInstance Instance, MosyValue InterceptorName);
	void FreeMosyModule(HINSTANCE ModuleInstance);
};

