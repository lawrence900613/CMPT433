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


#define MAX_SAMPLES 10000

// Static variables
static pthread_t sampler_thread;
static double current_samples[MAX_SAMPLES];
static double history_samples[MAX_SAMPLES];
static int current_sample_index = 0;
static int history_sample_size = 0;
static long long total_samples_taken = 0;
static int i2c_file_desc;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;




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

// Function to get the current time in milliseconds
long getCurrentTimeMs() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}


// Initialize the sampler
void Sampler_init(void) {
    i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_1);
    keep_sampling = true;
    current_sample_index = 0;
    if (pthread_create(&sampler_thread, NULL, sample_light_levels, NULL) != 0) {
        perror("Failed to create sampler thread");
    }
}

// Function to sample light levels
void* sample_light_levels(void* arg) {
    while (keep_sampling) {
        long start_time = getCurrentTimeMs();
        // int samples_in_one_second = 0;

        while (getCurrentTimeMs() - start_time < 1000) {
            (void)arg;

            // Read a register:
            uint16_t raw_read = read_i2c_reg16(i2c_file_desc, REG_DATA);
            
            // Convert byte order and shift bits into place
            uint16_t value = ((raw_read & 0xFF) << 8) | ((raw_read & 0xFF00) >> 8);
            value = value >> 4;

            // pthread_mutex_lock(&mutex);
            if (current_sample_index < MAX_SAMPLES) {
                current_samples[current_sample_index++] = value;
                total_samples_taken++;
                printf("Sample: %d\n", value);
            }
            // pthread_mutex_unlock(&mutex);

            // samples_in_one_second++;
            sleepForMs(1);
        }

        // printf("Samples in one second: %d\n", current_sample_index);
    }
    return NULL;
}

// Cleanup the sampler
void Sampler_cleanup(void) {
    keep_sampling = false;
    if (pthread_join(sampler_thread, NULL) != 0) {
        perror("Failed to join sampler thread");
    }
    close(i2c_file_desc);
}

// Move current data to history
void Sampler_moveCurrentDataToHistory(void) {
    pthread_mutex_lock(&mutex);
    history_sample_size = current_sample_index;
    for (int i = 0; i < current_sample_index; i++) {
        history_samples[i] = current_samples[i];
    }
    current_sample_index = 0;
    pthread_mutex_unlock(&mutex);
}

// Get the number of samples collected during the previous complete second
int Sampler_getHistorySize(void) {
    pthread_mutex_lock(&mutex);
    int size = history_sample_size;
    pthread_mutex_unlock(&mutex);
    return size;
}

// Get a copy of the samples in the sample history
double* Sampler_getHistory(int *size) {
    pthread_mutex_lock(&mutex);
    *size = history_sample_size;
    double* samples_copy = (double*)malloc(history_sample_size * sizeof(double));
    for (int i = 0; i < history_sample_size; i++) {
        samples_copy[i] = history_samples[i];
    }
    pthread_mutex_unlock(&mutex);
    return samples_copy;
}

// Get the average light level (not tied to the history)
double Sampler_getAverageReading(void) {
    pthread_mutex_lock(&mutex);
    double sum = 0;
    for (int i = 0; i < current_sample_index; i++) {
        sum += current_samples[i];
    }
    double average = (current_sample_index > 0) ? sum / current_sample_index : 0;
    pthread_mutex_unlock(&mutex);
    return average;
}

// Get the total number of light level samples taken so far
long long Sampler_getNumSamplesTaken(void) {
    pthread_mutex_lock(&mutex);
    long long total = total_samples_taken;
    pthread_mutex_unlock(&mutex);
    return total;
}


