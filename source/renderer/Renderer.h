#pragma once

#include <stdint.h>

int rendererInit(void* windowHandle, const uint32_t windowWidth, const uint32_t windowHeight);
void rendererStop();
void rendererLoop();

void* meshLoad(void* data, unsigned int size);
void* materialLoad(void* data, unsigned int size);
void* textureLoad(void* data, unsigned int size);