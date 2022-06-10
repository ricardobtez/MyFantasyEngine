#pragma once

void windowInit();
void *windowGetHandle();
void windowClose();
void windowLoop();
bool windowShouldClose();

int windowGetWidth();
int windowGetHeight();

void windowMouseEvent(int x, int y);