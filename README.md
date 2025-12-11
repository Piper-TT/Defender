# Defender 项目构建说明

## 概述
- 解决方案文件：`DefenderT.sln`
- 主要模块：`Client`（控制台可执行）、`ClipboardControl`、`FileScanControl`、`FireWallControl`、`USBControl`、`SystemLogControl`、`Common`、`BaseMoudle`
- 目标平台：`x64`
- 默认构建配置：`Release`
- 默认输出目录：`bin\win\release\x86_64`

## 开发环境
- Visual Studio 2022（工具集 `v143`）
- Windows 10 SDK（`10.0`）
- C++ 标准：`C++17`
- 编译选项统一：`/utf-8` 字符集、运行库 `MultiThreadedDLL`

## 目录与第三方依赖
- `3rdp\boost`：Boost 头文件与库根目录
  - 头文件目录示例：`d:\code_work\Defender\3rdp\boost`
  - 库目录示例：`d:\code_work\Defender\3rdp\boost\bin\win\release\x86_64`
- `3rdp\spdlog\include`：spdlog 头文件目录
- `3rdp\json\include`：nlohmann/json 头文件目录
- `include\libpng`：libpng 头文件目录
- `include\zlib`：zlib 头文件目录
- `include\lib`：本项目使用的第三方静态库集合（如 `libpng16_static.lib`、`zlibstatic.lib`、`sha1lib.lib`）
- `bin\win\release\x86_64`：工程输出目录，同时也用于放置某些第三方库（如 `lmdb.lib`）

## 模块依赖要点
- `Common`、`BaseMoudle`：被其他模块链接为静态库（`Common.lib`、`BaseMoudle.lib`）
- `ClipboardControl`
  - 需要：`libpng16_static.lib`、`zlibstatic.lib`、`sha1lib.lib`
  - 头文件：`include\libpng`、`include\zlib`
- `FileScanControl`
  - 需要：`lmdb.lib`、`ws2_32.lib`、`Crypt32.lib`
  - 头文件：`3rdp\spdlog\include`、`3rdp\json\include`、`3rdp\boost`、`Common`、`BaseMoudle`
- `FireWallControl`、`SystemLogControl`、`USBControl`
  - 需要：`Common.lib`、`BaseMoudle.lib`
  - 头文件：`3rdp\spdlog\include`、`3rdp\json\include`、`3rdp\boost`、`Common`、`BaseMoudle`

## 构建步骤（推荐）
1. 安装 Visual Studio 2022，确保勾选“使用 C++ 的桌面开发”。
2. 将第三方依赖放置到上述指定目录（尤其是 `3rdp\boost`、`3rdp\spdlog\include`、`3rdp\json\include`、`include\lib`）。
   - Boost 建议版本目录：`3rdp\boost`（工程已指向该根目录及 `bin\win\release\x86_64` 库目录）。
   - `lmdb.lib` 放置在 `bin\win\release\x86_64`。
3. 打开 `DefenderT.sln`，选择 `Release|x64`。
4. 先构建 `Common` 和 `BaseMoudle`，再构建各功能模块或选择“生成解决方案”。

## 命令行构建（可选）
- 若已安装 VS 构建工具并配置好环境变量，可使用：
  - `msbuild .\DefenderT.sln /p:Configuration=Release /p:Platform=x64 /m`
- 未安装 `msbuild` 时，建议通过 VS IDE 构建。

## 产物位置
- 所有模块的 `Release|x64` 输出位于：`bin\win\release\x86_64`

## 常见问题
- 链接错误 `deflate` 未解析：确保链接 `zlibstatic.lib`，并包含 `include\zlib` 头文件目录。
- 链接错误 `mdb_txn_begin` 未解析：确保 `bin\win\release\x86_64\lmdb.lib` 存在并被链接。

## 说明
- 工程已在各模块的 `Release|x64` 配置中统一了包含目录、库目录、编译标准与运行库设置，按上述目录放置第三方依赖即可构建通过。





