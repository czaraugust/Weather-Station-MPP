import serial
import time

ser = serial.Serial('COM7', 115200, timeout=0)
 
while 1:
	
	try:

		data = ser.readline().decode()

		if '@' in data:
			
			ts = time.localtime()
			teste = time.strftime("%d-%m-%Y %H:%M:%S", ts) +" " + data.replace('@', '')
			f = open('panelsensors.txt', 'a')
			print(teste, file = f, end='',flush=True)
			f.close()

			time.sleep(1)
	except ser.SerialTimeoutException:
		print('Data could not be read')
		time.sleep(1)
