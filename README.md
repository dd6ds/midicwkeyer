# MIDI CW Keyer - Rust Implementation

A Morse code keyer with MIDI input and audio output, converted from Python to Rust.

## Original Project

Based on [midicwkeyer.py](https://github.com/df7cb/df7cb-shack/blob/master/midicwkeyer/midicwkeyer.py) by Christoph Berg DF7CB.

## Features

- MIDI input for paddle keying (dit/dah)
- Real-time audio tone generation (850 Hz)
- Automatic Morse code decoding and display
- Adjustable WPM (words per minute) via MIDI control change
- Cosine-ramped audio envelope for clean keying
- State machine-based iambic keyer logic

## Building

```bash
cargo build --release
```

The compiled binary will be at: `target/release/midicwkeyer`

## Running

```bash
cargo run --release
```

Or directly:
```bash
./target/release/midicwkeyer
```

## MIDI Configuration

The program will list available MIDI ports and connect to the first one.
You can modify the code to select a specific port.

### Expected MIDI Messages

- **Note 1 (On/Off)**: Dit paddle
- **Note 2 (On/Off)**: Dah paddle  
- **Control Change 61 (0x3D)**: WPM adjustment (6-48 WPM)

## Dependencies

- `midir` - MIDI input handling
- `cpal` - Cross-platform audio I/O

## Cross-Compilation to Windows

From Linux, you can build for Windows using MSVC (recommended) or MinGW:

```bash
# Build for Windows with MSVC (RECOMMENDED - avoids antivirus issues)
cargo build --target x86_64-pc-windows-msvc --release
# Output: target/x86_64-pc-windows-msvc/release/midicwkeyer.exe (~365 KB)

# Alternative: Build with MinGW (may trigger antivirus false positives)
cargo build --target x86_64-pc-windows-gnu --release
# Output: target/x86_64-pc-windows-gnu/release/midicwkeyer.exe (~2.4 MB)
```

**Why MSVC?** MSVC-compiled binaries are smaller, use standard Windows libraries, and are much less likely to trigger antivirus false positives.

### Prerequisites for Cross-Compilation

**For MSVC (recommended):**
1. Install xwin (downloads Windows SDK):
   ```bash
   cargo install xwin
   ```

2. Download MSVC SDK (one-time setup):
   ```bash
   xwin --accept-license splat --output ~/.xwin
   ```

3. Install LLD linker:
   ```bash
   sudo apt install lld  # Debian/Ubuntu
   sudo dnf install lld  # Fedora/RHEL
   ```

**For MinGW (alternative):**
1. Install MinGW cross-compiler:
   ```bash
   sudo apt install mingw-w64  # Debian/Ubuntu
   sudo dnf install mingw64-gcc mingw64-winpthreads-static  # Fedora/RHEL
   ```

2. Add Windows target to Rust:
   ```bash
   rustup target add x86_64-pc-windows-msvc  # For MSVC
   rustup target add x86_64-pc-windows-gnu   # For MinGW
   ```

The project includes `.cargo/config.toml` with all necessary linker configurations.

## Building for Multiple Platforms

```bash
# Linux (native)
cargo build --release

# Windows MSVC (recommended - smaller, fewer AV issues)
cargo build --target x86_64-pc-windows-msvc --release

# Windows MinGW (alternative - larger, may trigger AV)
cargo build --target x86_64-pc-windows-gnu --release
```

## Binaries Location

- **Linux**: `target/release/midicwkeyer`
- **Windows (MSVC)**: `target/x86_64-pc-windows-msvc/release/midicwkeyer.exe` ✅ Recommended
- **Windows (MinGW)**: `target/x86_64-pc-windows-gnu/release/midicwkeyer.exe`

## Building for macOS

### Option 1: GitHub Actions (Recommended)

The project includes a GitHub Actions workflow that automatically builds for macOS (both Intel and Apple Silicon).

Push to GitHub and the workflow will create binaries for:
- Linux x86_64
- macOS x86_64 (Intel)
- macOS aarch64 (Apple Silicon)
- Windows x86_64 (MSVC)

Artifacts are available in the Actions tab after each push.

### Option 2: Native Build on Mac

If you have access to a Mac:
```bash
cargo build --release
```

### Option 3: Cross-compile from Linux

See `MACOS_CROSS_COMPILE.md` for OSXCross setup (complex, not recommended).

## Platform Support Summary

| Platform | Build Method | Status |
|----------|--------------|--------|
| Linux x86_64 | Native | ✅ Working |
| Windows x86_64 (MSVC) | Cross-compile (xwin) | ✅ Working |
| Windows x86_64 (MinGW) | Cross-compile (mingw-w64) | ✅ Working |
| macOS x86_64 (Intel) | GitHub Actions | ✅ Automated |
| macOS aarch64 (M1/M2/M3) | GitHub Actions | ✅ Automated |
