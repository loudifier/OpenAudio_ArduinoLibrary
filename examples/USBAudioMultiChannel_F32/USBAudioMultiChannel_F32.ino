#include <Audio.h>
#include <OpenAudio_ArduinoLibrary.h>
#include "USB_Audio_F32.h"

// Set audio parameters using Tools menu
// USB type must be Audio or Serial + MIDI + Audio
// Number of channels
// Sample rate
// Bit depth

// Both classes automatically expose the Tools > USB channels count.
AudioInputUSB_F32      usb_in;
AudioOutputUSB_F32     usb_out;

// USB loopback connections are created automatically in setup(),
// one per configured channel (getNumChannels() is a compile-time constant).
constexpr int USB_CHANNELS = AudioInputUSB_F32::getNumChannels();
AudioConnection_F32   *usb_patches[USB_CHANNELS];

// USB->USB requires an audio output ISR to drive the scheduler.
#include "output_i2s_f32.h"
AudioOutputI2S_F32       i2s_out;
AudioConnection_F32      s0(usb_in, 0, i2s_out, 0);
AudioConnection_F32      s1(usb_in, 1, i2s_out, 1);

void setup() {
  for (int i = 0; i < USB_CHANNELS; i++) {
    usb_patches[i] = new AudioConnection_F32(usb_in, i, usb_out, i);
  }
  AudioMemory(100);
  AudioMemory_F32(100);
}

void loop() {
}
