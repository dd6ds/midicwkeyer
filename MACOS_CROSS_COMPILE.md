# macOS Cross-Compilation from Linux

## Current Status: ❌ Not Configured

Cross-compiling to macOS (`x86_64-apple-darwin`) from Linux requires significant setup.

## Why It Failed

The error shows that the regular Linux `cc` compiler doesn't understand macOS-specific flags:
- `-framework` (macOS framework linking)
- `-arch` (macOS architecture flag)
- `-mmacosx-version-min` (minimum macOS version)

These are Apple/Clang-specific flags that require the macOS toolchain.

## Requirements for macOS Cross-Compilation

### 1. OSXCross Toolchain
- Cross-compilation toolchain for macOS
- GitHub: https://github.com/tpoechtrager/osxcross
- Provides macOS-compatible linker and tools

### 2. macOS SDK
- Apple's SDK (headers, libraries, frameworks)
- **Legal requirement**: Must extract from Xcode (requires macOS or Mac)
- Cannot be redistributed legally

### 3. Time Investment
- Initial setup: 1-2 hours
- Requires downloading Xcode or extracting SDK from Mac

## Options for Building macOS Binaries

### Option 1: Use GitHub Actions (Recommended)
**Easiest and most reliable approach**

Create `.github/workflows/build.yml`:
```yaml
name: Build
on: [push]
jobs:
  build-macos:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v3
      - uses: dtolnay/rust-toolchain@stable
      - run: cargo build --release
      - uses: actions/upload-artifact@v3
        with:
          name: midicwkeyer-macos
          path: target/release/midicwkeyer
```

**Advantages:**
- ✅ No local setup needed
- ✅ Free for public repos
- ✅ Official Apple hardware
- ✅ Automatic builds on commit

### Option 2: Access to a Mac
- Build natively on macOS
- Most straightforward approach
- Guaranteed compatibility

### Option 3: Install OSXCross (Advanced)

**Warning**: Complex setup, takes 1-2 hours

#### Step 1: Install dependencies
```bash
sudo apt install clang cmake libxml2-dev libssl-dev \
                 lzma-dev libbz2-dev libz-dev
```

#### Step 2: Clone and build osxcross
```bash
cd /tmp
git clone https://github.com/tpoechtrager/osxcross
cd osxcross
```

#### Step 3: Get macOS SDK
You need to extract the SDK from Xcode. Two options:

**A. On a Mac:**
```bash
# Download Xcode from App Store
# Then run:
./tools/gen_sdk_package_pbzx.sh
# This creates MacOSX<version>.sdk.tar.xz
```

**B. Alternative (Community SDKs):**
Some pre-packaged SDKs exist but check licensing.

#### Step 4: Build toolchain
```bash
mv MacOSX*.sdk.tar.xz tarballs/
UNATTENDED=1 ./build.sh
```

#### Step 5: Configure Cargo
Add to `.cargo/config.toml`:
```toml
[target.x86_64-apple-darwin]
linker = "x86_64-apple-darwin20.4-clang"
ar = "x86_64-apple-darwin20.4-ar"
rustflags = [
    "-C", "link-arg=-mmacosx-version-min=10.12"
]
```

## Recommendation

For this project, I recommend **Option 1 (GitHub Actions)** because:

1. **No complex setup** - Works immediately
2. **Free** - GitHub Actions provides free macOS runners
3. **Reliable** - Uses actual Apple hardware
4. **Automated** - Builds on every commit
5. **Multiple platforms** - Can build Linux, Windows, and macOS in one workflow

## Current Build Support

| Platform | Status | Method |
|----------|--------|--------|
| Linux | ✅ Working | Native build |
| Windows (MSVC) | ✅ Working | Cross-compile (xwin) |
| Windows (MinGW) | ✅ Working | Cross-compile (mingw-w64) |
| macOS | ⚠️ Needs setup | GitHub Actions recommended |

## Next Steps

Would you like me to:
1. Set up GitHub Actions for automated macOS builds? (Easiest)
2. Create a comprehensive OSXCross setup script? (Advanced)
3. Document native macOS build instructions? (If you have Mac access)

Choose option 1 for the quickest path to macOS binaries!
