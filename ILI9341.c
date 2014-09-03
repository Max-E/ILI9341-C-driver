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

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "ILI9341.h"

#if 0
#define write (void)write
#else
static inline void _write (int fd, const void *buf, size_t count, int line)
{
    ssize_t written = write (fd, buf, count);
    
    if (written < 0)
    {
        char buf[4096];
        snprintf (buf, sizeof buf, "Line %d", line);
        perror (buf);
        exit (1);
    }
    
    if (written != (ssize_t) count)
    {
        printf ("%ld != %lu on line %d\n", written, count, line);
        exit (1);
    }
}
#define write(fd,b,c) _write (fd, b, c, __LINE__)
#endif

int init_output_gpio (int gpio_num)
{
    int retfd, tmpfd;
    char buf[4096], vbuf[4096];
    
    snprintf (vbuf, sizeof vbuf, "/sys/class/gpio/gpio%d/value", gpio_num);
    
    retfd = open (vbuf, O_WRONLY);
    if (retfd < 0)
    {
        tmpfd = open ("/sys/class/gpio/export", O_WRONLY);
        snprintf (buf, sizeof buf, "%d", gpio_num);
        write (tmpfd, buf, strlen (buf));
        close (tmpfd);
        
        retfd = open (vbuf, O_WRONLY);
    }
    
    snprintf (buf, sizeof buf, "/sys/class/gpio/gpio%d/direction", gpio_num);
    tmpfd = open (buf, O_WRONLY);
    write (tmpfd, "out", 3);
    close (tmpfd);
    
    return retfd;
}

static void fsleep (double t)
{
    struct timespec req =
        {floor (t), floor (1000000.0 * (t - floor (t)))};
    
    nanosleep (&req, NULL);
}

#define write_gpio(fd,state) write ((fd), "01" + (state), 1)

void ili_reset (void)
{
    write_gpio (reset_fd, 1);
    fsleep (0.005);
    write_gpio (reset_fd, 0);
    fsleep (0.02);
    write_gpio (reset_fd, 1);
    fsleep (0.15);
}

static void data_command_select (int new_state)
{
    static int cur_state = -1;
    
    if (new_state != cur_state)
        write_gpio (data_command_select_fd, cur_state = new_state);
}

int init_spidev (int bus_num, int chip_select)
{
    char buf[4096];
    
    snprintf (buf, sizeof buf, "/dev/spidev%d.%d", bus_num, chip_select);
    int retfd = open (buf, O_RDWR);
    
    unsigned char setmode = 0;
    ioctl (retfd, SPI_IOC_WR_MODE, &setmode);
    
    return retfd;
}

void ili_send (int data_command, const unsigned char *data, int datasize)
{
    data_command_select (data_command);
    write (display_fd, data, datasize);
}

