import serial
import io
import time
import csv
import math
import pandas as pd
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt



def make_serial_obj(portpath, baudrate, tout):
	"""
	Creates a serial link, given the path to the board, 
	the baudrate, and the timeout length in seconds.
	"""
	return serial.Serial(portpath, baudrate, timeout = tout)


def read_data(serial):
	"""
	Sends the data command over serial, then takes the data and 
	stores it in an array in Python. 
	One serial line per array entry.
	"""
	serial.flushInput()	

	serial_data = []
	still_reading = True
	
	send("data", serial)
	time.sleep(1)
	
	while still_reading:

		cur_line = serial.readline().decode('utf-8')

		if "NOK" in cur_line:
			still_reading = False
			print("Error")
		elif "OK" in cur_line:
			still_reading = False
			print("Finished Reading")
		else:
			serial_data.append(cur_line)
	
	return serial_data


def print_serial(serial):
	"""
	Prints (in Python) everything that is being sent over 
	the serial line.
	"""
	for line in serial.readlines() : print(line.decode('utf-8'))


def lux_to_mag(lux):
	"""
	Converts from lux to mags/arcsec2
	"""
	cdm2 = lux / 3.14159
	mags = round( ((math.log10(cdm2 / 108000)) / -0.4), 3)
	
	return mags


def clean_data(serial, dirty_data):
	"""
	This takes the data array generated by read_data, 
	and cleans it for use in a pandas dataframe / csv file.
	"""
	clean_data = [line for line in dirty_data if "D" in line[1]]
	clean_data = [line[3:-1] for line in clean_data]
	clean_data_2D = [line.split(",") for line in clean_data]

	for line in clean_data_2D:
		line[5] = float(line[5])
		line[0] = int(line[0])
		line[3] = float(line[3])
		line[4] = int(line[4])
		lux = line[5]
		mag = lux_to_mag(lux)
		line.append(mag)

	return clean_data_2D


def log_dump(serial):
	"""
	Writes the log from the board to a text file called log.txt,
	will make this have a unique name eventually.
	"""
	serial.flushInput()
	
	log_file = open("log.txt", "w+") # make this use the hash for light sensor as title later
	still_reading = True
	
	send("log", serial)
	time.sleep(1)
	
	while still_reading:

		cur_line = serial.readline().decode('utf-8')

		if "NOK" in cur_line:
			still_reading = False
			print("Error")
		elif "OK" in cur_line:
			still_reading = False
			print("Finished Log Dump")
		else:
			log_file.write("%s\n\r" % cur_line)

	log_file.close()


def data_dump(serial):
	"""
	Writes the data to a text file for sanity check.
	"""
	serial.flushInput()	

	data_file = open("data.txt", "w+")
	serial_data = []
	still_reading = True
	
	send("data", serial)
	time.sleep(1)
	
	while still_reading:

		cur_line = serial.readline().decode('utf-8')

		if "NOK" in cur_line:
			still_reading = False
			print("Error")
		elif "OK" in cur_line:
			still_reading = False
			print("Finished Reading")
		else:
			data_file.write("%s" % cur_line)

	
	data_file.close()


def send(string, serial):
	"""
	Sends commands (or any string) to the light sensor via serial.
	"""
	for char in string:
		serial.write(str(char).encode('utf-8'))
		time.sleep(0.2)
	serial.write(str("\n").encode('utf-8'))


def set_current_datetime(serial):
	"""
	Pulls the current date and time and sets it on the light sensor board.
	"""
	cur_time = time.localtime()
	date_code = "ds,{},{},{}".format(cur_time[1], cur_time[2], cur_time[0] % 100)
	time_code = "ts,{},{},{}".format(cur_time[3], cur_time[4], cur_time[5])
	send(time_code, serial)
	send(date_code, serial)


def save_to_csv(clean_data, filename):
	"""
	Write the cleaned data to a csv file with name of 'filename'
	"""
	with open(filename, 'w') as csvfile:
		csvwrite = csv.writer(csvfile)
		csvwrite.writerows(clean_data)





"""
Example script that reads the data and outputs to a csv file.
"""

portpath = "/dev/ttyACM0"
baudrate = 9600
timeout = 5
filename = "data.csv"


print("Creating serial link...")
ser = make_serial_obj(portpath, baudrate, timeout)

print("Reading Data...")
dirty_data = read_data(ser)
print("Cleaning Data...")
clean_data = clean_data(ser, dirty_data)
print("Saving to file %s.csv...", filename)
save_to_csv(clean_data, filename)
print("Complete")

