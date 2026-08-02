/*
 * Demonstration of AudioOutputI2Ssink_F32: an I2S output that does NOT
 * generate the bit clock (BCLK) or frame sync (FS).
 *
 * The audio data appears on pin 7 (SAI1_TX_DATA0).  Pins 21 (BCLK) and
 * 20 (FS) must be driven by an external clock SOURCE - a second Teensy
 * running a normal I2S output (e.g. AudioOutputI2S_F32_Example), a codec
 * in its clock-source role, an FPGA, etc.  This Teensy only listens to
 * the clocks and shifts out its data.  Connect GND between the devices.
 *
 * The clock source must provide a bit clock of 64 x fs (2 slots x 32 bits)
 * and a 1 x fs frame sync (left-justified, word-high-then-low), which is
 * exactly what another Teensy's AudioOutputI2S_F32 produces.
 *
 * Wiring:
 *   Data out  : pin 7
 *   BCLK in   : pin 21  (external clock source)
 *   FS in     : pin 20  (external clock source)
 *   MCLK      : pin 23  (not used in sink mode)
 *
 * The sink's DMA interrupt drives the audio block scheduler, so the audio
 * is clocked by the external clock source.
 *
 * Expected result: the sine tone is present on pin 7 whenever the external
 * clock source is running.  You can watch it with a scope, or feed pin 7
 * into a second Teensy running AudioInputI2S_F32.
 */

#include <Arduino.h>
#include <Audio.h>
#include <OpenAudio_ArduinoLibrary.h>
#include <AudioStream_F32.h>

AudioSynthWaveformSine_F32 tone0;
AudioOutputI2Ssink_F32 i2s_out;

AudioConnection_F32 connect0(tone0, 0, i2s_out, 0);
AudioConnection_F32 connect1(tone0, 0, i2s_out, 1);

void setup() {
  delay(1000);

  AudioMemory_F32(10);

  tone0.amplitude(0.5);
  tone0.frequency(440.0);
  tone0.begin();
}

void loop() {
  delay(1000);
}
