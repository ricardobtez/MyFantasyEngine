#pragma once

void filamentInit();
void filamentStop();
void filamentLoop();

void* onMeshLoad(void* data, unsigned int data_size);
void* onMaterialLoad(void* data, unsigned int data_size);
void* onTextureLoad(void* data, unsigned int data_size);

void setCameraAngle(float a, float b);

// from JS, GL context
void filamentCreate();