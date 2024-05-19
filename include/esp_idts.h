#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"

typedef struct sens_range
{
    int8_t lower_bound;
    int8_t upper_bound;
    int8_t error;
} sens_range_t;

/*
* Supported sensor ranges and their error
* See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/temp_sensor.html
*/
const sens_range_t sens_range_table[] = {
    {.lower_bound = 50, .upper_bound = 125, .error = 3},    /* 0 */
    {.lower_bound = 20, .upper_bound = 100, .error = 2},    /* 1 */
    {.lower_bound = -10, .upper_bound = 80, .error = 1},    /* 2 */
    {.lower_bound = -30, .upper_bound = 50, .error = 2},    /* 3 */
    {.lower_bound = -40, .upper_bound = 20, .error = 3},    /* 4 */
    {.lower_bound = 0, .upper_bound = 0, .error = INT8_MAX} /* out of range */
};

/*
 * set the delta (in degrees Celsius) of how close a measurement is allowed to be to a measurement-range boundary
*/
void set_delta(float new_delta_c);
float delta_c = 2.0F;

/*
* sets the amount of readings that need to be taken after the range has been changed for the sensor to get accustomed
* to the new range. This implements hysteresis and prevents oscillating between ranges
*/
void set_cooldown(uint8_t new_cooldown);
uint8_t cooldown = 2;

/*
* manual installation and uninstallation functions. These don't need to be called manually.
*/
void install_dynamic_temp_sensor(void);
void uninstall_dynamic_temp_sensor(void);

/*
* get a measurement and the associated error depending on the range the measurement was taken in
*/
void read_temp_sensor(float *tsens_value, int8_t *error);
