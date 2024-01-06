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
 - [example_mod_ntc.yaml](example_mod_ntc.yaml): Mod-NTC used by itself
 - [example_mlx90614.yaml](example_mlx90614.yaml): MLX90614 by itself
 - [example_leaf_vpd.yaml](example_leaf_vpd.yaml): MLX90614 and SHT3x sensors to calculate leaf VPD

 ### Temperature Compensation
 For conductivity and pH measurements, the temperature of the liquid is important. For accurate measurements, a temperature sensor is needed. 

 #### Mod-EC
 The component has several optional configuration variables. See [example_mod_ec_dallas.yaml](example_mod_ec_dallas.yaml) for how it is used.
 - `temperature_sensor` - the ID of a temperature sensor, if not provided, 25°C is used. 
 - `temperature_coefficient` - *Defaults to 0.019* - The temperature coefficient to use for compensation. This is usually 0.019 for freshwater/hydroponics/pools, and 0.021 for saltwater. Other liquids have different coefficients. 
 - `temperature_constant` - *Defaults to 25.0* - The temperature to adjust the measurement to. For example, the default of 25°C will adjust the measured value as if the water was 25°C. 

 #### Mod-pH
 The component has several optional configuration variables. See [example_mod_ph_dallas.yaml](example_mod_ph_dallas.yaml)for how it is used.
 - `temperature_sensor` - the ID of a temperature sensor, if not provided, 25°C is used. 

 #### Mod-NTC
 The component has several optional configuration variables. See [example_mod_ntc.yaml](example_mod_ntc.yaml)for how it is used.
 - `beta` - the beta calibration value of the attached NTC temperature sensor.


* * *

### Calibration
Each sensor example comes with calibration controls. They are implemented with lamdas within Home Assistant. 

An example of the lambda:
```yaml
  - platform: template
    id: ec_calibrate_low
    name: EC Calibrate Low 0.1
    icon: mdi:format-vertical-align-bottom
    on_press:
        lambda: |-
          id(ec).calibrateLow(0.1);
```
To change the particular low-point calibration, edit the 2 `0.1` points to match your solution. Do the same thing for mid- and high-points as needed. 

 #### Mod-EC
 The Mod-EC sensor can use up to three calibration points: low, mid, and high. A single point can also be used. Read the [datasheet](https://docs.google.com/document/d/1tfF-OZBhD1JVnNeXnkn0zgdczgs0994KFTN9oT3JPR4/export?format=pdf&ref=microfire-llc) for detailed information. 

 #### Mod-pH
 The Mod-pH sensor can use up to three calibration points: low, mid, and high. A single point can also be used. Read the [datasheet](https://docs.google.com/document/d/1DSG9bdEHDt9mdQInVfCWy4qiohi6sVeEy7QbvBfUmU0/export?format=pdf&ref=microfire-llc) for detailed information. 

 #### Mod-ORP
 The Mod-ORP sensor uses a single point. Read the [datasheet](https://docs.google.com/document/d/1nhQdt0k4pQb8jUJF8Eyrj9TyxYFNImvvaVTNkO53OXs/export?format=pdf&ref=microfire-llc) for detailed information. 

 #### Mod-NTC
 The Mod-NTC sensor uses a beta value, typically provided by the NTC manufacturer. Read the [datasheet](https://docs.google.com/document/d/1xpLEcBNHOzufufisoClGkodkMlKaTpSObjkInAdEXmk/export?format=pdf&ref=microfire-llc) for detailed information. 

### Hardware
All components can be purchased at [microfire.co](https://microfire.co/). 

* * *

### Previous Versions

A big thanks to [Pascal Vizeli](https://github.com/pvizeli?ref=microfire-llc) and [Jesse Hills](https://github.com/jesserockz?ref=microfire-llc) for porting our [older](https://github.com/esphome/esphome/tree/dev/esphome/components/ufire_ec) [sensors](https://github.com/esphome/esphome/tree/dev/esphome/components/ufire_ise) to ESPHome. They were a great starting point for these components.

* * *

### Ask a question 🤙

*   [Discord](https://discord.gg/rAnZPdW)
*   [questions@microfire.co](mailto:questions@microfire.co)

* * *

### Website
[microfire.co](https://microfire.co)
