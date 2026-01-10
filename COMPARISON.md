# Python vs Rust Implementation Comparison

## Key Differences

### Audio Output

**Python**: Uses PulseAudio-specific `pasimple` library
**Rust**: Uses cross-platform `cpal` library (works on Linux, macOS, Windows)

### MIDI Input

**Python**: Uses `mido` library with blocking receive
**Rust**: Uses `midir` with asynchronous callback-based processing

### Concurrency Model

**Python**: 
- Single-threaded with blocking I/O
- Synchronous sample generation

**Rust**:
- Multi-threaded with Arc<Mutex> for shared state
- Channel-based audio sample communication
- Non-blocking MIDI processing in callback

### State Management

**Python**: Global variables (wpm, dit_sample, dah_sample, etc.)
**Rust**: Encapsulated in `KeyerState` struct with thread-safe Arc<Mutex>

### Type Safety

**Python**: Dynamic typing, runtime errors possible
**Rust**: Static typing, compile-time safety guarantees

### Memory Management

**Python**: Garbage collected
**Rust**: Zero-cost abstractions, no garbage collector, ownership system

### Error Handling

**Python**: Exceptions and try/except
**Rust**: Result<T, E> with ? operator and anyhow for error context

## Features Not Yet Implemented

- Multiple audio outputs (Python has sidetone + TX channels)
