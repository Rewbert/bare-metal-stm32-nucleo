#!/usr/bin/env bash
set -e

git clone git@github.com:ARM-software/CMSIS_5.git vendor/CMSIS
mkdir -p vendor/CMSIS/Device/ST
git clone git@github.com:STMicroelectronics/cmsis-device-l5.git vendor/CMSIS/Device/ST/STM32L5
