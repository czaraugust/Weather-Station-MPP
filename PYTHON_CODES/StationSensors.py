import paho.mqtt.client as mqtt
import sys
import time
import csv
csv.register_dialect('myDialect', delimiter='*', quoting=csv.QUOTE_NONE, escapechar = ' ', lineterminator = '\n' )

Broker = "localhost"
PortaBroker = 1883
KeepAliveBroker = 60
TopicoSubscribe = "station" 

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



def on_connect(client, userdata, flags, rc):
    client.subscribe(TopicoSubscribe)

def on_message(client, userdata, msg):
	MensagemRecebida = str(msg.payload)
	ts = time.localtime()
	MensagemRecebida.replace('b', '');
	if (prints ==1):
		print (time.strftime("%d-%m-%Y %H:%M:%S", ts) + " "+ MensagemRecebida)
		
	teste = time.strftime("%d-%m-%Y %H:%M:%S", ts) + " "+ MensagemRecebida
		
	f = open('stationSensors.csv', 'a')
	with f:
		writer = csv.writer(f,dialect='myDialect')
		writer.writerow(teste)

	f.close()	
	
client = mqtt.Client()
client.on_connect = on_connect
client.connect(Broker, PortaBroker, KeepAliveBroker)	

while 1:
	
	try:
		client.on_message = on_message
		client.loop_forever()
		
	except KeyboardInterrupt:
			print ("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
			sys.exit(0)