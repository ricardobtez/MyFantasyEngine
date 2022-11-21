#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner_cxx.h>

#include "BackgroundLoader.h"

bool decodeJpegXlOneShot(const uint8_t* jxl, size_t size, std::vector<uint8_t>* pixels, size_t* xsize, size_t* ysize, std::vector<uint8_t>* icc_profile)
{
	auto decoder = JxlDecoderMake(nullptr);

	if (JxlDecoderSubscribeEvents(decoder.get(), JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE) != JXL_DEC_SUCCESS) {
		return false;
	}

	if (JxlDecoderSetInput(decoder.get(), jxl, size) != JXL_DEC_SUCCESS) {
		return false;
	}

	JxlBasicInfo info;
	JxlPixelFormat format = { 3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0 };

	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(decoder.get());

		if (status == JXL_DEC_ERROR) {
			return false;
		} else if (status == JXL_DEC_NEED_MORE_INPUT) {
			return false;
		} else if (status == JXL_DEC_BASIC_INFO) {
			if (JxlDecoderGetBasicInfo(decoder.get(), &info) != JXL_DEC_SUCCESS) {
				return false;
			}
			*xsize = info.xsize;
			*ysize = info.ysize;
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			size_t buffer_size;
			if (JxlDecoderImageOutBufferSize(decoder.get(), &format, &buffer_size) != JXL_DEC_SUCCESS) {
				return false;
			}
			if (buffer_size != *xsize * *ysize * 3) {
				return false;
			}
			pixels->resize(*xsize * *ysize * 3);
			void* pixels_buffer = (void*)pixels->data();
			size_t pixels_buffer_size = pixels->size() * sizeof(uint8_t);
			if (JxlDecoderSetImageOutBuffer(decoder.get(), &format, pixels_buffer, pixels_buffer_size) != JXL_DEC_SUCCESS) {
				return false;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
			//We have a full image - in the case of an animation, keep decoding until the last frame
		} else if (status == JXL_DEC_SUCCESS) {
			return true;
		} else {
			return false;
		}
	}
}
