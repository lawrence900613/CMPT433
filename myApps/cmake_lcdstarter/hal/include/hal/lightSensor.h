

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdbool.h>

// Register in TLA2024
#define TLA2024_CHANNEL_CONF_1 0x83E2
#define I2CDRV_LINUX_BUS "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x48
#define REG_CONFIGURATION 0x01
#define REG_DATA 0x00
#define test 0x001


enum Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  NORMAL
};


// The I2C helper functions is ued to read the joystick value and direction in axis X and Y.

// Helper functions to write input to file descriptor (I2C)
int init_i2c_bus(char* bus, int address);

// Writes a 16-bit value to a register on the I2C bus (file descriptor).
void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value);

// Reads a 16-bit value from a register on the I2C bus file descriptor).
uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr);

// Begin/end the background thread which samples light levels.
void Sampler_init(void); 
void Sampler_cleanup(void);

void Sampler_moveCurrentDataToHistory(void);

void *sample_light_levels(void *arg);

int Sampler_getHistorySize(void);

double* Sampler_getHistory(int *size);

double Sampler_getAverageReading(void);

long long Sampler_getNumSamplesTaken(void);

