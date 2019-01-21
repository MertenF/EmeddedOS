#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define GPIO0 17


int main()
{
    if (geteuid() != 0) {
        printf("error, no root\n");
        return 1;
    }
    int memfd;
    if ((memfd = open("/dev/mem", O_RDWR|O_SYNC)) <0) {
        printf("unable to open /dev/mem: %s\n", strerror(errno));
        return 1;
    }

    size_t lenght = 0x01000000;
    size_t offset = 0x3f000000;
    void *mapping = mmap(NULL, lenght, (PROT_READ|PROT_WRITE), MAP_SHARED, memfd, offset);
    if (mapping == MAP_FAILED) {
       printf("mmap failed: %s\n", strerror(errno));
        return 1;
    }

    uint32_t *gpio_addr = (uint32_t *)mapping + 0x200000/4;
    uint32_t *gpset0 = gpio_addr + 0x1C/4;
    uint32_t *gpclr0 = gpio_addr + 0x28/4;
    uint32_t *gpfsel1 = gpio_addr + 0x00/4;

    printf("test\n");


    __sync_synchronize();
    *gpfsel1 = 0x00000000000000000001000000000000; //0b001 = output
    __sync_synchronize();

    while(1)
    {

        __sync_synchronize();
        *gpset0 = 1<<17;
        __sync_synchronize();

       sleep(1);

        __sync_synchronize();
        *gpclr0 = 1<<17;
        __sync_synchronize();

       sleep(1);

 }

    return 0;
}
