# ESP Internal Dynamic Temperature Sensor

A utility library to interface with the internal temperature sensor of ESP-series MCUs.
The library automatically selects the best sensor range based on the previous measurement.
This guarantees readings with the lowest error over the entire supported temperature range without user intervention.

Simply call the function `void read_temp_sensor(float *tsens_value, int8_t *error);` with user-supplied buffers for the value and reported measurement range error tolerance.
All initialization and range handling is done internally automatically.
The behavior of the range switches can be modified with the `set_delta` and `set_cooldown` functions.

Add this library to your esp-idf project by simply adding an `idf_component.yaml` file to your `main` folder with the content
```yaml
dependencies:
    idts:
        version: "*"
        git: "https://github.com/playduck/esp_idts.git"
    idf:
        version: ">=4.1.0"
```
