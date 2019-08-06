# This STM32F103 firmware interfaces an ATM90E3x with Atmel's GUI program
# Copyright (C) 2019 Mastro Gippo
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

#include "mbed.h"
#include "main.h"

Serial pc(pin_u_tx, pin_u_rx);
SPI device(pin_mosi, pin_miso, pin_sclk);
DigitalOut cs(pin_cs);

DigitalOut led1(pin_rly2);
DigitalOut rly(pin_rly1);

uint16_t ReadSPI(uint16_t addr)
{
    cs = 0;
    device.write(0x8000+addr);
    int res = device.write(0x0000);
    cs = 1;
    return res;
}
uint16_t WriteSPI(uint16_t addr, uint16_t val)
{
    cs = 0;
    device.write(addr);
    int res = device.write(val);
    cs = 1;
    return res;
}


unsigned char smg[50];
int count = 0;
int num;
char tc;
bool flagser_getend;

unsigned char b1[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
    0x81, 0x08, 0x65, 0xF3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xE3, 0x16};
unsigned char b2[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
    0x81, 0x03, 0x43, 0x13, 0x49, 0xF3, 0x16};
//unsigned char b3[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
//    0x81, 0x04, 0xA3, 0x13, 0x00, 0x00, 0x0B, 0x16};
unsigned char b3[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
    0x81, 0x04, 0xA3, 0x13, 0x33, 0x33, 0x71, 0x16};
unsigned char b_scale[] = { 0x81, 0x04, 0xA3, 0x13, 0x37, 0x37};
unsigned char rmem[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
    0x81, 0x04, 0xA3, 0x13, 0x33, 0x33, 0x71, 0x16};
unsigned char wmem[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
    0x84, 0x00, 0x54, 0x16};

unsigned char initS[] = {0xFE, 0xFE, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68};
unsigned char buffC[50];
            

void Wr(uint8_t * buf, int len)
{
    //return;
    for(int i = 0; i < len; i++)
    {
        while(!pc.writable());
        pc.putc(buf[i]);
        //wait(0.001);
    }
}

void WriteCmd(uint8_t * buf, int len)
{
    Wr(initS,10);
    unsigned char cs = 0xD0;
    for(int i = 0; i < len; i++)
    {
        while(!pc.writable());
        pc.putc(buf[i]);
        cs += buf[i];
        //wait(0.001);
    }
    pc.putc(cs);
    pc.putc(0x16);
}

void ParseAnswer()
{
    unsigned char cmd = smg[8];
    uint16_t addr = smg[11] - 0x03;
    addr = (addr << 8) + (uint8_t)(smg[10] - 0x33);

    if(smg[8] == 0x01) //read
    {
        if(addr == 0xF032) Wr(b1,22);
        else if(addr == 0x1010) Wr(b2,17);
        else if(addr == 0x1050) 
        {
            rly = !rly;
            Wr(b2,17); //lettura pins CPU
        }
        else if(addr == 0x1070)  WriteCmd(b_scale,6);//Wr(b3,18);
        else// if(cmd <= 0x1000)
        {
            rmem[12] = smg[10];
            rmem[13] = smg[11];
            
            addr = ReadSPI(addr);// + 0x3333;

            rmem[14] =(uint8_t)(addr)+0x33;
            rmem[15] =(uint8_t)(addr>>8)+0x33;
            
            uint8_t cs = 0x55;
            for(int i = 12; i < 16; i++)
                cs += rmem[i];
            rmem[16] = cs;
            Wr(rmem,18);
        }
    }
    else if(smg[8] == 0x04) //write
    {
        uint16_t data = smg[16] - 0x33;
        data = (data << 8) + (uint8_t)(smg[15] - 0x33);
        buffC[0] = 0x84;
        buffC[1] = 0x00;

        WriteSPI(addr,data);

        WriteCmd(buffC,2);
    }

        /*    if((smg[10] == 0x65) && (smg[11] == 0xF3)) Wr(b1,22);
    else if((smg[10] == 0x43) && (smg[11] == 0x13)) Wr(b2,17);
    else if((smg[10] == 0xA3) && (smg[11] == 0x13)) Wr(b3,18);*/

}

void SerInt() {
    unsigned char temp;
    static unsigned char len;
    while(pc.readable())
    {
        temp = pc.getc();
        if(count == 0)
        {
            if(temp == 0x68) //start
                smg[count++] = temp;
        }
        else if(count <= 6) //get address
        {
            smg[count++] = temp;
        }
        else if(count == 7)
        {
            if(temp == 0x68) //avant
                smg[count++] = temp;
        }
        else if(count == 8) //control code
        {
            smg[count++] = temp;
        }
        else if(count == 9) //len
        {
            len = temp;
            smg[count++] = temp;
        }
        else if(count <= (9+len+1))
        {
            smg[count++] = temp;
        }
        else
        {
            if(temp == 0x16) //end
            {
                smg[count++] = temp;
                wait(0.001);
                ParseAnswer();
                count = 0;
            }
            else
            { 
                pc.putc(0x69);
            }
        }
    }
}

void SerialStart(void) {
    pc.format(8,Serial::Even,1);
    pc.baud(115200);
    flagser_getend = 0;
    pc.attach(&SerInt,Serial::RxIrq);
    count = 0;
}
 
int main() {
    SerialStart();

    cs = 1;
    
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 0.5MHz clock rate
    device.format(16,0);
    device.frequency(500000);


    while(1) {
        led1 = 1;
        wait(0.2);
        led1 = 0;
        wait(0.2);
        //ser.printf("ciao\r\n");
        
    }
}