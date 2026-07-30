#include "output_usb_f32.h"

#ifdef AUDIO_INTERFACE

// debug counters
volatile uint32_t usb_output_update_count = 0;
volatile uint32_t usb_output_fill_count = 0;

audio_block_f32_t *AudioOutputUSB_F32::txBuffer_f32[USBAudioOutInterface::ringTxBufferSize][USB_AUDIO_MAX_NO_CHANNELS];

AudioOutputUSB_F32::AudioOutputUSB_F32(void)
	: AudioStream_F32(USB_AUDIO_MAX_NO_CHANNELS, inputQueueArray_f32),
	  _usbInterface(releaseBlocks, isBlockReady, copy_from_buffers),
	  numChannels(USB_AUDIO_MAX_NO_CHANNELS)
{
	begin();
	_usbInterface.begin();
}

AudioOutputUSB_F32::AudioOutputUSB_F32(const AudioSettings_F32 &settings)
	: AudioStream_F32(USB_AUDIO_MAX_NO_CHANNELS, inputQueueArray_f32),
	  _usbInterface(releaseBlocks, isBlockReady, copy_from_buffers),
	  numChannels(USB_AUDIO_MAX_NO_CHANNELS)
{
	begin();
	_usbInterface.begin();
}

AudioOutputUSB_F32::AudioOutputUSB_F32(int nch)
	: AudioStream_F32(nch, inputQueueArray_f32),
	  _usbInterface(releaseBlocks, isBlockReady, copy_from_buffers),
	  numChannels(nch)
{
	begin();
	_usbInterface.begin();
}

AudioOutputUSB_F32::AudioOutputUSB_F32(const AudioSettings_F32 &settings, int nch)
	: AudioStream_F32(nch, inputQueueArray_f32),
	  _usbInterface(releaseBlocks, isBlockReady, copy_from_buffers),
	  numChannels(nch)
{
	begin();
	_usbInterface.begin();
}

void AudioOutputUSB_F32::begin(void)
{
	for (uint16_t i = 0; i < USBAudioOutInterface::ringTxBufferSize; i++) {
		for (uint16_t j = 0; j < USB_AUDIO_MAX_NO_CHANNELS; j++) {
			txBuffer_f32[i][j] = NULL;
		}
	}
}

void AudioOutputUSB_F32::update(void)
{
	int16_t bIdx = -1;
	uint16_t noChannels;
	usb_output_update_count++;
	_usbInterface.update(bIdx, noChannels);
	if (bIdx < 0) {
		for (uint16_t i = 0; i < USB_AUDIO_MAX_NO_CHANNELS; i++) {
			audio_block_f32_t *b = receiveReadOnly_f32(i);
			if (b) {
				AudioStream_F32::release(b);
			}
		}
	}
	usb_output_fill_count++;
	for (uint16_t i = 0; i < noChannels; i++) {
		if (txBuffer_f32[bIdx][i]) {
			AudioStream_F32::release(txBuffer_f32[bIdx][i]);
		}
		txBuffer_f32[bIdx][i] = receiveReadOnly_f32(i);
		if (!txBuffer_f32[bIdx][i]) {
			if (!txBuffer_f32[bIdx][i]) {
				txBuffer_f32[bIdx][i] = AudioStream_F32::allocate_f32();
			}
			if (txBuffer_f32[bIdx][i]) {
				memset(txBuffer_f32[bIdx][i]->data, 0, AUDIO_BLOCK_SAMPLES * sizeof(float));
			} else {
				releaseBlocks(bIdx, noChannels);
				break;
			}
		}
	}
	_usbInterface.incrementBufferIndex();
}

float AudioOutputUSB_F32::getBufferedSamples() const
{
	return _usbInterface.getBufferedSamples();
}

float AudioOutputUSB_F32::getBufferedSamplesSmooth() const
{
	return _usbInterface.getBufferedSamplesSmooth();
}

float AudioOutputUSB_F32::getActualBIntervalUs() const
{
	return _usbInterface.getActualBIntervalUs();
}

USBAudioOutInterface::Status AudioOutputUSB_F32::getStatus() const
{
	return _usbInterface.getStatus();
}

void AudioOutputUSB_F32::copy_from_buffers(uint8_t *dst, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len)
{
	int16_t *dst16 = (int16_t *)dst;
	for (uint32_t i = 0; i < len; i++) {
		for (uint16_t j = 0; j < noChannels; j++) {
			float sample = txBuffer_f32[bIdx][j]->data[count + i];
			if (sample > 1.0f) sample = 1.0f;
			else if (sample < -1.0f) sample = -1.0f;
			*dst16++ = (int16_t)(sample * 32767.0f);
		}
	}
}

void AudioOutputUSB_F32::releaseBlocks(uint16_t bIdx, uint16_t noChannels)
{
	for (uint16_t i = 0; i < noChannels; i++) {
		if (txBuffer_f32[bIdx][i]) {
			AudioStream_F32::release(txBuffer_f32[bIdx][i]);
			txBuffer_f32[bIdx][i] = NULL;
		}
	}
}

bool AudioOutputUSB_F32::isBlockReady(uint16_t bIdx, uint16_t channel)
{
	return txBuffer_f32[bIdx][channel] != NULL;
}

#endif
