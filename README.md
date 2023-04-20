ESPHome Components
======

This repo contains components for Microfire sensors that can be used in ESPHome and Home Assistant. 

### Use
There are several examples:
 - [example_mod_ec.yaml](example_mod_ec.yaml): Mod-EC used by itself
 - [example_mod_ec_dallas.yaml](example_mod_ec_dallas.yaml): Mod-EC that uses a Dallas DS18B20 as a water temperature source
 - [example_mod_ph.yaml](example_mod_ph.yaml): Mod-pH used by itself
 - [example_mod_ph_dallas.yaml](example_mod_ph_dallas.yaml): Mod-pH that uses a Dallas DS18B20 as a water temperature source
  - [example_mod_orp.yaml](example_mod_orp.yaml): Mod-ORP used by itself
 - [example_ec_ph_dallas.yaml](example_ec_ph_dallas.yaml): Mod-EC, Mod-pH and DS18B20 used together
 - [example_ec_ph_orp_dallas.yaml](example_ec_ph_orp_dallas.yaml): Mod-EC, Mod-pH, Mod-ORP and DS18B20 used together

 ### Temperature Compensation
 For conductivity and pH measurements, the temperature of the liquid is important. For accurate meausurements, a temperature sensor is needed. 

 #### Mod-EC
 The component has several optional configuration variables. See [example_mod_ec_dallas.yaml](example_mod_ec_dallas.yaml) for how it is used.
 - `temperature_sensor` - the ID of a temperature sensor, if not provided, 25°C is used. 
 - `temperature_coefficient` - *Defaults to 0.019* - The temperature coefficient to use for compensation. This is usually 0.019 for freshwater/hydroponis/pools, and 0.021 for saltwater. Other liquids have different coefficients. 
 - `temperature_constant` - *Defaults to 25.0* - The temperature to adjust the measurement to. For example, the default of 25°C will adjust the measured value as if the water was 25°C. 

 #### Mod-pH
 The component has several optional configuration variables. See [example_mod_ph_dallas.yaml](example_mod_ph_dallas.yaml)for how it is used.
 - `temperature_sensor` - the ID of a temperature sensor, if not provided, 25°C is used. 

* * *

### Hardware
All components can be purchased at [microfire.co](https://microfire.co/). 

* * *

### Ask a question 🤙

*   [Discord](https://discord.gg/rAnZPdW)
*   [questions@microfire.co](mailto:questions@microfire.co)

* * *

### Website
[microfire.co](https://microfire.co)
