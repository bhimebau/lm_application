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
		print("PASSED ACK")
		break # continue
	else:
		print("ERROR ON WAKE, RETRYING") # print error

cur_time = time.localtime()
date_code = "{},{},{} {}:{}.{}".format(cur_time[1], cur_time[2], cur_time[0], 
									   cur_time[3], cur_time[4], cur_time[5])

# TODO
# figure out the format that data returns, so it can be written to file

f = open(date_code, "w+")
# write data
f.close()
