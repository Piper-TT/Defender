# Defender

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows%20x64-blue" alt="Platform">
  <img src="https://img.shields.io/badge/Language-C++17-purple" alt="Language">
  <img src="https://img.shields.io/badge/IDE-Visual%20Studio%202022-purple" alt="IDE">
  <img src="https://img.shields.io/badge/Build-Release-green" alt="Build">
</p>

Windows 终端安全防护工具集，提供剪贴板监控、文件扫描、防火墙管理、USB 控制、系统日志审计等功能模块。

---

## 功能模块

| 模块 | 说明 |
|------|------|
| **Client** | 控制台入口程序，统一调度各功能模块 |
| **ClipboardControl** | 剪贴板内容监控与管控 |
| **FileScanControl** | 文件安全扫描，支持 hash 比对 |
| **FireWallControl** | Windows 防火墙策略管理 |
| **USBControl** | USB 设备接入控制 |
| **SystemLogControl** | 系统日志采集与审计 |
| **HashStore** | 文件 hash 存储服务（基于 LMDB） |
| **Service** | 后台服务程序 |
| **Common** | 公共基础库 |
| **BaseMoudle** | 基础模块库 |

## 技术栈

- **开发环境**: Visual Studio 2022 (v143 工具集)
- **语言标准**: C++17
- **目标平台**: Windows x64
- **SDK**: Windows 10 SDK (10.0)
- **依赖库**:
  - [Boost](https://www.boost.org/) - 通用 C++ 库
  - [spdlog](https://github.com/gabime/spdlog) - 日志库
  - [nlohmann/json](https://github.com/nlohmann/json) - JSON 解析
  - [LMDB](https://www.symas.com/lmdb) - 高性能键值存储
  - [libpng](http://www.libpng.org/) - PNG 图像处理
  - [zlib](https://www.zlib.net/) - 压缩库

## 目录结构

```
Defender/
├── DefenderT.sln          # VS 解决方案
├── Client/                # 控制台程序
├── ClipboardControl/      # 剪贴板控制
├── FileScanControl/       # 文件扫描
├── FireWallControl/       # 防火墙控制
├── USBControl/            # USB 控制
├── SystemLogControl/      # 系统日志
├── HashStore/             # Hash 存储
├── Service/               # 后台服务
├── Common/                # 公共库
├── BaseMoudle/            # 基础模块
├── include/               # 头文件及静态库
│   ├── libpng/
│   ├── zlib/
│   └── lib/               # .lib 文件
├── 3rdp/                  # 第三方依赖
│   ├── boost/
│   ├── spdlog/
│   └── json/
├── bin/                   # 输出目录
│   └── win/release/x86_64/
└── lmdb/                  # LMDB 源码
```

## 构建步骤

### 1. 环境准备

1. 安装 **Visual Studio 2022**，勾选"使用 C++ 的桌面开发"
2. 安装 **Windows 10 SDK**

### 2. 依赖配置

将第三方库放置到以下目录：

| 依赖 | 目录 |
|------|------|
| Boost | `3rdp\boost\ (头文件及 `bin\win\release\x86_64\ 库) |
| spdlog | `3rdp\spdlog\include\ |
| nlohmann/json | `3rdp\json\include\ |
| libpng | `include\libpng\ |
| zlib | `include\zlib\ |
| 静态库 | `include\lib\ (libpng16_static.lib, zlibstatic.lib, sha1lib.lib) |
| lmdb.lib | `bin\win\release\x86_64\ |

### 3. 编译

**方式一：Visual Studio IDE**

1. 打开 `DefenderT.sln`
2. 选择配置 `Release|x64`
3. 先构建 `Common` 和 `BaseMoudle`
4. 构建整个解决方案

**方式二：命令行**

```batch
msbuild DefenderT.sln /p:Configuration=Release /p:Platform=x64 /m
```

### 4. 输出

所有编译产物位于：`bin\win\release\x86_64\`

## 模块依赖关系

```
Client
  └── 所有功能模块

ClipboardControl
  └── libpng, zlib, sha1lib

FileScanControl
  └── lmdb, Common, BaseMoudle, Boost, spdlog, json

FireWallControl / USBControl / SystemLogControl
  └── Common, BaseMoudle, Boost, spdlog, json

Common / BaseMoudle
  └── (基础静态库，被其他模块链接)
```

## 常见问题

### 链接错误：`deflate` 未解析

确保链接 `zlibstatic.lib`，并包含 `include\zlib` 头文件目录。

### 链接错误：`mdb_txn_begin` 未解析

确保 `bin\win\release\x86_64\lmdb.lib` 存在并被正确链接。

### 找不到 Boost 头文件

检查 `3rdp\boost` 目录结构，确保头文件直接位于该目录下（如 `3rdp\boost\boost\...`）。

---

## License

MIT License

---

<p align="center">
  <sub>Built with ❤️ by Piper-TT</sub>
</p>