void ili_init (void)
{
	ili_sendbyte (COMMAND, 0xEF);
	ili_sendbyte (DATA, 0x03);
	ili_sendbyte (DATA, 0x80);
	ili_sendbyte (DATA, 0x02);
	ili_sendbyte (COMMAND, 0xCF);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0XC1);
	ili_sendbyte (DATA, 0X30);
	ili_sendbyte (COMMAND, 0xED);
	ili_sendbyte (DATA, 0x64);
	ili_sendbyte (DATA, 0x03);
	ili_sendbyte (DATA, 0X12);
	ili_sendbyte (DATA, 0X81);
	ili_sendbyte (COMMAND, 0xE8);
	ili_sendbyte (DATA, 0x85);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0x78);
	ili_sendbyte (COMMAND, 0xCB);
	ili_sendbyte (DATA, 0x39);
	ili_sendbyte (DATA, 0x2C);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0x34);
	ili_sendbyte (DATA, 0x02);
	ili_sendbyte (COMMAND, 0xF7);
	ili_sendbyte (DATA, 0x20);
	ili_sendbyte (COMMAND, 0xEA);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (COMMAND, ILI9341_PWCTR1);	// Power control 
	ili_sendbyte (DATA, 0x23);					// VRH[5:0] 
	ili_sendbyte (COMMAND, ILI9341_PWCTR2);	// Power control 
	ili_sendbyte (DATA, 0x10);					// SAP[2:0];BT[3:0] 
	ili_sendbyte (COMMAND, ILI9341_VMCTR1);	// VCM control 
	ili_sendbyte (DATA, 0x3e);
	ili_sendbyte (DATA, 0x28);
	ili_sendbyte (COMMAND, ILI9341_VMCTR2);	// VCM control2 
	ili_sendbyte (DATA, 0x86);					// --
	ili_sendbyte (COMMAND, ILI9341_MADCTL);	//  Memory Access Control 
	ili_sendbyte (DATA, 0x48);
	ili_sendbyte (COMMAND, ILI9341_PIXFMT);
	ili_sendbyte (DATA, 0x55);
	ili_sendbyte (COMMAND, ILI9341_FRMCTR1);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0x18);
	ili_sendbyte (COMMAND, ILI9341_DFUNCTR);	//  Display Function Control 
	ili_sendbyte (DATA, 0x08);
	ili_sendbyte (DATA, 0x82);
	ili_sendbyte (DATA, 0x27);
	ili_sendbyte (COMMAND, 0xF2);				//  3Gamma Function Disable 
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (COMMAND, ILI9341_GAMMASET);	// Gamma curve selected 
	ili_sendbyte (DATA, 0x01);
	ili_sendbyte (COMMAND, ILI9341_GMCTRP1);	// Set Gamma 
	ili_sendbyte (DATA, 0x0F);
	ili_sendbyte (DATA, 0x31);
	ili_sendbyte (DATA, 0x2B);
	ili_sendbyte (DATA, 0x0C);
	ili_sendbyte (DATA, 0x0E);
	ili_sendbyte (DATA, 0x08);
	ili_sendbyte (DATA, 0x4E);
	ili_sendbyte (DATA, 0xF1);
	ili_sendbyte (DATA, 0x37);
	ili_sendbyte (DATA, 0x07);
	ili_sendbyte (DATA, 0x10);
	ili_sendbyte (DATA, 0x03);
	ili_sendbyte (DATA, 0x0E);
	ili_sendbyte (DATA, 0x09);
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (COMMAND, ILI9341_GMCTRN1);	// Set Gamma 
	ili_sendbyte (DATA, 0x00);
	ili_sendbyte (DATA, 0x0E);
	ili_sendbyte (DATA, 0x14);
	ili_sendbyte (DATA, 0x03);
	ili_sendbyte (DATA, 0x11);
	ili_sendbyte (DATA, 0x07);
	ili_sendbyte (DATA, 0x31);
	ili_sendbyte (DATA, 0xC1);
	ili_sendbyte (DATA, 0x48);
	ili_sendbyte (DATA, 0x08);
	ili_sendbyte (DATA, 0x0F);
	ili_sendbyte (DATA, 0x0C);
	ili_sendbyte (DATA, 0x31);
	ili_sendbyte (DATA, 0x36);
	ili_sendbyte (DATA, 0x0F);
	ili_sendbyte (COMMAND, ILI9341_SLPOUT);	// Exit Sleep*/
	fsleep (0.120);
	ili_sendbyte (COMMAND, ILI9341_DISPON);	// Display on 
	fsleep (0.120);
}

void ili_set_window (int left, int top, int right, int bottom)
{
    ili_sendbyte (COMMAND, ILI9341_CASET);
    ili_send16bit (DATA, left);
    ili_send16bit (DATA, right);
    ili_sendbyte (COMMAND, ILI9341_PASET);
    ili_send16bit (DATA, top);
    ili_send16bit (DATA, bottom);
    ili_sendbyte (COMMAND, ILI9341_RAMWR);
}
