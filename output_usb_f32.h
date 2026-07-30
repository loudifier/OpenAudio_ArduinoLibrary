#ifndef output_usb_f32_h_
#define output_usb_f32_h_

#include <Arduino.h>
#include "AudioStream_F32.h"
#include <usb_audio_interface.h>

#ifdef AUDIO_INTERFACE

class AudioOutputUSB_F32 : public AudioStream_F32
{
public:
	AudioOutputUSB_F32(void);
	AudioOutputUSB_F32(const AudioSettings_F32 &settings);
	AudioOutputUSB_F32(int nch);
	AudioOutputUSB_F32(const AudioSettings_F32 &settings, int nch);
	virtual void update(void);
	void begin(void);
	float getBufferedSamples() const;
	float getBufferedSamplesSmooth() const;
	float getActualBIntervalUs() const;
	USBAudioOutInterface::Status getStatus() const;

private:
	static void copy_from_buffers(uint8_t *dst, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len);
	static void releaseBlocks(uint16_t bIdx, uint16_t noChannels);
	static bool isBlockReady(uint16_t bIdx, uint16_t channel);

	static audio_block_f32_t *txBuffer_f32[USBAudioOutInterface::ringTxBufferSize][USB_AUDIO_MAX_NO_CHANNELS];
	audio_block_f32_t *inputQueueArray_f32[USB_AUDIO_MAX_NO_CHANNELS];
	USBAudioOutInterface _usbInterface;
	int numChannels;
};

#if USB_AUDIO_NO_CHANNELS_480 >= 4
class AudioOutputUSBQuad_F32 : public AudioOutputUSB_F32 { public: AudioOutputUSBQuad_F32(void) : AudioOutputUSB_F32(4) {} };
#if USB_AUDIO_NO_CHANNELS_480 >= 6
class AudioOutputUSBHex_F32 : public AudioOutputUSB_F32 { public: AudioOutputUSBHex_F32(void) : AudioOutputUSB_F32(6) {} };
#if USB_AUDIO_NO_CHANNELS_480 >= 8
class AudioOutputUSBOct_F32 : public AudioOutputUSB_F32 { public: AudioOutputUSBOct_F32(void) : AudioOutputUSB_F32(8) {} };
#endif
#endif
#endif

#endif // AUDIO_INTERFACE
#endif // output_usb_f32_h_
