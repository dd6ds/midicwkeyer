use anyhow::{Context, Result};
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use midir::MidiInput;
use std::collections::HashMap;
use std::io::{self, Write};
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::{Duration, Instant};

// Constants
const SAMPLE_RATE: f32 = 48000.0;
const RAMP_MS: f32 = 12.0; // at least 6ms ramp up/down (CWops recommendation)
const FREQ: f32 = 850.0;
const VOLUME: f32 = 100.0;

// Global state (using Arc<Mutex> for thread-safe access)
#[derive(Clone)]
struct KeyerState {
    wpm: usize,
    dot_duration: f32,
    dit_sample: Vec<u8>,
    dah_sample: Vec<u8>,
    paddles: [bool; 3], // index 0 unused, 1=dit, 2=dah
}

impl KeyerState {
    fn new() -> Self {
        let mut state = KeyerState {
            wpm: 24,
            dot_duration: 0.0,
            dit_sample: Vec::new(),
            dah_sample: Vec::new(),
            paddles: [false, false, false],
        };
        state.upload_samples(24);
        state
    }

    fn upload_samples(&mut self, speed: usize) {
        let dot_duration_ms = 1200 / speed;
        let dah_duration_ms = 3 * dot_duration_ms;

        self.wpm = speed;
        self.dot_duration = dot_duration_ms as f32 / 1000.0;
        self.dit_sample = generate_sample(dot_duration_ms);
        self.dah_sample = generate_sample(dah_duration_ms);
    }
}

fn generate_sample(duration_ms: usize) -> Vec<u8> {
    let ramp_samples = ((RAMP_MS / 1000.0) * SAMPLE_RATE) as usize;
    let num_samples = (((duration_ms as f32 + RAMP_MS) / 1000.0) * SAMPLE_RATE) as usize;

    let mut audio = Vec::with_capacity(num_samples + ramp_samples);

    // Ramp up with cosine envelope
    for x in 0..ramp_samples {
        let envelope = 0.5 * (1.0 - (std::f32::consts::PI * x as f32 / ramp_samples as f32).cos());
        let sine = (2.0 * std::f32::consts::PI * FREQ * (x as f32 / SAMPLE_RATE)).sin();
        audio.push((128.0 + VOLUME * envelope * sine) as u8);
    }

    // Main body (full volume)
    for x in ramp_samples..num_samples {
        let sine = (2.0 * std::f32::consts::PI * FREQ * (x as f32 / SAMPLE_RATE)).sin();
        audio.push((128.0 + VOLUME * sine) as u8);
    }

    // Ramp down with cosine envelope
    for x in 0..ramp_samples {
        let envelope = 0.5 * (1.0 + (std::f32::consts::PI * x as f32 / ramp_samples as f32).cos());
        let sine = (2.0 * std::f32::consts::PI * FREQ * ((x + num_samples) as f32 / SAMPLE_RATE)).sin();
        audio.push((128.0 + VOLUME * envelope * sine) as u8);
    }

    audio
}

