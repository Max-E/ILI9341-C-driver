// Copyright (c) 2014 Adafruit Industries
// Author: Tony DiCola
//
// Ported to C by Max Eliaser
// C port Copyright (c) 2014 Intel Corp
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

int data_command_select_fd, reset_fd, display_fd;

int init_output_gpio (int gpio_num);
int init_spidev (int bus_num, int chip_select);

// Screen dimensions
#define ILI9341_TFTWIDTH    240
#define ILI9341_TFTHEIGHT   320

// command/data select line
#define COMMAND 0
#define DATA    1

// Screen control codes
enum
{
    ILI9341_NOP         = 0x00,
    ILI9341_SWRESET     = 0x01,
    ILI9341_RDDID       = 0x04,
    ILI9341_RDDST       = 0x09,

    ILI9341_SLPIN       = 0x10,
    ILI9341_SLPOUT      = 0x11,
    ILI9341_PTLON       = 0x12,
    ILI9341_NORON       = 0x13,

    ILI9341_RDMODE      = 0x0A,
    ILI9341_RDMADCTL    = 0x0B,
    ILI9341_RDPIXFMT    = 0x0C,
    ILI9341_RDIMGFMT    = 0x0A,
    ILI9341_RDSELFDIAG  = 0x0F,

    ILI9341_INVOFF      = 0x20,
    ILI9341_INVON       = 0x21,
    ILI9341_GAMMASET    = 0x26,
    ILI9341_DISPOFF     = 0x28,
    ILI9341_DISPON      = 0x29,

    ILI9341_CASET       = 0x2A,
    ILI9341_PASET       = 0x2B,
    ILI9341_RAMWR       = 0x2C,
    ILI9341_RAMRD       = 0x2E,

    ILI9341_PTLAR       = 0x30,
    ILI9341_MADCTL      = 0x36,
    ILI9341_IDLOFF      = 0x38,
    ILI9341_PIXFMT      = 0x3A,

    ILI9341_FRMCTR1     = 0xB1,
    ILI9341_FRMCTR2     = 0xB2,
    ILI9341_FRMCTR3     = 0xB3,
    ILI9341_INVCTR      = 0xB4,
    ILI9341_DFUNCTR     = 0xB6,

    ILI9341_PWCTR1      = 0xC0,
    ILI9341_PWCTR2      = 0xC1,
    ILI9341_PWCTR3      = 0xC2,
    ILI9341_PWCTR4      = 0xC3,
    ILI9341_PWCTR5      = 0xC4,
    ILI9341_VMCTR1      = 0xC5,
    ILI9341_VMCTR2      = 0xC7,

    ILI9341_RDID1       = 0xDA,
    ILI9341_RDID2       = 0xDB,
    ILI9341_RDID3       = 0xDC,
    ILI9341_RDID4       = 0xDD,

    ILI9341_GMCTRP1     = 0xE0,
    ILI9341_GMCTRN1     = 0xE1,

    ILI9341_PWCTR6      = 0xFC
};

void ili_send (int data_command, const unsigned char *data, int datasize);
void ili_reset (void);
void ili_init (void);
void ili_set_window (int left, int top, int right, int bottom);

#define ili_sendbyte(data_command,n) \
    ili_send (data_command, (unsigned char[]) {n}, 1)

#define ili_send16bit(data_command,n) \
    ili_send (data_command, (unsigned char[]) {(n)>>8, (n)&255}, 2)
