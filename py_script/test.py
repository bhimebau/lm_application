import serial
import io
import time
import csv
import pandas as pd
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt


portpath = "/dev/ttyACM0"
baudrate = 115200
timeout = 5
filename = "data.csv"


def make_serial_obj(portpath, baudrate, tout):
	return serial.Serial(portpath, baudrate, timeout = tout)


def read_data(serial):
	serial.flushInput()

	serial_data = []
	still_reading = True
	
	while still_reading:

		cur_line = serial.readline().decode('utf-8')

		if cur_line == "NOK":
			still_reading = False
			print("Error")
		elif cur_line == "":
			still_reading = False
			print("No Data")
		elif cur_line == "OK":
			still_reading = False
			print("Finished Reading")
		else:
			serial_data.append(serial_line)
	
	return serial_data
	

def print_serial(serial):
	for line in serial.readlines() : print(line.decode('utf-8'))


def get_date():
	ser.write(str("dr").encode('utf-8'))
	time.sleep(1)
	ser.write(str("\n").encode('utf-8'))
	time.sleep(1)
	read()


def send(string, serial):
	for char in string:
		serial.write(str(char).encode('utf-8'))
		time.sleep(0.2)
	serial.write(str("\n").encode('utf-8'))


def set_current_datetime(serial):
	cur_time = time.localtime()
	date_code = "ds,{},{},{}".format(cur_time[1], cur_time[2], cur_time[0])
	time_code = "ts,{},{},{}".format(cur_time[3], cur_time[4], cur_time[5])
	send(time_code, serial)
	send(date_code, serial)


def save_to_csv(data, filename):
	with open(filename, 'wb') as csvfile:
		csvwrite = csv.writer(csvfile)
		csvwrite.writerows(data)

print("Creating serial link...")
ser = make_serial_obj(portpath, baudrate, timeout)

print("Setting current date...")
set_current_datetime(ser)
print_serial(ser)

print("Reading data...")
read_data(ser)

