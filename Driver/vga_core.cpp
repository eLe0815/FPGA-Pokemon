/*****************************************************************//**
 * @file timer_core.cpp
 *
 * @brief implementation of various video core classes
 *
 * @author p chu
 * @version v1.0: initial release
 ********************************************************************/

#include "vga_core.h"

/**********************************************************************
 * General purpose video core methods
 *********************************************************************/
GpvCore::GpvCore(uint32_t core_base_addr) {
   base_addr = core_base_addr;
}
GpvCore::~GpvCore() {
}

void GpvCore::wr_mem(int addr, uint32_t data) {
   io_write(base_addr, addr, data);
}

void GpvCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

/**********************************************************************
 * Sprite core methods
 *********************************************************************/
SpriteCore::SpriteCore(uint32_t core_base_addr, int sprite_size) {
   base_addr = core_base_addr;
   size = sprite_size;
}
SpriteCore::~SpriteCore() {
}

void SpriteCore::wr_mem(int addr, uint32_t color) {
   io_write(base_addr, addr, color);
}

void SpriteCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

void SpriteCore::move_xy(int x, int y) {
   io_write(base_addr, X_REG, x);
   io_write(base_addr, Y_REG, y);
   return;
}

void SpriteCore::wr_ctrl(int32_t cmd) {
   io_write(base_addr, SPRITE_CTRL_REG, cmd);
}


/**********************************************************************
 * OSD core methods
 *********************************************************************/
OsdCore::OsdCore(uint32_t core_base_addr) {
   base_addr = core_base_addr;
   set_color(0x0f0, CHROMA_KEY_COLOR);  // green on black
}
OsdCore::~OsdCore() {
}
// not used

void OsdCore::set_color(uint32_t fg_color, uint32_t bg_color) {
   io_write(base_addr, FG_CLR_REG, fg_color);
   io_write(base_addr, BG_CLR_REG, bg_color);
}

void OsdCore::wr_char(uint8_t x, uint8_t y, char ch, int reverse) {
   uint32_t ch_offset;
   uint32_t data;

   ch_offset = (y << 7) + (x & 0x07f);   // offset is concatenation of y and x
   if (reverse == 1)
      data = (uint32_t)(ch | 0x80);
   else
      data = (uint32_t) ch;
   io_write(base_addr, ch_offset, data);
   return;
}

void OsdCore::clr_screen() {
   int x, y;

   for (x = 0; x < CHAR_X_MAX; x++)
      for (y = 0; y < CHAR_Y_MAX; y++) {
         wr_char(x, y, NULL_CHAR);
      }
   return;
}

void OsdCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

/**********************************************************************
 * FrameCore core methods
 *********************************************************************/
FrameCore::FrameCore(uint32_t frame_base_addr) {
   base_addr = frame_base_addr;
}
FrameCore::~FrameCore() {
}
// not used

void FrameCore::wr_pix(int x, int y, int color) {
   uint32_t pix_offset;

   pix_offset = HMAX * y + x;
   io_write(base_addr, pix_offset, color);
   return;
}

void FrameCore::clr_screen(int color) {
   int x, y;

   for (x = 0; x < HMAX; x++)
      for (y = 0; y < VMAX; y++) {
         wr_pix(x, y, color);
      }
   return;
}

void FrameCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

void FrameCore::drawFastVLine(int x, int y, int h, int color) {
  plot_line(x, y, x, y + h - 1, color);
}

void FrameCore::drawFastHLine(int x, int y, int w, int color) {
  plot_line(x, y, x + w - 1, y, color);
}

void FrameCore::fillRect(int x, int y, int w, int h, int color) {
  for (int i = x; i < x + w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void FrameCore::fillRoundRect(int x, int y, int w, int h, int r, int color) {
  int max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  fillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void FrameCore::fillCircle(int x0, int y0, int r, int color) {

  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void FrameCore::fillCircleHelper(int x0, int y0, int r, int corners, int delta, int color) {

  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  int px = x;
  int py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

// from AdaFruit
void FrameCore::plot_line(int x0, int y0, int x1, int y1, int color) {
   int dx, dy;
   int err, ystep, steep;

   if (x0 > x1) {
      swap(x0, x1);
      swap(y0, y1);
   }
   // slope is high
   steep = (abs(y1 - y0) > abs(x1 - x0)) ? 1 : 0;
   if (steep) {
      swap(x0, y0);
      swap(x1, y1);
   }
   dx = x1 - x0;
   dy = abs(y1 - y0);
   err = dx / 2;
   if (y0 < y1) {
      ystep = 1;
   } else {
      ystep = -1;
   }
   for (; x0 <= x1; x0++) {
      if (steep) {
         wr_pix(y0, x0, color);
      } else {
         wr_pix(x0, y0, color);
      }
      err = err - dy;
      if (err < 0) {
         y0 = y0 + ystep;
         err = err + dx;
      }
   }
}



void FrameCore::swap(int &a, int &b) {
   int tmp;

   tmp = a;
   a = b;
   b = tmp;
}

