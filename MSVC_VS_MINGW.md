# MSVC vs MinGW Build Comparison

## Quick Summary

✅ **Use MSVC** (`x86_64-pc-windows-msvc`) for production Windows builds
⚠️ **Use MinGW** (`x86_64-pc-windows-gnu`) only if MSVC setup is not available

## Detailed Comparison

| Feature | MSVC | MinGW (GNU) |
|---------|------|-------------|
| **Binary Size** | ~365 KB | ~2.4 MB |
| **Antivirus Detection** | ✅ Rarely flagged | ⚠️ Often flagged as suspicious |
| **Linking** | Dynamic (uses Windows DLLs) | Static (bundles everything) |
| **Runtime Dependencies** | vcruntime140.dll (usually present) | None (self-contained) |
| **Windows Compatibility** | ✅ Native Windows toolchain | ✅ Compatible but non-standard |
| **Build Complexity** | Requires xwin + lld | Requires mingw-w64 |
| **Professional Use** | ✅ Industry standard | ⚠️ Less common |

## Why Antivirus Issues with MinGW?

1. **Static Linking**: MinGW binaries bundle all dependencies, making them larger and less common
2. **Packer Signatures**: The way MinGW packages executables can resemble malware packers
3. **Less Common**: Antivirus heuristics are trained on MSVC binaries (most Windows software)
4. **Missing Signature**: MSVC binaries follow Windows conventions better

## Real-World Example

```bash
# MSVC build
$ file target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
PE32+ executable (console) x86-64, for MS Windows, 6 sections
$ ls -lh target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
-rwxrwxr-x 365K midicwkeyer.exe  ✅ Small, clean

# MinGW build  
$ file target/x86_64-pc-windows-gnu/release/midicwkeyer.exe
PE32+ executable (console) x86-64 (stripped to external PDB), for MS Windows, 10 sections
$ ls -lh target/x86_64-pc-windows-gnu/release/midicwkeyer.exe
-rwxrwxr-x 2.4M midicwkeyer.exe  ⚠️ Larger, may trigger AV
```

## Build Instructions

### MSVC (Recommended)

```bash
# One-time setup
cargo install xwin
xwin --accept-license splat --output ~/.xwin
sudo apt install lld

# Build
cargo build --target x86_64-pc-windows-msvc --release
```

### MinGW (Fallback)

```bash
# One-time setup
sudo apt install mingw-w64
rustup target add x86_64-pc-windows-gnu

# Build
cargo build --target x86_64-pc-windows-gnu --release
```

## Recommendations

1. **Development**: Use either, MinGW is faster to set up
2. **Testing**: Test with MSVC if deploying to end users
3. **Production**: Always use MSVC to avoid support issues
4. **Distribution**: MSVC binaries are more acceptable to security software
5. **Code Signing**: MSVC binaries are better suited for code signing certificates
