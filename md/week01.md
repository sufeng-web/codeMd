|      |      |                                             |
| :--- | :--- | :------------------------------------------ |
|      |      |                                             |
|      |      |                                             |
|      |      | 运行 **`cmake --build .`** 进行真正的编译。 |

# 问题

#### 1.文件太多太乱 (.sln, .vcxproj)

原因：使用VS 自动生成的私有配置文件，不具备通用性。

解决方法：只保留 .cpp 和CMakeLists.txt，其余全部删掉。

#### 2.命令行cmake .. 编译报错：Cannot find source file**

原因：  add_executable 里的路径不对，这个里面可以直接加入相对的路径（相对CMakeLists.txt文件路径）

解决方法：1.确保main.cpp和 CMakeLists.txt 在同一目录

2.或者在 add_executable里写相对路径。

#### 3.找不到 `.exe` 文件**

原因：只运行了 cmake ..

#### 标准的操作流程 (Workflow)

1. 清理环境：确保源码目录只有 `.cpp`、`.h` 和 `CMakeLists.txt`。

2. 配置阶段 (Configure)

   ```C++
   mkdir build
   cd build
   cmake ..
   ```

3. 构建阶段 (Build)

   ```
   cmake --build .
   ```

-----

####  VS 原生工程 vs. CMake 工程对比

| 特性     | Visual Studio 原生项目 (.sln/.vcxproj)        | CMake 工程 (CMakeLists.txt)                                  |
| :------- | :-------------------------------------------- | :----------------------------------------------------------- |
| 跨平台性 | 绑定 Windows：离开 Windows 和 VS 就无法编译。 | 全平台通用：一份脚本，可生成 VS 工程、Linux Makefile 或 Xcode 工程。 |

-----

#### 怎么解决新建.cpp的问题？

在 Visual Studio 里直接“打开文件夹”，VS 2019/2022 原生支持 CMake 了。不需要先生成 .sln 再打开。

1. **操作**：打开 Visual Studio，选择 **“打开本地文件夹”**，选中你的 `week01` 根目录。
2. **魔法发生**：VS 识别到 `CMakeLists.txt` 后，会自动在顶部出现“CMake”菜单。
3. **新建文件**：在左侧“解决方案资源管理器”里，**依然可以右键 -> 添加 -> 新建项**。
4. 自动同步：当你添加了tools.cpp，VS 会弹窗提示你，或者你手动在CMakeLists.txt里加上它。
   - *提示*：VS 甚至会尝试帮你自动把新文件名填进 `CMakeLists.txt`（取决于你的设置）。

**优点**：保留了 VS 丝滑的操作感，同时本质上还是 CMake 工程。