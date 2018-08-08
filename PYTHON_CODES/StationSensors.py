import paho.mqtt.client as mqtt
import sys
import time

Broker = "localhost"
PortaBroker = 1883
KeepAliveBroker = 60
TopicoSubscribe = "station" 

def on_connect(client, userdata, flags, rc):
    client.subscribe(TopicoSubscribe)

def on_message(client, userdata, msg):
	MensagemRecebida = str(msg.payload)
	ts = time.localtime()
	teste = time.strftime("%d-%m-%Y %H:%M:%S", ts) + " "+ MensagemRecebida.replace('b', '').replace('\'', '')
	f = open('stationsensors.txt', 'a')
	print(teste, file = f, flush=True)
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