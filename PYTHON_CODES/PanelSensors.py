import serial
import time
import csv
import sys
#csv.register_dialect('pipes', delimiter=' ')		
csv.register_dialect('myDialect', delimiter='*', quoting=csv.QUOTE_NONE, escapechar = ' ', lineterminator = '' )
ser = serial.Serial('COM7', 115200, timeout=0)
prints = 0;

if (len(sys.argv) >1):
	if (str(sys.argv[1]) == 'p'): 	
		print("Imprimindo Valores");
		prints =1;
	else:
		print("nada");
else:
	print("Não imprimindo Valores");
	prints =0;
 
while 1:
	
	try:

		data = ser.readline().decode()

		if '@' in data:
			
			ts = time.localtime()
			teste = time.strftime("%d-%m-%Y %H:%M:%S", ts) +" " + data.replace('@', '') 
			f = open('panelSensors.csv', 'a')
			if (prints ==1):
				print (teste);
				
				
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
