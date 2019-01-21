#include <stdio.h>
#include <bcm2835.h>

#define PIN0 RPI_V2_GPIO_P1_11
#define PIN1 RPI_V2_GPIO_P1_12
#define PIN2 RPI_V2_GPIO_P1_13
#define PIN3 RPI_V2_GPIO_P1_15
#define PIN4 RPI_V2_GPIO_P1_16
#define PIN5 RPI_V2_GPIO_P1_18
#define PIN6 RPI_V2_GPIO_P1_08
#define PIN7 RPI_V2_GPIO_P1_07

uint8_t pins[8] = {PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7};


int main() {
    if (!bcm2835_init())
      return 1;


    for(int i=0; i<8; i++) {
        bcm2835_gpio_fsel(pins[i], BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(pins[i], LOW);
    }

    while(1) {
        for(int i=0; i<8; i++) {
	    bcm2835_gpio_write(pins[i], HIGH);
            bcm2835_delay(500);
            bcm2835_gpio_write(pins[i], LOW);
        }
    }


    bcm2835_close();
    return 0;
}
