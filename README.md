<p align="center">
  <img src="Images/gsas_logo_white.svg" alt="GSAS-LABS Logo" width="100">
</p>

<h1 align="center">GSAS-LABS </br> P.003-V.1.0.1 </h1>

<p align="center">
  <img src="https://img.shields.io/badge/MCU-ATtiny85-blue" alt="ATtiny85">
  <img src="https://img.shields.io/badge/Shift_Register-74HC595-blue" alt="74HC595">
  <img src="https://img.shields.io/badge/PlatformIO-Embedded-orange" alt="PlatformIO">
  <img src="https://img.shields.io/badge/PCB-2_Layer-green" alt="2 Layer PCB">
  <img src="https://img.shields.io/badge/Power-3V-success" alt="3V">
  <img src="https://img.shields.io/badge/Status-Ready-brightgreen" alt="Ready">
</p>

<p align="center">
  Digital dice based on an ATtiny85 and 74HC595. Supports D4, D6, D8, and D10 modes, with configurable rolling animation, button-based type selection, EEPROM persistence, and low-power sleep mode for extended battery life.
</p>

# 🎲 Digital Dice

<p align="center">
  <img src="Images/hardware_image.png" alt="P.003 Hardware" width="250">
  &nbsp;&nbsp;&nbsp;
  <img src="Images/pcb.png" alt="P.003 PCB" width="250">
  &nbsp;&nbsp;&nbsp;
  <img src="Images/pcb_soldered .png" alt="P.003 Assembled PCB" width="250">
</p>

P.003 is a compact digital dice designed around the ATtiny85 microcontroller.

The project uses a 74HC595 shift register to control the display while minimizing the number of microcontroller pins required.

The dice supports multiple modes and remembers the selected configuration between power cycles.

## Supported Dice

- 🎲 D4
- 🎲 D6
- 🎲 D8
- 🎲 D10

# 🎮 How to Use

## Power Switch

The board includes an ON/OFF power switch.

Move the switch to the **ON** position to power the device.

When the board is powered, the power indicator LED turns on, indicating that the circuit is active and ready to use.

Move the switch to the **OFF** position when the device is not being used. Turning the board off helps preserve battery life.

## Power Indicator

A dedicated LED indicates the power status of the board.

- **LED ON** — The board is powered and ready to use.
- **LED OFF** — The board is switched off.

For maximum battery life, turn the board off when it will not be used for an extended period.

## Buttons

The device includes two buttons:

- **ROLL / RESET**
- **MODE / SELECT**

Both buttons support short and long presses.

### ROLL / RESET

A **short press** rolls the currently selected dice.

The display performs the rolling animation and then shows the generated result.

A **long press** clears the display.

### MODE / SELECT

A **short press** changes the currently selected dice type.

The available dice types are:

```text
D4 → D6 → D8 → D10
```

A **long press** confirms the currently selected dice type.

While selecting a dice type, the led point (`.`) of the 7-segment display is illuminated to indicate that the device is in selection mode.

After confirming the selection with a long press, the decimal point turns off.

The confirmed dice type is remembered by the device and automatically restored the next time the board is powered on.


## Dice Modes

The device supports the following dice types:

| Mode | Possible Results |
|---|---|
| D4 | 1 to 4 |
| D6 | 1 to 6 |
| D8 | 1 to 8 |
| D10 | 0 to 9 |

> In D10 mode, the generated values range from `0` to `9`.

# 🔋 Battery & Power Consumption

The board is powered by a **CR2450 lithium coin cell**.

Power consumption depends on the display state and whether the power indicator LED is installed.

## Power Consumption

| Operating State | Approx. Current |
|---|---:|
| All 7-segment display segments ON | ~7.5 mA |
| Standby / Sleep — power indicator LED installed | ~0.65 mA |
| Standby / Sleep — power indicator LED not installed | ~0.05 mA |

The firmware uses the ATtiny85 **Power-down sleep mode** when the device is not being used. The ADC and unused peripherals are disabled during low-power operation to minimize battery consumption.

The power indicator LED is connected directly to the power circuit and therefore consumes approximately **0.60 mA continuously** when the board is switched ON. This increases the standby consumption from approximately **0.05 mA to 0.65 mA**.

For maximum battery life, omitting the power indicator LED significantly reduces the standby current.

## Estimated Battery Life

Using a CR2450 with a nominal capacity of approximately **620 mAh**:

| Operating State | Current | Theoretical Battery Life |
|---|---:|---:|
| Standby — LED installed | ~0.65 mA | ~954 hours / ~40 days |
| Standby — LED not installed | ~0.05 mA | ~517 days / ~1.42 years |
| All display segments ON continuously | ~7.5 mA | ~83 hours / ~3.4 days |

> Battery life values are theoretical estimates based on the nominal 620 mAh battery capacity and measured current consumption. Actual battery life will be lower and depends on battery voltage, discharge characteristics, temperature, pulse loads, and the device's operating cycle.

> The CR2450's rated capacity is specified under particular discharge conditions. Continuous operation at currents significantly above the manufacturer's standard drain can result in less usable capacity than the nominal 620 mAh rating.

# 🧾 PCB BOM

<p align="center">
  <img src="Images/hardware_vertical_image.png" alt="P.003 PCB" width="500">
</p>

