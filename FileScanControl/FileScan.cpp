#include "FileScan.h"

CFileScan::CFileScan() {
    return;
}

CFileScan::~CFileScan() {
    if (m_pFileScanFun) {
        delete m_pFileScanFun;
        m_pFileScanFun = nullptr;
    }
}

CFileScan& CFileScan::GetInstance() {
    static CFileScan instance;
    return instance;
}

DWORD CFileScan::UnRegister() {
    return 0;
}

IComponent* CFileScan::Register() {
    return nullptr;
}

BOOL CFileScan::EnableFunction() {
    // 开启文件扫描功能
    if (!m_pFileScanFun) {
        m_pFileScanFun = new CFileScanFun();
    }
    m_pFileScanFun->EnableScanFileFunction();
    return 0;
}

BOOL CFileScan::DisableFunction() {
    if (m_pFileScanFun) {
        m_pFileScanFun->StopSearch();
        delete m_pFileScanFun;
        m_pFileScanFun = nullptr;
    }
    return 0;
}

BOOL CFileScan::DispatchMessages(IPC_MSG_DATA* pIpcMsg) {
    std::string strData(reinterpret_cast<char*>(pIpcMsg->Data), pIpcMsg->dwSize);
    switch (pIpcMsg->dwMsgCode) {
        case FILESCAN_CONTROL_OPEN_ALL_FUNCTION:
            EnableFunction();
            break;
        case FILESCAN_CONTROL_CLOSE_ALL_FUNCTION:
            DisableFunction();
            break;
    }
    return 0;
}

// 这是导出函数的一个示例。
FILESCANCONTROL_EXPORTS IComponent* GetComInstance() {
    WriteInfo("Welcome to FileScanControl!");
    return &CFileScan::GetInstance();
}
