#include <bcm2835.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

volatile uint8_t g_Loop = 1;

void sighandler(int signum)
{
    g_Loop = 0;
}




float getTemp() {
    char read_command = 0b01010000;
    char send_buffer[] = {read_command, 0x00, 0x00};
    char rec_buf[3] = {0};

    bcm2835_spi_transfernb(send_buffer, rec_buf, sizeof(send_buffer));
//    printf("Read from SPI: %02X  %02X  %02X  %02X\n", rec_buf[0], rec_buf[1], rec_buf[2]);
    uint16_t temp = (rec_buf[1]<<8) + rec_buf[2];
//    printf("Samen: %04X   %d \n", temp, temp);
    temp = (temp&0x7FFFFFFF)>>3; //discard bit 15, 2, 1 and 0
//    printf("T= %04X  %d  %f\n", temp, temp, temp/16.0);
    return temp/16.0;
}


int main(int argc, char **argv)
{
    signal(SIGINT, sighandler);

    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failed. Are you running as root??\n");
      return 1;
    }

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default, MSB sent first
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // CPOL=1 clk idle @1, rising edge=data  CPHA=1 
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default/slowest (~6.1KHz)
    bcm2835_spi_chipSelect(BCM2835_SPI_CS1);                      // temp sensor op CS1
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);      // CS1, active LOW

    //reset temp sensor
    const char reset[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    bcm2835_spi_writenb(reset, 4);
    bcm2835_delay(500);

    if(argc != 2) {
        printf("Argument meegeven aub!\n");
        printf("c continu\n");
        printf("s single shot\n");
        return 1;
    }
    if(!strcmp(argv[1], "s")) {
        printf("Temperatuur: %f°C\n", getTemp());
    } else if(!strcmp(argv[1], "c")) {
        while(g_Loop) {
            printf("Temperatuur: %f°C\n", getTemp());
            bcm2835_delay(500);
        }
     }

    printf("Goodbye\n");
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
