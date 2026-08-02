#include "input_usb_f32.h"

#ifdef AUDIO_INTERFACE

// debug counters
volatile uint32_t usb_input_update_count = 0;
volatile uint32_t usb_input_tx_count = 0;

audio_block_f32_t *AudioInputUSB_F32::rxBuffer_f32[USBAudioInInterface::ringRxBufferSize][USB_AUDIO_MAX_NO_CHANNELS];

AudioInputUSB_F32::AudioInputUSB_F32(float kp, float ki)
	: AudioStream_F32(0, NULL), _usbInterface(setBlockQuite, releaseBlock, allocateBlock, areBlocksReady, copy_to_buffers, kp, ki)
{
	for (uint16_t i = 0; i < USBAudioInInterface::ringRxBufferSize; i++) {
		for (uint16_t j = 0; j < USB_AUDIO_MAX_NO_CHANNELS; j++) {
			rxBuffer_f32[i][j] = NULL;
		}
	}
	_usbInterface.begin();
}

AudioInputUSB_F32::AudioInputUSB_F32(const AudioSettings_F32 &settings, float kp, float ki)
	: AudioStream_F32(0, NULL), _usbInterface(setBlockQuite, releaseBlock, allocateBlock, areBlocksReady, copy_to_buffers, kp, ki)
{
	for (uint16_t i = 0; i < USBAudioInInterface::ringRxBufferSize; i++) {
		for (uint16_t j = 0; j < USB_AUDIO_MAX_NO_CHANNELS; j++) {
			rxBuffer_f32[i][j] = NULL;
		}
	}
	_usbInterface.begin();
}

void AudioInputUSB_F32::begin(void)
{
}

void AudioInputUSB_F32::update(void)
{
	int16_t bIdx = -1;
	uint16_t noChannels;
	usb_input_update_count++;
	_usbInterface.update(bIdx, noChannels);
	if (bIdx != -1) {
		usb_input_tx_count++;
		for (uint16_t i = 0; i < noChannels; i++) {
			transmit(rxBuffer_f32[bIdx][i], i);
			AudioStream_F32::release(rxBuffer_f32[bIdx][i]);
			rxBuffer_f32[bIdx][i] = NULL;
		}
		_usbInterface.incrementBufferIndex();
	}
}

float AudioInputUSB_F32::getBufferedSamples() const
{
	return _usbInterface.getBufferedSamples();
}

float AudioInputUSB_F32::getBufferedSamplesSmooth() const
{
	return _usbInterface.getBufferedSamplesSmooth();
}

float AudioInputUSB_F32::getRequestedSamplingFrequ() const
{
	return _usbInterface.getRequestedSamplingFrequ();
}

float AudioInputUSB_F32::getActualBIntervalUs() const
{
	return _usbInterface.getActualBIntervalUs();
}

USBAudioInInterface::Status AudioInputUSB_F32::getStatus() const
{
	return _usbInterface.getStatus();
}

float AudioInputUSB_F32::volume(void)
{
	return _usbInterface.volume();
}

#if AUDIO_SUBSLOT_SIZE == 2
void AudioInputUSB_F32::copy_to_buffers(const uint8_t *src, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len)
{
	const int16_t *src16 = (const int16_t *)src;
	for (uint32_t i = 0; i < len; i++) {
		for (uint16_t j = 0; j < noChannels; j++) {
			rxBuffer_f32[bIdx][j]->data[count + i] = *src16++ / 32768.0f;
		}
	}
}
#elif AUDIO_SUBSLOT_SIZE == 3
void AudioInputUSB_F32::copy_to_buffers(const uint8_t *src, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len)
{
	for (uint32_t i = 0; i < len; i++) {
		for (uint16_t j = 0; j < noChannels; j++) {
			uint32_t val = src[0] | ((uint32_t)src[1] << 8) | ((uint32_t)src[2] << 16);
			if (val & 0x800000) val |= 0xFF000000;
			rxBuffer_f32[bIdx][j]->data[count + i] = (int32_t)val / 8388608.0f;
			src += 3;
		}
	}
}
#elif AUDIO_SUBSLOT_SIZE == 4
void AudioInputUSB_F32::copy_to_buffers(const uint8_t *src, uint16_t bIdx, uint16_t noChannels, unsigned int count, unsigned int len)
{
	const int32_t *src32 = (const int32_t *)src;
	for (uint32_t i = 0; i < len; i++) {
		for (uint16_t j = 0; j < noChannels; j++) {
			rxBuffer_f32[bIdx][j]->data[count + i] = *src32++ / 2147483648.0f;
		}
	}
}
#else
#error "Unsupported AUDIO_SUBSLOT_SIZE (supported: 2, 3, 4)"
#endif

bool AudioInputUSB_F32::setBlockQuite(uint16_t bIdx, uint16_t channel)
{
	if (!rxBuffer_f32[bIdx][channel]) {
		rxBuffer_f32[bIdx][channel] = AudioStream_F32::allocate_f32();
	}
	if (rxBuffer_f32[bIdx][channel]) {
		memset(rxBuffer_f32[bIdx][channel]->data, 0, AUDIO_BLOCK_SAMPLES * sizeof(float));
		return true;
	}
	return false;
}

void AudioInputUSB_F32::releaseBlock(uint16_t bIdx, uint16_t channel)
{
	if (rxBuffer_f32[bIdx][channel]) {
		AudioStream_F32::release(rxBuffer_f32[bIdx][channel]);
		rxBuffer_f32[bIdx][channel] = NULL;
	}
}

bool AudioInputUSB_F32::allocateBlock(uint16_t bIdx, uint16_t channel)
{
	if (!rxBuffer_f32[bIdx][channel]) {
		rxBuffer_f32[bIdx][channel] = AudioStream_F32::allocate_f32();
	}
	return rxBuffer_f32[bIdx][channel] != NULL;
}

bool AudioInputUSB_F32::areBlocksReady(uint16_t bIdx, uint16_t noChannels)
{
	for (uint16_t i = 0; i < noChannels; i++) {
		if (!rxBuffer_f32[bIdx][i]) {
			return false;
		}
	}
	return true;
}

#endif
