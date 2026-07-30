#include <Audio.h>
#include <OpenAudio_ArduinoLibrary.h>
#include "USB_Audio_F32.h"

AudioInputUSBOct_F32    usb_in;
AudioOutputUSBOct_F32   usb_out;

AudioConnection_F32      patch0(usb_in, 0, usb_out, 0);
AudioConnection_F32      patch1(usb_in, 1, usb_out, 1);
AudioConnection_F32      patch2(usb_in, 2, usb_out, 2);
AudioConnection_F32      patch3(usb_in, 3, usb_out, 3);
AudioConnection_F32      patch4(usb_in, 4, usb_out, 4);
AudioConnection_F32      patch5(usb_in, 5, usb_out, 5);
AudioConnection_F32      patch6(usb_in, 6, usb_out, 6);
AudioConnection_F32      patch7(usb_in, 7, usb_out, 7);

// USB->USB requires an audio output ISR to drive the scheduler.
#include "output_i2s_f32.h"
AudioOutputI2S_F32       i2s_out;
AudioConnection_F32      patch8(usb_in, 0, i2s_out, 0);
AudioConnection_F32      patch9(usb_in, 1, i2s_out, 1);

void setup() {
  AudioMemory(100);
  AudioMemory_F32(100);
}

void loop() {
  
}
