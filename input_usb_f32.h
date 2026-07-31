#ifndef input_usb_f32_h_
#define input_usb_f32_h_

#include <Arduino.h>
#include "AudioStream_F32.h"
#include <usb_audio_interface.h>

#ifdef AUDIO_INTERFACE

class AudioInputUSB_F32 : public AudioStream_F32
{
public:
	AudioInputUSB_F32(float kp = 400.f, float ki = 0.2f);
	AudioInputUSB_F32(const AudioSettings_F32 &settings, float kp = 400.f, float ki = 0.2f);
	virtual void update(void);
	void begin(void);
	static constexpr int getNumChannels() { return USB_AUDIO_MAX_NO_CHANNELS; }
	float getBufferedSamples() const;
	float getBufferedSamplesSmooth() const;
	float getRequestedSamplingFrequ() const;
	float getActualBIntervalUs() const;
	USBAudioInInterface::Status getStatus() const;

private:
	static void copy_to_buffers(const uint8_t *src, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len);
	static bool setBlockQuite(uint16_t bIdx, uint16_t channel);
	static void releaseBlock(uint16_t bIdx, uint16_t channel);
	static bool allocateBlock(uint16_t bIdx, uint16_t channel);
	static bool areBlocksReady(uint16_t bIdx, uint16_t noChannels);

	static audio_block_f32_t *rxBuffer_f32[USBAudioInInterface::ringRxBufferSize][USB_AUDIO_MAX_NO_CHANNELS];
	USBAudioInInterface _usbInterface;
};

#endif // AUDIO_INTERFACE
#endif // input_usb_f32_h_
