#pragma once

int windowInit();
void *windowGetHandle();
void windowClose();
void windowLoop();
bool windowShouldClose();

int windowGetWidth();
int windowGetHeight();