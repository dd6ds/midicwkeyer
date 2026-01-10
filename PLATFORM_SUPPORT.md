# Platform Support and Build Matrix

## Quick Summary

✅ **Linux**: Build natively
✅ **Windows (MSVC)**: Cross-compile from Linux (recommended)
✅ **Windows (MinGW)**: Cross-compile from Linux (fallback)
✅ **macOS**: Use GitHub Actions (easiest)

## Detailed Build Matrix

| Platform | Target | Method | Complexity | Binary Size | Notes |
|----------|--------|--------|------------|-------------|-------|
| **Linux x86_64** | `x86_64-unknown-linux-gnu` | Native | ⭐ Easy | ~782 KB | Standard Linux build |
| **Windows x86_64 MSVC** | `x86_64-pc-windows-msvc` | Cross-compile | ⭐⭐ Medium | ~365 KB | ✅ Recommended for Windows |
| **Windows x86_64 MinGW** | `x86_64-pc-windows-gnu` | Cross-compile | ⭐ Easy | ~2.4 MB | ⚠️ May trigger antivirus |
| **macOS Intel** | `x86_64-apple-darwin` | GitHub Actions | ⭐ Easy | ~TBD | Intel Macs |
| **macOS Apple Silicon** | `aarch64-apple-darwin` | GitHub Actions | ⭐ Easy | ~TBD | M1/M2/M3 Macs |

## Build Commands by Platform

### Linux (Native)
```bash
cargo build --release
# Output: target/release/midicwkeyer
```

**Requirements**: None (native build)

### Windows MSVC (Cross-compile) ✅ Recommended
```bash
# One-time setup
cargo install xwin
xwin --accept-license splat --output ~/.xwin
sudo apt install lld

# Build
cargo build --target x86_64-pc-windows-msvc --release
# Output: target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
```

**Requirements**:
- xwin (cargo install xwin)
- lld linker
- Windows SDK (~1 GB download)

**Advantages**:
- Small binary size (365 KB)
- Rarely triggers antivirus
- Standard Microsoft toolchain

### Windows MinGW (Cross-compile)
```bash
# One-time setup
sudo apt install mingw-w64
rustup target add x86_64-pc-windows-gnu

# Build
cargo build --target x86_64-pc-windows-gnu --release
# Output: target/x86_64-pc-windows-gnu/release/midicwkeyer.exe
```

**Requirements**:
- mingw-w64 package

**Disadvantages**:
- Larger binary (2.4 MB)
- May trigger antivirus false positives
- Less standard on Windows

### macOS (GitHub Actions) ✅ Recommended
```bash
# Just push to GitHub!
git push origin main
```

The `.github/workflows/build.yml` will:
1. Build for macOS Intel (x86_64)
2. Build for macOS Apple Silicon (aarch64)
3. Create downloadable artifacts

**Requirements**:
- GitHub repository
- GitHub Actions enabled (free)

**Advantages**:
- No local setup needed
- Builds both Intel and ARM
- Uses real Apple hardware
- Free for public repos

### macOS (OSXCross) - Advanced
```bash
# Complex setup required - see MACOS_CROSS_COMPILE.md
# Not recommended unless you have specific needs
```

**Requirements**:
- osxcross toolchain
- macOS SDK (from Xcode)
- 1-2 hours setup time

## Workflow Overview

### Local Development
```
┌─────────────┐
│   Write     │
│   Code      │
└──────┬──────┘
       │
       ├─→ Test on Linux (cargo build --release)
       ├─→ Test on Windows (cargo build --target x86_64-pc-windows-msvc --release)
       │
       ▼
┌─────────────┐
│  Push to    │
│  GitHub     │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   GitHub    │
│   Actions   │
│  Builds:    │
│  - Linux    │
│  - Windows  │
│  - macOS    │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  Download   │
│  Artifacts  │
└─────────────┘
```

### CI/CD Pipeline (Automated)
```
Push to GitHub
    ↓
GitHub Actions triggers
    ↓
Parallel builds:
    ├─ Ubuntu runner → Linux binary
    ├─ Windows runner → Windows binary
    └─ macOS runner → macOS binaries (Intel + ARM)
    ↓
Artifacts uploaded
    ↓
Available for download in Actions tab
```

## Current Setup Status

| Component | Status | Location |
|-----------|--------|----------|
| Linux build | ✅ Working | Native |
| Windows MSVC cross-compile | ✅ Working | `.cargo/config.toml` |
| Windows MinGW cross-compile | ✅ Working | `.cargo/config.toml` |
| GitHub Actions workflow | ✅ Ready | `.github/workflows/build.yml` |
| OSXCross | ❌ Not set up | Not needed (use GitHub Actions) |

## Recommendations by Use Case

### For Development
- **Linux**: Use native build
- **Windows**: Test with MSVC cross-compile
- **macOS**: Use GitHub Actions

### For Distribution
- **All platforms**: Use GitHub Actions
  - Creates optimized binaries for all platforms
  - Consistent build environment
  - Easy to download from Releases

### For Testing
- **Quick iteration**: Build locally for Linux + Windows MSVC
- **Full testing**: Push to GitHub and test all artifacts

## Size Comparison

Final binary sizes (release builds):

```
Linux (x86_64):              782 KB
Windows MSVC (x86_64):       365 KB  ← Smallest!
Windows MinGW (x86_64):      2.4 MB  ← Largest
macOS Intel (x86_64):        ~500 KB (estimated)
macOS ARM (aarch64):         ~500 KB (estimated)
```

## Next Steps

1. **Set up GitHub repository** (if not already)
2. **Push code** to GitHub
3. **Watch Actions tab** for automatic builds
4. **Download artifacts** for testing
5. **Create releases** for distribution

The GitHub Actions workflow is ready to use immediately!
