// FireWallControl.cpp : 瀹氫箟 DLL 鐨勫鍑哄嚱鏁般€?
//

#include "pch.h"
#include "framework.h"
#include "FireWallControl.h"

CFireWallControl::CFireWallControl() {}

CFireWallControl::~CFireWallControl() {
    WriteInfo("CFireWallControlClass Buffer Free");
    if (m_pFirewallBase) {
        delete m_pFirewallBase;
        m_pFirewallBase = nullptr;
    }
}

CFireWallControl& CFireWallControl::GetInstance() {
    static CFireWallControl instance;
    return instance;
}

DWORD CFireWallControl::UnRegister() {
    return 0;
}

IComponent* CFireWallControl::Register() {
    return &GetInstance();
}

BOOL CFireWallControl::EnableFunction() {
    int            nWindowsVersion = 0;
    BOOL           bWin64          = FALSE;
    CWindowsHelper GetWindowsVersion;
    GetWindowsVersion.SeGetWindowsVersion(nWindowsVersion, bWin64);

    if (nWindowsVersion == WIN_XP) {
        // 2000涓嶈€冭檻浜?
        // XP鐜
        m_pFirewallBase = CFirewallBase::factory("WinXP");
    } else {
        m_pFirewallBase = CFirewallBase::factory("Win7");
    }

    m_pFirewallBase->InitCom();

    m_pFirewallBase->StartMonitoring();

    // m_pFirewallBase->ConfigFirewallEnable(TRUE);

    return 0;
}

BOOL CFireWallControl::DisableFunction() {
    return 0;
}

BOOL CFireWallControl::DispatchMessages(IPC_MSG_DATA* pIpcMsg) {
    if (!pIpcMsg) {
        return FALSE;
    }

    std::string strData(reinterpret_cast<char*>(pIpcMsg->Data), pIpcMsg->dwSize);
    switch (pIpcMsg->dwMsgCode) {
        case FIREWALL_CONTROL_OPEN_ALL_FUNCTION:
            EnableFunction();
            break;
        case FIREWALL_CONTROL_CLOSE_ALL_FUNCTION:
            DisableFunction();
            break;
        default:
            break;
    }
    return TRUE;
}

FIREWALLCONTROL_API IComponent* GetComInstance() {
    WriteInfo("Welcome to FireWallControl!");
    return &CFireWallControl::GetInstance();
}
