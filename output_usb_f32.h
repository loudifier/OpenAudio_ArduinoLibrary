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
	virtual void update(void);
	void begin(void);
	static constexpr int getNumChannels() { return USB_AUDIO_MAX_NO_CHANNELS; }
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
};

#endif // AUDIO_INTERFACE
#endif // output_usb_f32_h_
