#include <stdio.h>
#include <inttypes.h>

#ifndef __ESP_IDTS_H__
#define __ESP_IDTS_H__

typedef struct sens_range
{
    int8_t lower_bound;
    int8_t upper_bound;
    int8_t error;
} sens_range_t;


/*
 * set the delta (in degrees Celsius) of how close a measurement is allowed to be to a measurement-range boundary
*/
void set_delta(float new_delta_c);
/*
* sets the amount of readings that need to be taken after the range has been changed for the sensor to get accustomed
* to the new range. This implements hysteresis and prevents oscillating between ranges
*/
void set_cooldown(uint8_t new_cooldown);
/*
* manual installation and uninstallation functions. These don't need to be called manually.
*/
void install_dynamic_temp_sensor(void);
void uninstall_dynamic_temp_sensor(void);

/*
* get a measurement and the associated error depending on the range the measurement was taken in
*/
void read_temp_sensor(float *tsens_value, int8_t *error);

#endif
