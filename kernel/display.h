#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>

//#include "font8x8_basic.h"
#include "../font.h"
#include "../drivers/keyboard/keyboard_map.h"
#include "rsdt.h"
#include "bios.h"

struct stivale2_struct_tag_framebuffer *videoheader;

static int displayEnabled =0;
static  uint32_t * bufferr = 0x0;
static char cmdbuffer[255];
static  int keycolor = 0xFFFFFF;

void fontcolor (int c);
static void CreateBackbuffer();

static void PushLine(uint32_t * screen);

static void putpixel(int x, int y, int color);
static int colorc = 0;
static void video_char(char character, int pos_x, int pos_y, int color);

static int charoffset= 10;
static int lineoffset = 10;

void ResetPosition();
void processchars(char mappedchar);

void EnableVideo();

void processkeys(char keycode);

void ClearScreen();