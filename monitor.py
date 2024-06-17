# -*- coding: utf-8 -*-
import time
import sys
import signal
from datetime import datetime, timedelta
import os

light = 0
wet = 0

def readFromOutput():
    global light, wet
    with open('output.txt',mode='r+') as f:
            Rdata = f.read()
            data = Rdata.split()
            if(data[0] == 'R'):
                data = Rdata.split()
                data[0] = 'W\n'
                light = int(data[2])
                wet = int(data[4])
                data = ' '.join(data)
                f.seek(0)
                f.write(data)
                f.truncate()
            else:
                print("read error")

def start():
    readFromOutput()
    with open('log.txt', mode='a+') as f:
        f.seek(0)
        if not f.read(1):
            f.write(datetime.now().strftime("%Y-%m-%d %H:%M:%S") + f' {light} {wet} 0\n')
        else:
            print('start error')

def update():
    readFromOutput()
    log_file = 'log.txt'
    if os.path.exists(log_file):
        with open(log_file, mode='a+') as f:
            f.seek(0)
            lines = f.readlines()
            if lines:
                last_line = lines[-1].strip()
                last_date_str,last_time_str, last_light_str, last_wet_str, last_log_value_str = last_line.split()
                last_time = datetime.strptime(last_date_str + " " + last_time_str, "%Y-%m-%d %H:%M:%S")
                current_time = datetime.now()
                time_interval = (current_time - last_time).seconds / 60
                log_value = time_interval * ((100 - wet) / 100) * ((1024 - light) / 1024)+ float(last_log_value_str)
                f.write(f'{current_time.strftime("%Y-%m-%d %H:%M:%S")} {light} {wet} {log_value}\n')
                print(f'{current_time.strftime("%Y-%m-%d %H:%M:%S")} {light} {wet} {log_value}',end = '')
                if log_value >= 360:
                    print(' True')
                else:
                    print(' False')


def exit():
    os.remove('log.txt')


def main():
    command = sys.argv[1]
    if command == "start":
        start()
    elif command == "update":
        update()
    elif command == "exit":
        exit()
    sys.stdout.flush()

if __name__ == '__main__':
    main()
