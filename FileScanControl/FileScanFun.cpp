#include "FileScanFun.h"
#include <FileOperationHelper.h>
#include "FeatureDB.h"
#include "FileEAHelper.h"

CFileScanFun::CFileScanFun() {
    BOOL bRet = CFeatureDB::GetInstance()->Init();
    if (bRet) {
        CFeatureDB::GetInstance()->Load();  // 鍔犺浇鐗瑰緛搴?
    }

    m_bStopSearch = FALSE;
}

CFileScanFun::~CFileScanFun() {
    if (m_PeCacheHelper) {
        m_PeCacheHelper->PE_CACHE_Clear_AllCache();
        delete m_PeCacheHelper;
        m_PeCacheHelper = nullptr;
    }
}



BOOL CFileScanFun::EnableScanFileFunction() {
    ThreadPool Pool(5);
    // 鑾峰彇鎵€鏈夐€昏緫椹卞姩鍣?
    DWORD   drives = GetLogicalDrives();
    DWORD   count  = 0;
    wstring wstrDrive;
    if (!m_PeCacheHelper) {
        m_PeCacheHelper = new PECacheHelper();
    }

    Pool.init();

    // TODO: 实现多线程文件扫描

    /*
    for (TCHAR letter = 'A'; letter <= 'Z'; ++letter)
    {
            if ((drives & 1) == 1)
            {
                    wchar_t FileName[MAX_PATH] = { 0 };
                    swprintf_s(FileName, MAX_PATH, _T("%c:\\"), letter);
                    FileScanThreadPool.enqueue([FileName]() {
                            CFileScanFun File;
                            File.GetFileListByFolder(wstring(FileName));
                    });
            }
            drives >>= 1;
    }
    */

    WriteInfo(("END"));
    return 0;
}

