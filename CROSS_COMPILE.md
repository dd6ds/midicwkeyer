## Cross-Compilation to Windows (MSVC)

Building with MSVC toolchain from Linux (recommended for Windows deployment to avoid antivirus false positives):

```bash
# Build for Windows with MSVC toolchain (64-bit)
cargo build --target x86_64-pc-windows-msvc --release

# The Windows executable will be at:
# target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
```

### Why MSVC vs GNU/MinGW?

| Toolchain | Binary Size | Antivirus Detection | Dynamic Linking |
|-----------|-------------|---------------------|-----------------|
| **MSVC** (recommended) | ~365 KB | ✅ Rarely flagged | Uses Windows system DLLs |
| GNU/MinGW | ~2.4 MB | ⚠️ Sometimes flagged | Statically linked |

**MSVC binaries are preferred** because:
- Smaller size (uses Windows system libraries)
- Better Windows compatibility
- **Fewer false positives from antivirus software**
- Standard Microsoft toolchain output

### Setup for MSVC Cross-Compilation

This has already been set up in the project, but for reference:

1. **Install xwin** (downloads MSVC SDK):
   ```bash
   cargo install xwin
   ```

2. **Download MSVC SDK**:
   ```bash
   xwin --accept-license splat --output ~/.xwin
   ```

3. **Install LLD linker** (usually already available):
   ```bash
   sudo apt install lld  # Debian/Ubuntu
   sudo dnf install lld  # Fedora/RHEL
   ```

4. The `.cargo/config.toml` is already configured with the proper paths.
