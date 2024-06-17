# -*- coding: utf-8 -*-

import time
import random
import sys
import Jetson.GPIO as GPIO
import serial

SPICLK = 11
SPIMISO = 9
SPIMOSI = 10
SPICS = 8
output_pin = 7
photo_ch = 0

def init():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(output_pin, GPIO.OUT, initial=GPIO.HIGH)
    GPIO.setwarnings(False)
    GPIO.cleanup()
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(SPIMOSI, GPIO.OUT)
    GPIO.setup(SPIMISO, GPIO.IN)
    GPIO.setup(SPICLK, GPIO.OUT)
    GPIO.setup(SPICS, GPIO.OUT)

def readadc(adcnum, clockpin, mosipin, misopin, cspin):
    if ((adcnum > 7) or (adcnum < 0)):
        return  -1
    GPIO.output(cspin, True)

    GPIO.output(clockpin, False)
    GPIO.output(cspin, False)

    commandout = adcnum
    commandout |= 0x18
    commandout <<= 3
    for i in range(5):
        if commandout & 0x80:
            GPIO.output(mosipin, True)
        else:
            GPIO.output(mosipin, False)
        commandout <<= 1
        GPIO.output(clockpin, True)
        GPIO.output(clockpin, False)

    adcout = 0
    for i in range(12):
        GPIO.output(clockpin, True)
        GPIO.output(clockpin, False)
        adcout <<= 1
        if GPIO.input(misopin):
            adcout |= 0x1
    GPIO.output(cspin, True)
    adcout >>= 1
    return adcout

def read_light_sensor():
    return readadc(photo_ch,SPICLK,SPIMOSI,SPIMISO,SPICS)

def read_humidity_sensor():
    line = "error"  # Initialize line with an empty string
    ser = serial.Serial('/dev/ttyUSB0', 9600)
    if ser.is_open:
        try:
            while True:
                data_raw = ser.readline()  # 讀取一行
                data = data_raw.decode()   # 用預設的UTF-8解碼
                return data
        except KeyboardInterrupt:
            print("中斷")
        finally:
            ser.close()
    else:
        print("無法打開串口")
    return line

def main():
    init()
    while True:
        light = read_light_sensor()
        humidity = read_humidity_sensor()
        file='output.txt' 
        with open(file, 'w') as filetowrite:
            filetowrite.write(f"R\nLight: {light} Humidity: {humidity}")
        time.sleep(5) 

if __name__ == '__main__':
    main()
