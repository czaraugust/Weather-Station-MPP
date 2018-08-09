import serial
import time
import csv
#csv.register_dialect('pipes', delimiter=' ')		
csv.register_dialect('myDialect', delimiter='*', quoting=csv.QUOTE_NONE, escapechar = ' ', lineterminator = '' )

ser = serial.Serial('COM7', 115200, timeout=0)
 
while 1:
	
	try:

		data = ser.readline().decode()

		if '@' in data:
			
			ts = time.localtime()
			teste = time.strftime("%d-%m-%Y %H:%M:%S", ts) +" " + data.replace('@', '') 
			f = open('panelSensors.csv', 'a')
			
			with f:
				writer = csv.writer(f,dialect='myDialect')
				writer.writerow(teste)

			f.close()			
			
			#print(teste, file = f, end='',flush=True)
			#f.close()

			time.sleep(1)
	except ser.SerialTimeoutException:
		print('Data could not be read')
		time.sleep(1)
