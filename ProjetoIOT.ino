#include <WiFi.h>
#include <Arduino.h>
#include <PubSubClient.h>

#define PIN_LED 2
#define LED_BUILTIN 2

#define ID_MQTT "IoT_PUC_SG_mqtt"
#define TOPICO_SUBSCRIBE_LED  "topico_liga_desliga_led"
#define TOPICO_PUBLISH_TEMPERATURA  "topico_sensor_temperatura"

const char* SSID = "Moto E⁶ Plus";
const char* PASSWORD = "ajuda123";

const char* BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;

WiFiClient espClient;
PubSubClient MQTT(espClient);

long numAleatorio;

void initWiFi(void);
void initMQTT(void);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
  
void initWiFi(void)
{
	delay(10);
	Serial.print("\t\t ⇒ Conexão WiFi ⇐\n");
	Serial.print("\t › Conectando-se na rede: ");
	Serial.print(SSID);
	Serial.println("\t › Aguarde..... \n");
	reconnectWiFi();
}

void initMQTT(void)
{
	MQTT.setServer(BROKER_MQTT, BROKER_PORT);
	MQTT.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
	String msg;

	for (int i = 0; i < length; i++)
	{
		char c = (char)payload[i];
		msg += c;
	}
	Serial.print("Chegou a seguinte string via MQTT: ");
	Serial.println(msg);

	if (msg.equals("L"))
	{
		digitalWrite(PIN_LED, HIGH);
		Serial.println("LED aceso mediante comando MQTT");
	}
	if (msg.equals("D"))
	{
		digitalWrite(PIN_LED, LOW);
		Serial.println("LED apagado mediante comando MQTT");
	}
}

void reconnectMQTT(void)
{
	while (!MQTT.connected())
	{
		Serial.print("* Tentando se conectar ao Broker MQTT: ");
		Serial.println(BROKER_MQTT);
		if (MQTT.connect(ID_MQTT))
		{
			Serial.println("Conectado com sucesso ao broker MQTT!");
			MQTT.subscribe(TOPICO_SUBSCRIBE_LED);
		}
		else
		{
			Serial.println("Falha ao reconectar no broker.");
			Serial.println("Havera nova tentatica de conexao em 2s");
			delay(2000);
		}
	}
}

void VerificaConexoesWiFIEMQTT(void)
{
	if (!MQTT.connected())	
	{ 
		reconnectMQTT(); 
	}
	reconnectWiFi();
}

void reconnectWiFi(void)
{
	if (WiFi.status() == WL_CONNECTED)	
	{ 
		return;
	}
	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(100);
		Serial.print(".");
	}
	Serial.println();
	Serial.print("Conectado com sucesso na rede ");
	Serial.print(SSID);
	Serial.println("\nIP obtido: ");
	Serial.println(WiFi.localIP());
}

void setup() 
{
	Serial.begin(9600);
	delay(1000);
	Serial.println("Disciplina IoT: acesso a nuvem via ESP32");
	delay(1000);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LOW);
	randomSeed(analogRead(0));
	initWiFi();
	initMQTT();
}

void loop() 
{
	char temperatura_str[10] = {0};
	VerificaConexoesWiFIEMQTT();
	numAleatorio = random(10, 101);
	sprintf(temperatura_str, "%dC", numAleatorio);
	MQTT.publish(TOPICO_PUBLISH_TEMPERATURA, temperatura_str);
	Serial.print("Gerando temperatura aleatoria: ");
	Serial.println(temperatura_str);
	MQTT.loop();
	delay(2000);
}