fn get_morse_table() -> HashMap<String, &'static str> {
    let mut morse = HashMap::new();
    morse.insert(".-".to_string(), "A");
    morse.insert(".-.-".to_string(), "Ä");
    morse.insert("-...".to_string(), "B");
    morse.insert("-.-.".to_string(), "C");
    morse.insert("----".to_string(), "CH");
    morse.insert("-..".to_string(), "D");
    morse.insert(".".to_string(), "E");
    morse.insert("..-.".to_string(), "F");
    morse.insert("--.".to_string(), "G");
    morse.insert("....".to_string(), "H");
    morse.insert("..".to_string(), "I");
    morse.insert(".---".to_string(), "J");
    morse.insert("-.-".to_string(), "K");
    morse.insert(".-..".to_string(), "L");
    morse.insert("--".to_string(), "M");
    morse.insert("-.".to_string(), "N");
    morse.insert("---".to_string(), "O");
    morse.insert("---.".to_string(), "Ö");
    morse.insert(".--.".to_string(), "P");
    morse.insert("--.-".to_string(), "Q");
    morse.insert(".-.".to_string(), "R");
    morse.insert("...".to_string(), "S");
    morse.insert("-".to_string(), "T");
    morse.insert("..-".to_string(), "U");
    morse.insert("..--".to_string(), "Ü");
    morse.insert("...-".to_string(), "V");
    morse.insert(".--".to_string(), "W");
    morse.insert("-..-".to_string(), "X");
    morse.insert("-.--".to_string(), "Y");
    morse.insert("--..".to_string(), "Z");
    morse.insert("-----".to_string(), "0");
    morse.insert(".----".to_string(), "1");
    morse.insert("..---".to_string(), "2");
    morse.insert("...--".to_string(), "3");
    morse.insert("....-".to_string(), "4");
    morse.insert(".....".to_string(), "5");
    morse.insert("-....".to_string(), "6");
    morse.insert("--...".to_string(), "7");
    morse.insert("---..".to_string(), "8");
    morse.insert("----.".to_string(), "9");
    morse.insert(".-.-.".to_string(), "+");
    morse.insert("--..--".to_string(), ",");
    morse.insert("-....-".to_string(), "-");
    morse.insert(".-.-.-".to_string(), ".");
    morse.insert("-..-.".to_string(), "/");
    morse.insert("---...".to_string(), ";");
    morse.insert("-...-".to_string(), "=");
    morse.insert("..--..".to_string(), "?");
    morse.insert(".--.-.".to_string(), "@");
    morse.insert(".-...".to_string(), "<AS>");
    morse.insert("...-.-".to_string(), "<SK>");
    morse
}

fn erase_chars(chars: usize) {
    for _ in 0..chars {
        print!("\x08 \x08");
    }
    io::stdout().flush().unwrap();
}

#[derive(Debug, Clone, Copy, PartialEq)]
enum State {
    Idle,
    Dit,
    Dah,
    PauseBeforeDit,
    PauseBeforeDah,
    PauseAfterDit,
    PauseAfterDah,
}

fn keyer_loop(
    state: Arc<Mutex<KeyerState>>,
    audio_tx: std::sync::mpsc::Sender<Vec<u8>>,
) -> Result<()> {
    let morse = get_morse_table();
    let mut current_state = State::Idle;
    let mut sign = String::new();
    let mut sign_len = 0;

    loop {
        // Play sample for current state
        {
            let state_lock = state.lock().unwrap();
            match current_state {
                State::Dit => {
                    print!(".");
                    io::stdout().flush().unwrap();
                    sign.push('.');
                    sign_len += 1;
                    audio_tx.send(state_lock.dit_sample.clone()).ok();
                }
                State::Dah => {
                    print!("-");
                    io::stdout().flush().unwrap();
                    sign.push('-');
                    sign_len += 1;
                    audio_tx.send(state_lock.dah_sample.clone()).ok();
                }
                _ => {}
            }
        }

        // Sleep for state duration
        {
            let state_lock = state.lock().unwrap();
            match current_state {
                State::Idle => {}
                State::Dah => thread::sleep(Duration::from_secs_f32(3.0 * state_lock.dot_duration)),
                _ => thread::sleep(Duration::from_secs_f32(state_lock.dot_duration)),
            }
        }

        // Read paddle input
        let (dit, dah) = {
            let state_lock = state.lock().unwrap();
            (state_lock.paddles[1], state_lock.paddles[2])
        };

        // State machine transitions
        current_state = match current_state {
            State::Idle => {
                if dit {
                    State::Dit
                } else if dah {
                    State::Dah
                } else {
                    // Decode character when idle
                    if let Some(char) = morse.get(&sign) {
                        erase_chars(sign_len);
                        print!("{}", char);
                        io::stdout().flush().unwrap();
                        sign_len = char.len();
                    } else if !sign.is_empty() {
                        print!(" ");
                        io::stdout().flush().unwrap();
                        sign_len += 1;
                    }

                    // Wait for next input with timeout
                    let start = Instant::now();
                    thread::sleep(Duration::from_millis(50)); // Small polling delay
                    let delay = start.elapsed().as_secs_f32();

                    let (dit_new, dah_new) = {
                        let state_lock = state.lock().unwrap();
                        (state_lock.paddles[1], state_lock.paddles[2])
                    };

                    let dot_duration = state.lock().unwrap().dot_duration;

                    if delay <= dot_duration && (dit_new || dah_new) {
                        // Character continued
                        erase_chars(sign_len);
                        print!("{}", sign);
                        io::stdout().flush().unwrap();
                        sign_len = sign.len();
                        State::Idle
                    } else {
                        if delay >= 5.0 * dot_duration {
                            print!(" ");
                            io::stdout().flush().unwrap();
                        }
                        sign.clear();
                        sign_len = 0;
                        State::Idle
                    }
                }
            }
            State::PauseBeforeDit => State::Dit,
            State::PauseBeforeDah => State::Dah,
            State::Dit => {
                if dah {
                    State::PauseBeforeDah
                } else {
                    State::PauseAfterDit
                }
            }
            State::Dah => {
                if dit {
                    State::PauseBeforeDit
                } else {
                    State::PauseAfterDah
                }
            }
            State::PauseAfterDit => {
                if dah {
                    State::Dah
                } else {
                    State::Idle
                }
            }
            State::PauseAfterDah => {
                if dit {
                    State::Dit
                } else {
                    State::Idle
                }
            }
        };
    }
}

