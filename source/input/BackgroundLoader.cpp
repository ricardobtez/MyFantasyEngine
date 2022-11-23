#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner_cxx.h>

#include "BackgroundLoader.h"

bool decodeJpegXlOneShot(const uint8_t* jxl, size_t size, std::vector<uint8_t>* pixels, size_t* xsize, size_t* ysize, std::vector<uint8_t>* icc_profile)
{
	// Multi-threaded parallel runner.
	auto runner = JxlResizableParallelRunnerMake(nullptr);

	auto dec = JxlDecoderMake(nullptr);
	
	if (JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING | JXL_DEC_FULL_IMAGE) != JXL_DEC_SUCCESS) {
		return false;
	}

	if (JxlDecoderSetParallelRunner(dec.get(), JxlResizableParallelRunner, runner.get()) != JXL_DEC_SUCCESS) {
		return false;
	}

	JxlBasicInfo info;
	JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};

	JxlDecoderSetInput(dec.get(), jxl, size);
	JxlDecoderCloseInput(dec.get());

	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());

		if (status == JXL_DEC_ERROR) {
			fprintf(stderr, "Decoder error\n");
			return false;
		} else if (status == JXL_DEC_NEED_MORE_INPUT) {
			fprintf(stderr, "Error, already provided all input\n");
			return false;
		} else if (status == JXL_DEC_BASIC_INFO) {
			if (JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &info)) {
				fprintf(stderr, "JxlDecoderGetBasicInfo failed\n");
				return false;
			}
			*xsize = info.xsize;
			*ysize = info.ysize;
			JxlResizableParallelRunnerSetThreads(runner.get(), JxlResizableParallelRunnerSuggestThreads(info.xsize, info.ysize));
		} else if (status == JXL_DEC_COLOR_ENCODING) {
		// Get the ICC color profile of the pixel data
			size_t icc_size;
			if (JxlDecoderGetICCProfileSize( dec.get(), &format, JXL_COLOR_PROFILE_TARGET_DATA, &icc_size) != JXL_DEC_SUCCESS) {
				return false;
			}
			icc_profile->resize(icc_size);
			if (JxlDecoderGetColorAsICCProfile( dec.get(), &format, JXL_COLOR_PROFILE_TARGET_DATA, icc_profile->data(), icc_profile->size()) != JXL_DEC_SUCCESS) {
				return false;
			}
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			size_t buffer_size;
			if (JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size) != JXL_DEC_SUCCESS) {
				fprintf(stderr, "JxlDecoderImageOutBufferSize failed\n");
				return false;
			}
			if (buffer_size != *xsize * *ysize * 3) {
				return false;
			}
			pixels->resize(*xsize * *ysize * 3);
			void* pixels_buffer = (void*)pixels->data();
			size_t pixels_buffer_size = pixels->size() * sizeof(float);
			if (JxlDecoderSetImageOutBuffer(dec.get(), &format, pixels_buffer, pixels_buffer_size) != JXL_DEC_SUCCESS) {
				fprintf(stderr, "JxlDecoderSetImageOutBuffer failed\n");
				return false;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
		// Nothing to do. Do not yet return. If the image is an animation, more
		// full frames may be decoded. This example only keeps the last one.
		} else if (status == JXL_DEC_SUCCESS) {
		// All decoding successfully finished.
		// It's not required to call JxlDecoderReleaseInput(dec.get()) here since
		// the decoder will be destroyed.
			return true;
		} else {
			fprintf(stderr, "Unknown decoder status\n");
			return false;
		}
	}
}
