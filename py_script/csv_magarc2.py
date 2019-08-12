"""
Example script that reads the data and outputs to a csv file.
"""
from lib import *


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
