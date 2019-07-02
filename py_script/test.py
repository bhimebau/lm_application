import serial
import io
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=3)

def init():
	for line in ser.readlines(): pass
	print("Port ", ser.name)
	#ser.write(str("@\n").encode('utf-8'))	

def read():
	for line in ser.readlines() : print(line.decode('utf-8'))

def get_date():
	ser.write(str("dr").encode('utf-8'))
	time.sleep(1)
	ser.write(str("\n").encode('utf-8'))
	time.sleep(1)
	read()

def send(foo):
	for char in foo:
		ser.write(str(char).encode('utf-8'))
		time.sleep(0.2)
	ser.write(str("\n").encode('utf-8'))
	

init()
send("dr")
read()
