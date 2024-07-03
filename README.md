# MultiSensorScan

## 1.项目简介

该项目是一个使用 C++ 开发的应用程序，涉及各个功能模块化编写和调试。

## 2.项目结构

```
F:\学习项目\自适应成像\602_ui_02(各功能模块化编写)(0630调试)
├── 602_ui_02(各功能模块化编写)/
│   ├── x64/
│   │   ├── Debug/
│   │       ├── 602_ui_02(各功能模块化编写).log
│   ├── 602_ui_02(各功能模块化编写).vcxproj
│   ├── 602_ui_02(各功能模块化编写).vcxproj.filters
│   ├── 602_ui_02(各功能模块化编写).vcxproj.user
│   ├── dataprocess.cpp
│   ├── dataprocess.h
│   ├── datasave.cpp
│   ├── datasave.h
│   ├── Line_rule_struct.h
│   ├── LTDMC.dll
│   ├── LTDMC.h
│   ├── LTDMC.lib
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── mainwindow.qrc
│   ├── mainwindow.ui
│   ├── motioncontrol.cpp
│   ├── motioncontrol.h
│   ├── motion_ui.cpp
│   ├── motion_ui.h
│   ├── motion_ui.ui
│   ├── mouseeventlabel.cpp
│   ├── mouseeventlabel.h
│   ├── pa22scontrol.cpp
│   ├── pa22scontrol.h
│   ├── parameter_ui.cpp
│   ├── parameter_ui.h
│   ├── parameter_ui.ui
│   ├── PVT.dll
│   ├── servocontrol.cpp
│   ├── servocontrol.h
├── x64/
│   ├── Debug/
│       ├── 602_ui_02(各功能模块化编写).exe
│       ├── 602_ui_02(各功能模块化编写).pdb
├── .gitattributes
├── .gitignore
├── 602_ui_02(各功能模块化编写).sln
├── README.md

```



## 3.依赖库

**LTDMC**：一个动态链接库（DLL），用于运动控制。

**PVT**：另一个动态链接库，功能暂未明确。

## 4.安装

### 4.1前提条件

\- 安装 [Visual Studio](https://visualstudio.microsoft.com/)

\- 安装 [Git](https://git-scm.com/)

\- 安装并配置依赖库（如 LTDMC.dll 和 PVT.dll）

### 4.2克隆仓库

\```sh 

git clone https://github.com/yourusername/yourrepository.git 

cd yourrepository

### 4.3配置项目

确保在项目属性中正确配置了所有依赖库的包含路径和库路径。

### 4.4构建项目

1. 打开 `sln`文件。
2. 选择构建配置（Debug 或 Release）。
3. 点击“生成”菜单，选择“生成解决方案”。

## 5.使用说明

## 6.测试

## 7.贡献

欢迎贡献代码和提出建议。请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 获取更多信息。

## 8.常见问题

## 9.许可证

## 10.致谢
