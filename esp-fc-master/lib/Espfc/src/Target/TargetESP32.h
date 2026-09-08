#pragma once

// ============================================================
// ESP32 Flight Controller Target Configuration
// Board: ESP32 DevKit v1 (30-pin)
// Gyro:  MPU6500 (SPI, CS=GPIO5)
// Accel: Built-in MPU6500 + ADXL345 (I2C, addr=0x53)
// RX:    FlySky IA6B iBus (Serial2 RX=GPIO35)
// ESCs:  4x BLHeli_S (DShot300, pins 4,12,14,27)
// Batt:  2000mAh 3S LiPo (ADC1_CH0=GPIO36)
// I2C:   GPIO21(SDA), GPIO22(SCL) — ADXL345, future BMP280
// Buzzer: GPIO26 (active low)
// LED:   GPIO2 (built-in)
// ============================================================

// NOTE: ESPFC_INPUT is defined for pin array compatibility, but ESPFC_INPUT_PIN is -1 (unused).
// GPIO35 is used by Serial2 RX (iBus), not PPM. PPM input is NOT supported on this build.
// The receiver communicates via Serial2 using FlySky iBus protocol (bidirectional half-duplex).
// PPM is never activated since FEATURE_RX_PPM is excluded from ESPFC_FEATURE_MASK.
#define ESPFC_INPUT
#define ESPFC_INPUT_PIN -1  // PPM not used — GPIO35 is Serial2 RX (iBus)

#define ESPFC_OUTPUT_COUNT 8
#define ESPFC_OUTPUT_0 4   // M1 - front-right
#define ESPFC_OUTPUT_1 12  // M2 - rear-right
#define ESPFC_OUTPUT_2 14  // M3 - rear-left
#define ESPFC_OUTPUT_3 27  // M4 - front-left
#define ESPFC_OUTPUT_4 -1
#define ESPFC_OUTPUT_5 -1
#define ESPFC_OUTPUT_6 -1
#define ESPFC_OUTPUT_7 -1

// Serial0: USB / UART0 — MSP (Configurator / CLI)
#define ESPFC_SERIAL_0
#define ESPFC_SERIAL_0_DEV Serial
#define ESPFC_SERIAL_0_DEV_T HardwareSerial
#define ESPFC_SERIAL_0_TX 1
#define ESPFC_SERIAL_0_RX 3
#define ESPFC_SERIAL_0_FN (SERIAL_FUNCTION_MSP)
#define ESPFC_SERIAL_0_BAUD (SERIAL_SPEED_115200)
#define ESPFC_SERIAL_0_BBAUD (SERIAL_SPEED_NONE)

// Serial1: UART1 — GPS (future expansion, TX=GPIO33, RX=GPIO32)
#define ESPFC_SERIAL_1
#define ESPFC_SERIAL_1_DEV Serial1
#define ESPFC_SERIAL_1_DEV_T HardwareSerial
#define ESPFC_SERIAL_1_TX 33
#define ESPFC_SERIAL_1_RX 32
#define ESPFC_SERIAL_1_FN (SERIAL_FUNCTION_GPS)
#define ESPFC_SERIAL_1_BAUD (SERIAL_SPEED_115200)
#define ESPFC_SERIAL_1_BBAUD (SERIAL_SPEED_NONE)

// Serial2: UART2 — FlySky iBus RC input + iBus Telemetry (TX=GPIO25, RX=GPIO35)
// iBus is half-duplex: connect receiver iBus pin to BOTH TX25 and RX35 via a 1k resistor
// on the TX line, or connect only RX35 if telemetry is not needed.
#define ESPFC_SERIAL_2
#define ESPFC_SERIAL_2_DEV Serial2
#define ESPFC_SERIAL_2_DEV_T HardwareSerial
#define ESPFC_SERIAL_2_TX 25
#define ESPFC_SERIAL_2_RX 35
#define ESPFC_SERIAL_2_FN (SERIAL_FUNCTION_RX_SERIAL | SERIAL_FUNCTION_TELEMETRY_IBUS)
#define ESPFC_SERIAL_2_BAUD (SERIAL_SPEED_115200)
#define ESPFC_SERIAL_2_BBAUD (SERIAL_SPEED_NONE)