fn main() -> Result<()> {
    println!("MIDI CW Keyer - Rust Implementation");
    println!("Based on midicwkeyer.py by DF7CB");
    println!();

    // Initialize keyer state
    let state = Arc::new(Mutex::new(KeyerState::new()));

    // Setup audio output
    let host = cpal::default_host();
    let device = host
        .default_output_device()
        .context("No output device available")?;

    println!("Using audio device: {}", device.name()?);

    let config = device.default_output_config()?;
    println!("Audio config: {:?}", config);

    // Channel for sending audio samples
    let (audio_tx, audio_rx) = std::sync::mpsc::channel::<Vec<u8>>();

    // Build audio stream
    let stream = device.build_output_stream(
        &config.into(),
        move |data: &mut [u8], _: &cpal::OutputCallbackInfo| {
            if let Ok(sample) = audio_rx.try_recv() {
                let len = std::cmp::min(data.len(), sample.len());
                data[..len].copy_from_slice(&sample[..len]);
            } else {
                // Fill with silence
                data.fill(128);
            }
        },
        move |err| eprintln!("Audio stream error: {}", err),
        None,
    )?;

    stream.play()?;

    // Setup MIDI input
    let midi_in = MidiInput::new("midicwkeyer")?;
    let ports = midi_in.ports();

    println!("\nAvailable MIDI ports:");
    for (i, port) in ports.iter().enumerate() {
        println!("  {}: {}", i, midi_in.port_name(port)?);
    }

    if ports.is_empty() {
        eprintln!("No MIDI input ports available!");
        return Ok(());
    }

    // Connect to first available port (or modify to select specific port)
    let port = &ports[0];
    println!("\nConnecting to: {}", midi_in.port_name(port)?);

    let state_clone = Arc::clone(&state);
    let _conn_in = midi_in
        .connect(
            port,
            "midicwkeyer-input",
            move |_timestamp, message, _| {
                if message.len() >= 3 {
                    let msg_type = message[0] & 0xF0;
                    let note = message[1];
                    let velocity = message[2];

                    // Note on/off for paddles (note 1 = dit, note 2 = dah)
                    if (msg_type == 0x90 || msg_type == 0x80) && (note == 1 || note == 2) {
                        let is_on = msg_type == 0x90 && velocity > 0;
                        let mut state = state_clone.lock().unwrap();
                        state.paddles[note as usize] = is_on;
                    }

                    // Control change for WPM adjustment (controller 61 = 0x3D)
                    if msg_type == 0xB0 && note == 0x3D {
                        let wpm = 6 + ((42.0 * velocity as f32) / 127.0).round() as usize;
                        let mut state = state_clone.lock().unwrap();
                        if wpm != state.wpm {
                            print!("<{}>", wpm);
                            io::stdout().flush().unwrap();
                            state.upload_samples(wpm);
                        }
                    }
                }
            },
            (),
        )
        .map_err(|e| anyhow::anyhow!("Failed to connect to MIDI port: {:?}", e))?;

    println!("\nKeyer running at {} WPM. Press Ctrl+C to exit.", state.lock().unwrap().wpm);
    println!("Waiting for paddle input...\n");

    // Run the keyer loop in main thread
    keyer_loop(state, audio_tx)?;

    Ok(())
}
