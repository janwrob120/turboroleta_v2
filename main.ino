#include "WiFi.h"
#include "blinds.h"
#include <PubSubClient.h>

const char *ssid     = "internety";
const char *password = "fffiiittt";
const char* broker = "mqtt.eclipse.org";

WiFiClient client;
PubSubClient mqtt(client);
long lastMsg = 0;
char msg[50];
int value = 0;
bool MQTTstat=0;

Blinds blinds;

boolean mqttConnect()
{
    Serial.print("Connecting to ");
    Serial.print(broker);

    // Connect to MQTT Broker
    boolean status = mqtt.connect("esp32");

    if (status == false)
    {
        Serial.println(" fail");
        return false;
    }
    
    mqtt.setCallback(receive_msg);
    mqtt.subscribe("blinnnds");
    
    Serial.println(" success");
    return mqtt.connected();
}


void setup()
{
    pinMode(2,OUTPUT); //onboard led
	Serial.begin(9600);

    WiFi.begin(ssid, password);
    int timeout=0;
    while (( WiFi.status() != WL_CONNECTED )&&(timeout<20) )
    {
        timeout++;
        delay ( 500 );
        Serial.print ( "." );
    }
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    blinds.timeClient->begin();
    mqtt.setServer(broker, 1883);
    MQTTstat = mqttConnect();
    

    
}

void loop()
{
    if (WiFi.status()!=WL_CONNECTED) //in case of lost connection
    {
        digitalWrite(2,1);
        WiFi.begin(ssid, password);

        int timeout=0;
        while (( WiFi.status() != WL_CONNECTED )&&(timeout<20) )
        {
            timeout++;
            delay ( 500 );
            Serial.print ( "." );
        }
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        blinds.timeClient->begin();
        mqtt.setServer(broker, 1883);
        mqttConnect();
    }
    else
    {
        digitalWrite(2,0);
    }

    if(mqtt.connected()) mqttConnect();

    blinds.manage();
	mqtt.loop();
    delay(10);
}


void receive_msg(char *topic, byte *payload, unsigned int length)
{
    if((char)payload[0]=='1')
    {
        blinds.close(1);
    }
    if((char)payload[0]=='0')
    {
        blinds.open(1);
    }
}
