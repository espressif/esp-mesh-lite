# 自定义修改 `mesh_lite.proto` 文件并生成 C 和 H 文件

### 修改 `mesh_lite.proto` 文件

**编辑 `mesh_lite.proto` 文件**：
   - 打开 `mesh_lite.proto` 文件，可以使用任何文本编辑器进行修改。
   - 添加新的消息类型或者修改现有消息的字段定义。

### 生成 C 和 H 文件

**使用 `protoc` 工具生成文件**：
   - 打开终端或命令提示符。
   - cd 至 esp-mesh-lite/components/mesh_lite/src 目录
   - 使用以下命令生成 C 和 H 文件：
     ```bash
     protoc --c_out=. mesh_lite.proto
     ```
   - 这将在当前目录下生成相应的 C 和 H 文件。

### 注意事项

1. **插件依赖**：
   - 确保系统安装了 `protobuf-c-compiler`，它包含了 `protoc-gen-c` 和 `protoc-gen-h` 这两个插件，用于生成 C 和 H 文件。

2. **目标目录和路径**：
   - 确保生成文件的输出目录正确指定，可以使用 `--c_out` 参数指定生成的 C 和 H 文件的目录。

3. **检查编译器插件**：
   - 如果遇到 `protoc-gen-h: program not found or is not executable` 错误，可能是由于缺少或未正确配置 `protoc-gen-h` 插件。需要确保该插件在系统的可执行路径中。
