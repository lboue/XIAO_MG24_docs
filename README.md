#  Seeed-Studio XIAO MG24 docs

XIAO MG24 - Silicon Labs EFR32MG24, Matter® Native over Thread/BLE 5.3
- Arduino Compatible, 5μA Ultra-Low-Power, On-Board Antenna, 19 GPIOs
- Product page: [Seeed-Studio XIAO-MG24](https://www.seeedstudio.com/Seeed-Studio-XIAO-MG24-p-6247.html)
- Wiki: [Getting Started with Seeed Studio XIAO MG24](https://wiki.seeedstudio.com/xiao_mg24_getting_started/)

## Hardware

- [Schematic](https://files.seeedstudio.com/wiki/XIAO_MG24/Getting_Start/XIAO_MG24_v1_0_SCH_PDF.pdf)

### Pinout

- [Pinout diagram](https://media-cdn.seeedstudio.com/media/wysiwyg/102010610HO2.jpg)

| MG24 Pin |   | Arduino | i²C  | SPI   | Serial | Other  |
|----------|---|---------|------|-------|--------|--------|
| PA00     |   |         |      | SCK1  |        |        |
| PA03     |   |         |      | SCK0  |        |        |
| PA04     |   |         |      | MISO0 |        |        |
| PA05     |   |         |      | MOSI0 |        |        |
| PA07     |   |         |      |       |        | USER_LED |
| PA08     |   | D12     |      |       |        |        |
| PA09     |   | D11     |      |       |        |        |
|          |   |         |      |       |        |        |
| PB00     |   | D15     |      | MOSI1 |        |        |
| PB01     |   | D16     |      | MISO1 |        |        |
| PB02     |   | D13     | SCL1 |       |        |        |
| PB03     |   | D14     | SDA1 |       |        |        |
| PB04     |   |         |      |       |        |  RF SW |
|          |   |         |      |       |        |        |
| PC00     |   | D0      |      |       |        |        |
| PC01     |   | D1      |      |       |        |        |
| PC02     |   | D2      |      |       |        |        |
| PC03     |   | D3      |      |       |        |        |
| PC04     |   | D4      | SDA0 |       |        |        |
| PC05     |   | D5      | SCL0 |       |        |        |
| PC06     |   | D6      |      |       | TX0    |        |
| PC07     |   | D7      |      |       | RX0    |        |
| PC08     |   |         |      |       |        | MIC_EN     |
| PC09     |   |         |      |       |        | MIC_ADC    |


## Software

### Arduino

- [Silicon Labs Arduino Core](https://github.com/SiliconLabs/arduino)

### Silicon Labs

- [Silicon Labs Matter](https://docs.silabs.com/matter/latest/matter-start/)
- [EFR32 Matter Application Examples](https://github.com/SiliconLabs/matter_applications)
- [Silicon Labs Simplicity Studio Extension for Matter](https://github.com/SiliconLabs/matter_extension)
