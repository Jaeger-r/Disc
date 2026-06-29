# DiskClient Windows 打包说明

## 目标

把 Qt 网盘客户端整理为一个可以直接发给 Windows 用户的目录，包含：

- `DiskClient.exe`
- Qt 运行库和插件
- `diskclient.ini`
- `tls/ca.crt`
- `icon.png`
- `file.png`

客户端默认走 TLS，并连接：

- 服务端 IP：`192.168.1.72`
- 端口：`1234`

## 目录结构

打包完成后的可分发目录应类似下面这样：

```text
DiskClient-windows-x64/
├── DiskClient.exe
├── diskclient.ini
├── icon.png
├── file.png
├── Qt6Core.dll
├── Qt6Gui.dll
├── Qt6Network.dll
├── Qt6Widgets.dll
├── platforms/
├── tls/
│   └── ca.crt
└── ...
```

`platforms/qwindows.dll` 必须存在，否则程序通常会直接启动失败。

## 前置条件

在 Windows 构建机上准备：

- Qt 6.9.1 for Windows
- `cmake`
- `ninja` 或 Visual Studio C++ 工具链
- `windeployqt`
- PowerShell 5.1 或更高版本

推荐直接用 Qt 自带的命令行环境或 “x64 Native Tools Command Prompt for VS”。

## 一键打包

在 Windows 上进入项目目录后运行：

```powershell
powershell -ExecutionPolicy Bypass -File .\diskclient\deploy\package_windows_client.ps1 `
  -QtRoot "C:\Qt\6.9.1\msvc2022_64" `
  -BuildDir ".\diskclient\build\windows-release" `
  -Config Release
```

如果你使用 MinGW 套件，也可以把 `-QtRoot` 指向类似：

```text
C:\Qt\6.9.1\mingw_64
```

脚本会自动完成：

1. `cmake` 配置
2. 编译 `DiskClient.exe`
3. 调用 `windeployqt` 复制 Qt 依赖
4. 复制 `diskclient.ini`
5. 复制 `tls/ca.crt`
6. 复制图片资源
7. 生成 `out/DiskClient-windows-x64.zip`

## 出包后检查

至少检查下面几项：

1. 双击 `DiskClient.exe` 能正常启动。
2. 包内存在 `platforms/qwindows.dll`。
3. 包内存在 `tls/ca.crt`。
4. `diskclient.ini` 中 `serverHost=192.168.1.72`。
5. Windows 客户端与 Mac 服务端在同一局域网下能连上 `192.168.1.72:1234`。

## 联机前检查

在 Mac 服务端机器上确认：

1. `diskserver.ini` 的 `listenHost=0.0.0.0`
2. 服务端成功监听 `1234`
3. macOS 防火墙允许该程序入站
4. 当前服务端证书 SAN 含 `192.168.1.72`

## 常见问题

### 能启动但提示连不上服务端

优先检查：

- 服务端是否真的启动成功
- Windows 客户端是否能 ping 通 `192.168.1.72`
- 防火墙是否拦截 `1234`
- `diskclient.ini` 是否被改回了其他地址

### 启动时报平台插件错误

通常是 `platforms/qwindows.dll` 缺失，或 `windeployqt` 没有正确执行。

### TLS 握手失败

优先检查：

- 包里的 `tls/ca.crt` 是否存在
- 客户端连接 IP 是否仍然是 `192.168.1.72`
- 服务端证书是否仍然匹配该 IP

### 这台 Mac 为什么不能直接产出 Windows 包

因为当前环境没有安装：

- Windows Qt 工具链
- `windeployqt`
- Windows 交叉编译器

所以这边可以把打包流程和脚本准备好，但真正的 `.exe` 仍需要在 Windows 的 Qt 构建环境中生成。