#define ESPFC_SERIAL_SOFT_0
#define ESPFC_SERIAL_SOFT_0_FN (SERIAL_FUNCTION_MSP)
#define ESPFC_SERIAL_SOFT_0_WIFI

#define ESPFC_SERIAL_REMAP_PINS
#define ESPFC_SERIAL_DEBUG_PORT SERIAL_UART_0

// SPI: MPU6500 gyro (CS=GPIO5, SCK=GPIO18, MOSI=GPIO23, MISO=GPIO19)
#define ESPFC_SPI_0
#define ESPFC_SPI_0_DEV SPI1
#define ESPFC_SPI_0_SCK 18
#define ESPFC_SPI_0_MOSI 23
#define ESPFC_SPI_0_MISO 19

#define ESPFC_SPI_CS_GYRO 5
#define ESPFC_SPI_CS_BARO 15

// I2C: ADXL345 + future BMP280 baro (SDA=GPIO21, SCL=GPIO22)
#define ESPFC_I2C_0
#define ESPFC_I2C_0_SCL 22
#define ESPFC_I2C_0_SDA 21
#define ESPFC_I2C_0_SOFT

#define ESPFC_BUZZER_PIN 26
#define ESPFC_BUTTON_PIN 0
#define ESPFC_LED_PIN 2

// ADC: Battery voltage (GPIO36 = ADC1_CH0)
// Voltage divider: 100k+100k (top) / 100k (bottom) = 2:1 ratio
// 3S LiPo full = 12.6V → 4.2V at ADC pin (within 3.3V ESP32 range via divider)
// ADC_SCALE = reference_voltage / ADC_resolution = 3.3V / 4096 = 0.00080566
// Voltage compensation: actual = adc_val * ADC_SCALE * ((R1+R2)/R2)
// Default divider: (100000+100000)/100000 = 2.0 → VBAT_SCALE=200 (in EEPROM)
#define ESPFC_ADC_0
#define ESPFC_ADC_0_PIN 36

#define ESPFC_ADC_1
#define ESPFC_ADC_1_PIN 39

#define ESPFC_ADC_SCALE (3.3f / 4096)

// Features enabled by default:
//   FEATURE_RX_SERIAL   - Serial-based receiver (iBus on Serial2)
//   FEATURE_DYNAMIC_FILTER - Dynamic notch filter (auto gyro noise rejection)
//   FEATURE_TELEMETRY   - Enable telemetry output (iBus back to TX)
#define ESPFC_FEATURE_MASK (FEATURE_RX_SERIAL | FEATURE_DYNAMIC_FILTER | FEATURE_TELEMETRY)

#define ESPFC_GYRO_I2C_RATE_MAX 2000
#define ESPFC_GYRO_SPI_RATE_MAX 4000

#define ESPFC_DSHOT_TELEMETRY

#define ESPFC_FREE_RTOS
#ifndef CONFIG_FREERTOS_UNICORE
#define ESPFC_MULTI_CORE
#endif

// #define ESPFC_FREE_RTOS_QUEUE
#define ESPFC_ATOMIC_QUEUE

#define ESPFC_DSP

// ============================================================
// Development Presets (applied only when ESPFC_REVISION is not defined)
// ============================================================
#define ESPFC_DEV_PRESET_MODES      // Arm on AUX1 > 1300, Angle on AUX2 > 1300
#define ESPFC_DEV_PRESET_DSHOT      // DShot300 protocol for BLHeli_S ESCs
#define ESPFC_DEV_PRESET_IBUS       // FlySky iBus serial RX provider
// #define ESPFC_DEV_PRESET_BLACKBOX_SERIAL=1  // Enable blackbox on Serial1
// #define ESPFC_DEV_PRESET_SCALER             // Enable PID scaling on AUX channels

#include "./TargetEsp32Common.h"
