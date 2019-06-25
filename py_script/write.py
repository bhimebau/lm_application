import serial
import io
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=10)
# port, baud rate, timeout

print("Port ", ser.name)
# prints the port that was used

while True:
	ser.write(b'@') # write wakeup char
	time.sleep(1) # wait
	ack = ser.readline() # read serial for response
	#print(ack.strip())
	if ack.strip() == b'OK': # check if system is awake
	# strip only necissary because arduino returns \nb'OK\n\r
		print("PASSED ACK")
		break # continue
	else:
		print("ERROR ON WAKE, RETRYING") # print error

# gets current date and time
cur_time = time.localtime()
date_code = "ds,{},{},{}".format(cur_time[1], cur_time[2], cur_time[0]).encode()
# formats the date
time_code = "ts,{},{},{}".format(cur_time[3], cur_time[4], cur_time[5]).encode()
# formats the time

ser.write(date_code)
print(ser.readline())
ser.write(time_code)
print(ser.readline())
# need to work out timing to see if sleep is needed
# but this should write the date and time to the board
