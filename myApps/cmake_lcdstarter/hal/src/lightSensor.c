/* This file include actual implements of joystick functionality such as control I2C bus to read ADC.
 * It also include helper functions to read the joystick value and direction in axis X and Y.
*/
#include "hal/lightSensor.h"
#include <assert.h>
#include "time.h"
#include <pthread.h>

static int i2c_file_desc = -1;


static pthread_t sampler_thread;
static bool keep_sampling = false;

void *Sampler_getLightLevel(void *args);

static void sleepForMs(long long delayInMs) { 
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000; 
    long long delayNs = delayInMs * NS_PER_MS;  
    int seconds = delayNs / NS_PER_SECOND;  
    int nanoseconds = delayNs % NS_PER_SECOND;  
    struct timespec reqDelay = {seconds, nanoseconds}; 
    nanosleep(&reqDelay, (struct timespec *) NULL); 
}


int init_i2c_bus(char* bus, int address) {
    i2c_file_desc = open(bus, O_RDWR);
    if (i2c_file_desc == -1) {
        printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(EXIT_FAILURE);
    }

    if (ioctl(i2c_file_desc, I2C_SLAVE, address) == -1) {
        perror("Unable to set I2C device to slave address.");
        exit(EXIT_FAILURE);
    }
    return i2c_file_desc;
}

void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value) {
    int tx_size = 1 + sizeof(value);
    uint8_t buff[tx_size];
    buff[0] = reg_addr;
    buff[1] = (value & 0xFF);
    buff[2] = (value & 0xFF00) >> 8;
    int bytes_written = write(i2c_file_desc, buff, tx_size);
    if (bytes_written != tx_size) {
        perror("Unable to write i2c register");
        exit(EXIT_FAILURE);
    }
    sleepForMs(1);
}

uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr) {
    // To read a register, must first write the address
    int bytes_written = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
    if (bytes_written != sizeof(reg_addr)) {
        perror("Unable to write i2c register.");
        exit(EXIT_FAILURE);
    }

    // Now read the value and return it
    uint16_t value = 0;
    int bytes_read = read(i2c_file_desc, &value, sizeof(value));
    if (bytes_read != sizeof(value)) {
        perror("Unable to read i2c register");
        exit(EXIT_FAILURE);
    }
    sleepForMs(1);
    return value;
}

void Sampler_init() {
    i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    keep_sampling = true;
    if (pthread_create(&sampler_thread, NULL, &Sampler_getLightLevel, NULL) != 0) {
        perror("Failed to create sampler thread");
    }
}
void *Sampler_getLightLevel(void *args) 
{
    while (keep_sampling) {
        (void)args;
        write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_1);

        // Read a register:
        uint16_t raw_read = read_i2c_reg16(i2c_file_desc, REG_DATA);
        
        // Convert byte order and shift bits into place
        uint16_t value = ((raw_read & 0xFF) << 8) | ((raw_read & 0xFF00) >> 8);
        value  = value >> 4;
        printf("Light level: %d\n", value);
        sleepForMs(1000);
    }
    return NULL;
}




