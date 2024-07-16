# Customizing `mesh_lite.proto` and Generating C and H Files

### Modifying `mesh_lite.proto` File

**Edit `mesh_lite.proto` File**:
   - Open the `mesh_lite.proto` file using any text editor.
   - Add new message types or modify the field definitions of existing messages.

### Generating C and H Files

**Using `protoc` Tool to Generate Files**:
   - Open a terminal or command prompt.
   - cd to the esp-mesh-lite/components/mesh_lite/src directory
   - Use the following command to generate C and H files:
     ```bash
     protoc --c_out=. mesh_lite.proto
     ```
   - This command will generate corresponding C and H files in the current directory.

### Notes

1. **Plugin Dependencies**:
   - Ensure that `protobuf-c-compiler` is installed on your system, which includes `protoc-gen-c` and `protoc-gen-h` plugins required for generating C and H files.

2. **Target Directory and Paths**:
   - Ensure that the output directory for generated files is correctly specified. Use the `--c_out` parameter to specify the directory for generated C and H files.

3. **Checking Compiler Plugins**:
   - If you encounter `protoc-gen-h: program not found or is not executable` error, it may be due to missing or incorrectly configured `protoc-gen-h` plugin. Ensure that this plugin is located in the system's executable path.