BOOL CFileScanFun::GetFileListByFolder(const std::wstring wstrFolder) {
    std::wstring strFullMask;
    std::wstring wstrSubFolder;
    std::wstring strCurFolder;
    std::string  strFileName;

    static DWORD dwFileCount = 0;

    static int          cnt = 0;
    long long           handle;  // 鏂囦欢鍙ユ焺
    size_t              len      = 0;
    size_t              pos      = 0;
    size_t              find_ret = wstring::npos;
    wstring             strTail;
    struct _wfinddata_t finder;  // 鏂囦欢淇℃伅鐨勭粨鏋勪綋
    /* win32 閾炬帴鏂囦欢鏈?绉?
      绫诲瀷
    1.蹇嵎鏂瑰紡 锛堟枃浠舵垨鐩綍锛夋枃浠跺睘鎬т笌鏅€氭枃浠剁浉鍚岋紝閫氳繃鍚庣紑.link璇嗗埆 灏辨槸涓€绉嶆櫘閫氭枃浠剁敱explorer.exe杩涚▼瑙ｆ瀽/缁存姢 闈炲唴鏍哥淮鎶?
    2.纭摼鎺?锛堝彧鑳芥槸鏂囦欢锛?灏辨槸鏅€氭枃浠讹紝鏃犳硶涓庣湡韬尯鍒?鎴栬€呰姣忎釜閮芥槸鐪熻韩 绫诲瀷鐢卞唴鏍哥淮鎶?
    3.杞摼鎺? 锛堝彧鑳芥槸鐩綍锛夐€氳繃鏂囦欢灞炴€ILE_ATTRIBUTE_REPARSE_POINT璇嗗埆锛屾殏涓嶇煡濡備綍涓庣鍙烽摼鎺ュ尯鍒?绫诲瀷鐢卞唴鏍哥淮鎶?
    4.绗﹀彿閾炬帴 锛堟枃浠舵垨鐩綍锛?閫氳繃鏂囦欢灞炴€ILE_ATTRIBUTE_REPARSE_POINT璇嗗埆锛屾殏涓嶇煡濡備綍涓庤蒋閾炬帴鍖哄埆 绫诲瀷鐢卞唴鏍哥淮鎶?
    瀵逛簬闄ょ‖閾炬帴涔嬪 鐨勫叏閮ㄤ笉鎵弿锛?

    */
    // 鍒ゆ柇鏄惁鏄?link鏂囦欢
    len = wstrFolder.length();
    if (len > 6) /* 瀛楃涓?.link/" 鐨勯暱搴?*/
    {
        pos = len - 6;
    }

    /* 濡傛灉鎵惧埌浜?*/
    strTail = wstrFolder.substr(pos, 6);
    if (!_tcsicmp(strTail.c_str(), _T(".link/"))) {
        WriteInfo((" skip quick link path= {}"), CStrUtil::ConvertW2A(wstrFolder).c_str());
        return 0;
    }

    DWORD attr = GetFileAttributes(wstrFolder.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        WriteInfo(("GetFileAttributes return attr is INVALID_FILE_ATTRIBUTES, Folder = {}"), CStrUtil::ConvertW2A(wstrFolder).c_str());
        return 0;
    }
    if (attr & FILE_ATTRIBUTE_REPARSE_POINT) {
        WriteInfo((" skip soft/symbol link path= {}"), CStrUtil::ConvertW2A(wstrFolder).c_str());
        return 0;
    } else if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        strFullMask = wstrFolder + std::wstring(_T("*.*"));
    } else {
        strFullMask = wstrFolder;
    }

    handle = _wfindfirst(strFullMask.c_str(), &finder);  // 绗竴娆℃煡鎵?
    if (-1 == handle) {
        return -1;
    }

    do {
        if (finder.attrib & _A_SUBDIR)                                                      // 濡傛灉鏄洰褰曞垯閫掑綊;
        {
            if (0 == _tcscmp(finder.name, _T(".")) || 0 == _tcscmp(finder.name, _T("..")))  // 濡傛灉鏄?鎴?.鍒欒繃婊?
                continue;

            wstrSubFolder = wstrFolder + finder.name + _T("\\");

            strCurFolder = wstrSubFolder;

            // if (m_pFileFinderProc != NULL && !m_bStopSearch)
            //	m_pFileFinderProc(this, FF_FOLDER, m_pCustomParam);

            // skip RECYCLER
            /*strCurFolder.MakeLower();*/
            transform(strCurFolder.begin(), strCurFolder.end(), strCurFolder.begin(), ::tolower);
            if (-1 != strCurFolder.find(_T("recycler")) || -1 != strCurFolder.find(_T("$recycle.bin"))) {
                continue;
            }

            // skip system temp folder    //涓嶈兘杩囨护鎺変复鏃剁洰褰曪紝鏈変簺宸ヤ笟杞欢浼氶噴鏀炬枃浠跺埌涓存椂鐩綍杩愯
            if (-1 != strCurFolder.find(_T("winnt\\temp")) || -1 != strCurFolder.find(_T("windows\\temp"))) {
                continue;
            }
            // skip user temp folder
            if (-1 != strCurFolder.find(_T("local settings\\temp")) || -1 != strCurFolder.find(_T("local\\temp"))) {
                continue;
            }

            // skip volume folder
            if (-1 != strCurFolder.find(_T("system volume information"))) {
                continue;
            }

            // Recursive call
            GetFileListByFolder(wstrSubFolder);
        } else {
            wstring wstrFullFileName = wstrFolder + finder.name;
            if (CheckIsPEFile(wstrFullFileName)) {
                // 鑾峰彇PE鏂囦欢鐨勮缁嗕俊鎭?
                wstring   wstrHashCode;
                ULONGLONG FileSize;
                ULONGLONG LastWriteTime;
                CHAR      strVirusName[MAX_VIRUS_NAME_LEN] = {0};
                GetFileInfoEx(wstrFullFileName, wstrHashCode, FileSize, LastWriteTime);

                // 淇濆瓨鍒版枃浠朵腑
                string strFileName = CStrUtil::ConvertW2A(wstrFullFileName) + "Hash:" + CStrUtil::ConvertW2A(wstrHashCode);
                FileOperationHelper::SeWriteFile("FileScan.txt", strFileName, strFileName.size());

                if (!m_PeCacheHelper->PE_CACHE_Query_Cache(wstrFullFileName, FileSize, LastWriteTime, NULL, NULL, NULL)) {
                    m_PeCacheHelper->PE_CACHE_insert(wstrFullFileName, FileSize, LastWriteTime, wstrHashCode);
                }

                // 鐗瑰緛搴撳紩鎿庢娴嬪嫆绱㈢梾姣?
                BOOL bVirus = CFeatureDB::GetInstance()->CheckRansomware(strVirusName, wstrFullFileName.c_str());
                if (bVirus) {
                    FileEAHelper::WriteFileExAttr(CStrUtil::ConvertW2A(wstrFullFileName), "Virus", "1");
                    WriteError(("*****************Virus Find Virus Path = {}"), CStrUtil::ConvertW2A(wstrFullFileName).c_str());
                }
            }
        }
        dwFileCount++;
    } while (!_wfindnext(handle, &finder) && !m_bStopSearch);

    _findclose(handle);

    return 0;
}

