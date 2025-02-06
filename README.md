# Kirkstone

- Kernel version: 6.0.0
- Released year: 2022
- Kernel support by Adlink(continuing).
- Manifest of the Yocto BSP is [here](https://github.com/ADLINK/adlink-manifest/tree/lec-rb5-yocto-kirkstone).



## 1. Supported Recipes by Adlink

| Resources                           | Description                                                  |
| ----------------------------------- | ------------------------------------------------------------ |
| conf                                | Machine file and configuration files                         |
| recipes-connectivity/adlink-network | Libraries and applications related to communication with other devices |
| recipes-graphics/wayland            | Graphics  related to Wayland                                 |
| recipes-kernel                      | Kernel related configuration changes and patches             |
| recipes-sema/sema                   | Sema related support                                         |
| recipes-tools                       | Adlink tools                                                 |
| recipes-utils/mdio                  | utils mdio                                                   |



## 2. Supported Modules

- LEC-RB5N


## 3. Supported Features & Interfaces

### 3.1 LEC-RB5N (based on I-Pi SMARC plus carrier + LEC-RB5N Dev Kit)

| Interfaces                                 | Support |
| ------------------------------------------ | ------- |
| RAM [LPDDR5(4G/8G)]                        | Y       |
| HDMI                                       | Y       |
| UFS                                        | Y       |
| SEMA 4.0                                   | Y       |
| SPI1                                       | Y       |
| Ethernet - 0 & 1                           | Y       |
| Wi-Fi(optional) [Azurewave AW-CM276NF]     | Y       |
| Bluetooth(optional) [Azurewave AW-CM276NF] | Y       |
| PCIe                                       | Y       |
| USB 2.0                                    | Y       |
| USB 3.0                                    | Y       |
| SER                                        | Y       |
| CAN                                        | Y       |
| SD Card                                    | Y       |
| I2C                                        | Y       |
| GPIO                                       | Y       |
| RTC                                        | Y       |

## 4. Documentation

Refer to the [wiki](https://github.com/ADLINK/meta-adlink-qualcomm/wiki) page for instructions on building the Yocto as well as flashing the image.