| Reference | Component | Value / Part | Quantity | Unit Price | Total |
|---|---|---|---:|---|---:|
| AFF1 | 7-Segment Display | Common Cathod  | 1 | R$ 2.50 | R$ 2.50 |
| BT1 | Battery Holder | BAT-HLD-006-SMT, 1x CR2450 | 1 | R$ 2.53 | R$ 2.53 |
| D1 | LED | 3 mm Through-Hole LED | 1 | R$ 0.15 | R$ 0.15 |
| Q1 | P-Channel MOSFET | BS250 | 1 | R$ 2.50 | R$ 2.50 |
| R1 | Resistor | 100 kΩ | 1 | R$ 0.05 | R$ 0.05 |
| R2 | Resistor | 2.2 kΩ | 1 | R$ 0.05 | R$ 0.05 |
| R3–R10 | Resistor | 1K Ω | 8 | R$ 0.05 | R$ 0.40 |
| SW1 | Slide Switch | Micro Chave SS12D07 | 1 | R$ 0.50  | R$ 0.50  |
| SW2-SW3 | Push Button | 6 mm Tactile Switch | 2 | R$ 0.15  | R$ 0.30  |
| U1 | Microcontroller | ATTINY85-20PU, DIP-8 | 1 | R$ 26.00 | R$ 26.00 |
| U2 | Shift Register | SN74HC595N, DIP-16 | 1 | R$ 2.52  | R$ 2.52 |

## 💰 Estimated Cost

| Item | Estimated Cost |
|---|---:|
| Electronic Components | R$ 37.50 |
| PCB Manufacturing | R$ 8,50 |
| **Estimated Total Project Cost** | **R$ 46** |

> Component prices may vary depending on supplier, quantity, shipping costs, and location.

> PCB manufacturing costs will be added once the board fabrication service and order quantity are defined.

# 📐 KiCad PCB Configuration

The PCB uses custom routing values configured specifically for this project.

## 📏 Board Dimensions

| Parameter | Dimension |
|---|---:|
| Width | 42 mm |
| Height | 78 mm |

## 🛤️ Track Widths

| Track Width | Recommended Use |
|---|---|
| 0.25 mm | Signal routing |
| 0.40 mm | Power and higher-current connections |

## 🔘 Via Configuration

| Parameter | Value |
|---|---:|
| Via Diameter | 0.8 mm |
| Drill Hole | 0.4 mm |

> These values are configured as custom routing presets in the KiCad project and should be maintained when modifying or extending the PCB design.

# 🛠 Development Environment

The firmware is developed and built using PlatformIO.

The project includes a Makefile that simplifies the most common development tasks.

## Requirements

- macOS
- Homebrew
- PlatformIO

# ⚙️ Makefile

The project provides the following commands.

## Install PlatformIO

```bash
make install
```

This command checks whether PlatformIO is already installed.

If PlatformIO is not available, it installs PlatformIO using Homebrew.

## Update PlatformIO

```bash
make update
```

Updates PlatformIO and prints the installed version.

## Build Firmware

```bash
make build
```

## Program ATtiny85 Fuses

```bash
make fuses
```

## Upload Firmware

```bash
make upload
```

# 🔌 Programming the ATtiny85

The ATtiny85 is programmed using the ISP protocol.

The required signals are:

<p align="center">
  <img src="Images/usbasp_image.png" alt="USBasp Programming Header" width="750">
</p>

| Signal | Function |
|---|---|
| MOSI | Master Out Slave In |
| MISO | Master In Slave Out |
| SCK | Serial Clock |
| RESET | Programming Reset |
| VCC | Target Power |
| GND | Ground |

The board can be programmed using an ISP programmer such as a USBasp.

## ISP Connection

```text
USBasp              ATtiny85 PCB

MOSI   ──────────── MOSI
MISO   ──────────── MISO
SCK    ──────────── SCK
RESET  ──────────── RESET
VCC    ──────────── VCC
GND    ──────────── GND
```

# 🐧 GSAS-LABS

GSAS-LABS is an independent engineering laboratory dedicated to the design, development, and exploration of electronics and software projects. It combines hardware and software engineering to create practical projects focused on embedded systems, microcontrollers, PCB design, and software development.

Areas of interest include:

- Embedded systems
- Microcontrollers
- PCB design
- Hardware prototyping
- Electronics
- Software engineering

# 📜 License

This project combines software and hardware, with each part released under its corresponding open-source license.

## 💻 Firmware

The firmware is licensed under the [GNU General Public License v3.0 (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.html).

The firmware can be used, studied, modified, and redistributed. Any distributed modifications must remain open-source and be released under the GPLv3.

## 🔧 Hardware

The hardware design is licensed under the [CERN Open Hardware Licence v2 (CERN-OHL-S)](https://ohwr.org/licences/).

This includes the hardware design files, such as:

- Schematics
- PCB layouts
- KiCad project files
- Manufacturing files

The hardware design can be used, studied, modified, manufactured, and distributed. Hardware designs based on this project must preserve the same open and strongly reciprocal principles when distributed.

## 🐧 GSAS-LABS Name and Logo

The **GSAS-LABS name, logo, and visual identity are not covered by the software or hardware licenses**.

All rights to the GSAS-LABS name and logo are reserved by their respective owner.

Use of the GSAS-LABS name or logo does not imply endorsement, affiliation, or approval of derivative projects.

---

<p align="center">
  <strong>Experiment · Build · Learn · Share</strong>
</p>

<p align="center">
  Made with ❤️ by GSAS-LABS
</p>

<p align="center">
  <a href="https://github.com/sponsors/gustavoSAS">
    <img src="Images/sponsor-button.svg" alt="Sponsor gustavoSAS" height="36">
  </a>
</p>
