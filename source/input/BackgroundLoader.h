#pragma once

#include <vector>
#include <stdint.h>

bool decodeJpegXlOneShot(const uint8_t* jxl, size_t size, std::vector<uint8_t>* pixels, size_t* xsize, size_t* ysize, std::vector<uint8_t>* icc_profile);