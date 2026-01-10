#!/bin/bash
# Build script for MIDI CW Keyer - builds for Linux and Windows (MSVC)

set -e

echo "🔨 Building MIDI CW Keyer for multiple platforms..."
echo

# Build for Linux (native)
echo "📦 Building for Linux..."
cargo build --release
echo "✅ Linux build complete: target/release/midicwkeyer"
echo

# Build for Windows with MSVC (recommended - avoids antivirus false positives)
echo "📦 Building for Windows (MSVC - recommended)..."
cargo build --target x86_64-pc-windows-msvc --release
echo "✅ Windows MSVC build complete: target/x86_64-pc-windows-msvc/release/midicwkeyer.exe"
echo

# Optionally build with MinGW (larger, may trigger AV)
if [ "$BUILD_GNU" = "1" ]; then
    echo "📦 Building for Windows (GNU/MinGW)..."
    cargo build --target x86_64-pc-windows-gnu --release
    echo "✅ Windows GNU build complete: target/x86_64-pc-windows-gnu/release/midicwkeyer.exe"
    echo
fi

# Show file sizes
echo "📊 Build sizes:"
ls -lh target/release/midicwkeyer
ls -lh target/x86_64-pc-windows-msvc/release/midicwkeyer.exe
if [ "$BUILD_GNU" = "1" ]; then
    ls -lh target/x86_64-pc-windows-gnu/release/midicwkeyer.exe
fi
echo

echo "🎉 All builds complete!"
echo
echo "💡 Tip: MSVC build (~365 KB) is recommended for Windows to avoid antivirus issues"
echo "   MinGW build (~2.4 MB) may trigger false positives"
