// Test program for ILI9341. 
// Copyright (C) 2014 Intel Corp
// Author: Max Eliaser
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

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "ILI9341.h"

int main (int argc, const char *argv[])
{
    int framenum = 0;
    int rownum = 0;
    int effectnum = 0;
    double val = 0;

#define INTERLACE 3
#define ROWSPERFRAME (ILI9341_TFTHEIGHT/(2*INTERLACE))
    
    data_command_select_fd = init_output_gpio (82);
    reset_fd = init_output_gpio (83);
    display_fd = init_spidev (32766, 0);
    
    ili_reset ();
    ili_init ();
    
    static uint16_t buf[ILI9341_TFTWIDTH] = {0};
    ili_set_window (0, 0, ILI9341_TFTWIDTH - 1, ILI9341_TFTHEIGHT - 1);
    for (int n = 0; n < ILI9341_TFTHEIGHT + 15; n++)
        ili_send (DATA, (const unsigned char *)buf, sizeof buf);

#define FXSWITCH 75
    
    while (true)
    {
        for (int y = framenum%INTERLACE; y < ILI9341_TFTHEIGHT/2; y += INTERLACE)
        {
            static uint16_t buf[ILI9341_TFTWIDTH/2];
            
            ili_set_window (ILI9341_TFTWIDTH/4, ILI9341_TFTHEIGHT/4 + y, 3*ILI9341_TFTWIDTH/4 - 1, ILI9341_TFTHEIGHT/4 + y);
            
            for (int x = 0; x < ILI9341_TFTWIDTH/2; x++)
            {
                switch (effectnum % 3)
                {
                case 0:
                    val = (double)(rownum % (6 * ROWSPERFRAME * FXSWITCH))/(1920.0 * ROWSPERFRAME);
                    buf[x] = (uint16_t)(512.0 / 2.0 * (sin ((double) (x ^ y) * val) + 1.0));
                    break;
                case 1:
                    val = (double)(rownum % (12 * ROWSPERFRAME * FXSWITCH))/(360.0 * ROWSPERFRAME);
                    buf[x] = (uint16_t)(512.0 / 2.0 * (sin ((double) (sin ((double)x/10.0) * sin ((double)y/10.0)) * val) + 1.0));
                    break;
                case 2:
                    val = (double)(rownum % (18 * ROWSPERFRAME * FXSWITCH))/(1920.0 * ROWSPERFRAME);
                    buf[x] = (uint16_t)(512.0 / 2.0 * (sin ((double) (sin ((x+framenum)/10.0) * sin ((y+framenum)/10.0)) * val) + 1.0));
                    break;
                }
            }
            
            ili_send (DATA, (const unsigned char *)buf, sizeof buf);
            
            rownum++;
            
            if (rownum % INTERLACE)
                continue;
            
            static uint16_t rainbow[6][3] = 
            {
                {0xF800, 0xF800, 0xF800}, // red
                {0xFFE0, 0xFFE0, 0xFFE0}, // yellow
                {0x07E0, 0x07E0, 0x07E0}, // green
                {0x07FF, 0x07FF, 0x07FF}, // cyan
                {0x001F, 0x001F, 0x001F}, // blue
                {0xF81F, 0xF81F, 0xF81F}, // magenta
            };
            
            for (int n = 0; n < 6; n++)
            {
                int y = (framenum + n * ILI9341_TFTHEIGHT / 3) % (ILI9341_TFTHEIGHT * 2);
                
                if (y >= ILI9341_TFTHEIGHT)
                    continue;
                
                ili_set_window (0, y, 2, y);
                ili_send (DATA, (const unsigned char *)(rainbow[n]), sizeof rainbow[n]);
            }
        }
        
        framenum++;
        
        if (framenum % FXSWITCH == 0)
            effectnum++;
    }
    
    return 0;
}
