#include <stdio.h>
#include <inttypes.h>

#include "esp_log.h"
#include "driver/temperature_sensor.h"

#include "esp_idts.h"

static const char *TAG = "IDTS";

static bool configured = false;
static temperature_sensor_handle_t temp_sensor = NULL;

static uint8_t current_range = 0;
static uint8_t switch_cooldown = 0;

void set_delta(float new_delta_c)   {
    delta_c = new_delta_c;
}

void set_cooldown(uint8_t new_cooldown) {
    cooldown = new_cooldown;
}

void uninstall_dynamic_temp_sensor(void)
{
    ESP_LOGD(TAG, "Uninstalling internal temperature sensor");

    configured = false;
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_sensor));
    ESP_ERROR_CHECK(temperature_sensor_uninstall(temp_sensor));
    temp_sensor = NULL;
}

void install_dynamic_temp_sensor(void)
{
    if (configured)
    {
        uninstall_dynamic_temp_sensor();
    }

    ESP_LOGD(TAG, "Installing internal temperature sensor");

    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(sens_range_table[current_range].lower_bound, sens_range_table[current_range].upper_bound);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    ESP_LOGD(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    configured = true;
}

int8_t select_best_range(float measurement)
{
    int8_t best_range = current_range;
    int8_t best_score = sens_range_table[current_range].error;

    // Check if the measurement is close to the current range's boundary
    if (measurement >= sens_range_table[current_range].lower_bound + delta_c ||
        measurement <= sens_range_table[current_range].upper_bound - delta_c)
    {
        best_score = INT8_MAX;
        ESP_LOGD(TAG, "Measurement is close too current range boundary");
    }

    // Check for a better range
    for (int8_t i = 0; i < sizeof(sens_range_table) / sizeof(sens_range_table[0]); ++i)
    {

        if (measurement >= sens_range_table[i].lower_bound + delta_c &&
            measurement <= sens_range_table[i].upper_bound - delta_c &&
            sens_range_table[i].error <= best_score)
        {

            best_range = i;
            best_score = sens_range_table[i].error;
        }
    }

    return best_range;
}

void read_temp_sensor(float *tsens_value, int8_t *error)
{
    if (!configured || temp_sensor == NULL)
    {
        install_dynamic_temp_sensor();
    }
    *error = sens_range_table[current_range].error;

    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, tsens_value));
    ESP_LOGI(TAG, "Temp %.02f ℃  (range %i, error <%i℃ )", *tsens_value, current_range, *error);

    // get recommended range based on current measurement
    uint8_t proposed_range = select_best_range(*tsens_value);

    // check if we recently switched range
    if (switch_cooldown > 0)
    {
        switch_cooldown = switch_cooldown - 1;
    }

    if (switch_cooldown == 0 && proposed_range != current_range)
    {
        if (sens_range_table[proposed_range].error == INT8_MAX)
        {
            ESP_LOGW(TAG, "Potentially out of range");
        }
        else
        {
            ESP_LOGD(TAG, "Changing from range %i to %i", current_range, proposed_range);
            current_range = proposed_range;
            switch_cooldown = cooldown;
            uninstall_dynamic_temp_sensor(); // uninstall the driver so we automatically re-install on the next measurement
        }
    }
    else
    {
        ESP_LOGD(TAG, "Already in the ideal range");
    }
}
