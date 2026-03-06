#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <IComponent.h>
#include <LogHelper.h>
#include <IPCMmf.h>
#include "FileScanFun.h"

class CFileScan : public IComponent
{
public:
	CFileScan();
	~CFileScan();

	static CFileScan& GetInstance();

	DWORD UnRegister();
	IComponent* Register();

	BOOL EnableFunction();
	BOOL DisableFunction();
	BOOL DispatchMessages(IPC_MSG_DATA* pIpcMsg);

private:
	CFileScanFun* m_pFileScanFun = nullptr;
};

extern "C" __declspec(dllexport) IComponent * GetComInstance();

