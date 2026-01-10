# Quick Build Reference

## TL;DR

```bash
# Build everything locally (Linux + Windows)
./build.sh

# Build for macOS
git push  # GitHub Actions handles it automatically
```

## One-Line Commands

### Linux
```bash
cargo build --release
```

### Windows (MSVC - Recommended)
```bash
cargo build --target x86_64-pc-windows-msvc --release
```

### Windows (MinGW - Fallback)
```bash
cargo build --target x86_64-pc-windows-gnu --release
```

### macOS (Both Intel and ARM)
```bash
git push origin main  # GitHub Actions builds it
```

## Output Locations

```
target/
├── release/
│   └── midicwkeyer                                          # Linux
├── x86_64-pc-windows-msvc/release/
│   └── midicwkeyer.exe                                      # Windows MSVC ✅
└── x86_64-pc-windows-gnu/release/
    └── midicwkeyer.exe                                      # Windows MinGW
```

For macOS: Check GitHub Actions → Artifacts tab

## Quick Setup (First Time)

### Windows MSVC Cross-Compile
```bash
cargo install xwin
xwin --accept-license splat --output ~/.xwin
sudo apt install lld
```

### Windows MinGW Cross-Compile
```bash
sudo apt install mingw-w64
rustup target add x86_64-pc-windows-gnu
```

### macOS (GitHub Actions)
```bash
# Just push to GitHub - no setup needed!
git remote add origin https://github.com/yourusername/midicwkeyer.git
git push -u origin main
```

## Common Tasks

### Build everything locally
```bash
./build.sh
```

### Clean rebuild
```bash
cargo clean
./build.sh
```

### Check all builds work
```bash
cargo check --release                                    # Linux
cargo check --target x86_64-pc-windows-msvc --release  # Windows
```

### Strip binaries (reduce size)
```bash
strip target/release/midicwkeyer
strip target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
```

## File Sizes Reference

| Platform | Size | Note |
|----------|------|------|
| Linux | ~782 KB | Native |
| Windows MSVC | ~365 KB | ✅ Recommended |
| Windows MinGW | ~2.4 MB | ⚠️ AV issues |
| macOS | ~500 KB | Via GitHub Actions |

## Troubleshooting

### "linker `link.exe` not found" (Windows MSVC)
- Solution: Use MSVC target, not gnu
- Command: `cargo build --target x86_64-pc-windows-msvc --release`

### "framework: unrecognized option" (macOS)
- Solution: Use GitHub Actions, not local cross-compile
- See: `MACOS_CROSS_COMPILE.md`

### Antivirus flags Windows binary
- Solution: Use MSVC target instead of MinGW
- See: `MSVC_VS_MINGW.md`

## Full Documentation

- `README.md` - Main project documentation
- `PLATFORM_SUPPORT.md` - Detailed platform info
- `CROSS_COMPILE.md` - Cross-compilation guide
- `MACOS_CROSS_COMPILE.md` - macOS-specific info
- `MSVC_VS_MINGW.md` - MSVC vs MinGW comparison
- `MSVC_SETUP_COMPLETE.md` - MSVC setup summary
