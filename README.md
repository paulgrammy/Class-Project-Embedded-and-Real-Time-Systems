# ees-ss26-project

EES-Repository for the class project.

# Configurable Signal Generator (FreeRTOS / MicroBlaze)

Kursprojekt – Eingebettete Systeme und Echtzeitsysteme (SS 2026), TH Mannheim
Authors: M. Heinzmann, P. Gramescu

A configurable multi-channel signal generator running on a FreeRTOS-based MicroBlaze system. Signal parameters are sent over UART from a PC, parsed into a waveform configuration, and used to generate samples that are streamed via SPI to a DAC (PmodDA2).

## Overview

- Configuration strings are received over **UART**, parsed, and validated.
- Each sample period, the current waveform state is advanced and the next DAC value is computed.
- Samples are sent over **SPI** to a PmodDA2 DAC via ETwinSPI.
- Two channels are supported, each independently configurable at runtime without discontinuities in the output.

## Configuration String Format Example
#c1wSf100p0a100o2048.
| Tag | Meaning              |
|-----|----------------------|
| `#` | Start marker         |
| `c` | Channel select       |
| `w` | Waveform select      |
| `f` | Frequency            |
| `p` | Phase shift          |
| `a` | Amplitude            |
| `o` | Offset               |
| `.` | End marker           |

Supported waveforms: square, triangle, sawtooth, sine.

## Architecture
- **parser** – Locates tags in the incoming string (`get_tags`) and converts the substrings between them into a `waveform_t` struct (`parse_settings`). Returns an error if tags are missing or out of order.
- **waveforms** – Hardware signal generation. `waveforms_init_channel()` sets up a phase accumulator (frequency/amplitude clamping, phase in radians, computes phase increment Δφ = 2π·f_signal/f_sample). `waveforms_tick()` advances the phase and returns the next DAC sample; the sine wave uses a parabolic approximation to avoid trigonometric functions. Output is scaled and clamped to the 12-bit DAC range (0–4095).
- **task_wave** – Interfaces `waveforms` to the SPI hardware, driven by a hardware timer.
- **task_uart** – Interfaces `parser` to the UART hardware; implements a `WAITING` / `RECEIVING` / `DISCARDING` state machine, plus a `help` command.

## Interrupts

- **UART RX ISR** – Reads all bytes from the RX FIFO and pushes them to a queue (`xQueueSendToBackFromISR`). No parsing is done in the ISR; all processing happens in `task_uart`.
- **Timer ISR (50 kHz sample rate)** – Only wakes `task_wave` via `vTaskNotifyGiveFromISR()` and clears the interrupt flag. All computation (float math, SPI transfer) happens in task_wave.

## Tasks

- **task_uart** – Lower priority. Assembles the configuration string, then calls `parse_settings()` and forwards the result to `task_wave` via queue.
- **task_wave** – Higher priority, event-driven (`ulTaskNotifyTake()`), triggered by the 50 kHz timer. On each notification: checks for a new configuration (non-blocking), re-initializes the phase accumulator on channel changes (phase reset to avoid signal jumps), computes the next sample per channel via `waveforms_tick()`, and outputs it over SPI (big-endian).