BOOL CFileScanFun::CheckIsPEFile(const std::wstring wstrFilePath) {
    BOOL   bIsPEFile   = FALSE;
    HANDLE hFileHandle = nullptr;
    BYTE   buffer[2];
    DWORD  bytesRead;

    // 鑾峰彇鏂囦欢鐨勫睘鎬?
    DWORD fileAttributes = GetFileAttributes(wstrFilePath.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES || (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        WriteError(("File not found or invalid"));
        goto END;
    }
    // 鎵撳紑鏂囦欢
    hFileHandle = CreateFile(wstrFilePath.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFileHandle == INVALID_HANDLE_VALUE) {
        WriteError(("Failed to open file  GetLasrError = {} filepath = {}"), GetLastError(), CStrUtil::ConvertW2A(wstrFilePath).c_str());
        goto END;
    }

    // 璇诲彇鏂囦欢鐨勫墠涓や釜瀛楄妭
    if (!ReadFile(hFileHandle, buffer, sizeof(buffer), &bytesRead, NULL) || bytesRead != sizeof(buffer)) {
        WriteError(("Failed to ReadFile GetLasrError = {} filepath = {} "), GetLastError(), CStrUtil::ConvertW2A(wstrFilePath).c_str());
        goto END;
    }

    // 鍒ゆ柇鏄惁涓?PE 鏂囦欢
    if (buffer[0] == 'M' && buffer[1] == 'Z') {
        bIsPEFile = TRUE;
    }

END:
    // 鍏抽棴鏂囦欢鍙ユ焺
    if (hFileHandle) {
        CloseHandle(hFileHandle);
    }
    return bIsPEFile;
}

TCHAR* CFileScanFun::GetHashString(const unsigned char* pcSrc, DWORD dwSrcLen, TCHAR* pszDst, DWORD dwDstLen) {
    if (dwSrcLen * 2 > dwDstLen) {
        return NULL;
    }

    for (int i = 0; i < dwSrcLen; i++) {
        swprintf_s(pszDst + 2 * i, dwDstLen - 2 * i, _T("%02X"), pcSrc[i]);
    }

    return pszDst;
}

BOOL CFileScanFun::GetFileInfoEx(const std::wstring wstrFullFileName, wstring& wstrHashCode, ULONGLONG& FileSize, ULONGLONG& LastWriteTime) {
    unsigned char             bHashCode[INTEGRITY_LENGTH]          = {0};
    TCHAR                     szFileHash[INTEGRITY_LENGTH * 2 + 1] = {0};
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (!m_pefilevalidate.GetPEFileDegistByLib(wstrFullFileName.c_str(), bHashCode)) {
        return FALSE;
    }

    GetHashString(bHashCode, sizeof(bHashCode), szFileHash, sizeof(szFileHash) / sizeof(TCHAR));
    if (!GetFileAttributesEx(wstrFullFileName.c_str(), GetFileExInfoStandard, &FileAttrData)) {
        return FALSE;
    }

    ULARGE_INTEGER size;
    ULARGE_INTEGER WriteTime;
    size.HighPart = FileAttrData.nFileSizeHigh;
    size.LowPart  = FileAttrData.nFileSizeLow;

    WriteTime.HighPart = FileAttrData.ftLastWriteTime.dwHighDateTime;
    WriteTime.LowPart  = FileAttrData.ftLastWriteTime.dwLowDateTime;

    FileSize      = size.QuadPart;
    LastWriteTime = WriteTime.QuadPart;
    wstrHashCode  = szFileHash;

    return TRUE;
}

