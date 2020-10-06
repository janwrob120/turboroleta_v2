#include "WiFi.h"
#include "blinds.h"
#include <PubSubClient.h>
#include "DHT.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#define DHTpin 5
#define LAMPpin 27
#define DEKSTOPpin 35
#define LIGHTpin 34

const char *ssid     = "internety";
const char *password = "fffiiittt";
const char *broker = "192.168.1.110";
const char *topic = "room_Jan_synchronized";


WiFiUDP ntpUDP;
NTPClient *timeClient = new NTPClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000); //3600 czas zimowy; 7200 czas letni
WiFiClient client; 
PubSubClient mqtt(client);
long lastMsg = 0;
char msg[50];
int value = 0;
bool MQTTstat=0;
unsigned long prev_timer=0;
unsigned long timer=0;
String NTP_time;
float light=10,temp=20;
bool lamp=0, desktop =0, blinds_opened=0;

DHT dht(DHTpin, DHT22);
Blinds blinds;

boolean mqttConnect()
{
    Serial.print("Connecting to ");
    Serial.print(broker);

    // Connect to MQTT Broker
    boolean status = mqtt.connect("esp32_room1");

    if (status == false)
    {
        Serial.println(" fail");
        return false;
    }
    
    mqtt.setCallback(receive_msg);
    mqtt.subscribe("blinds_control");
    
    Serial.println(" success");
    return mqtt.connected();
}


void setup()
{
	Serial.begin(115200);
    pinMode(DHTpin, INPUT);
    dht.begin();
    pinMode(2,OUTPUT);
    pinMode(LAMPpin,INPUT_PULLUP);
    pinMode(LIGHTpin,INPUT);
    pinMode(DEKSTOPpin,INPUT_PULLUP);

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

    timeClient->begin();
    mqtt.setServer(broker, 1885);
    MQTTstat = mqttConnect();
    

    
}

void loop()
{
    timeClient->update();
    NTP_time=timeClient->getFormattedTime();
    blinds.current_time = NTP_time.substring(0,5);//cutting out only hh:mm 


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

        timeClient->begin();
        mqtt.setServer(broker, 1885);
        mqttConnect();
    }
    else
    {
        digitalWrite(2,0);

        timer=millis();
        if((timer-prev_timer>=5000)&&((timeClient->getSeconds()==0)||(timeClient->getSeconds()==20)||(timeClient->getSeconds()==40)))
        {
            light = 1-analogRead(LIGHTpin)/4095.0;
            temp = dht.readTemperature();
            desktop=digitalRead(DEKSTOPpin);
            lamp = digitalRead(LAMPpin);
            blinds_opened = !digitalRead(EDGE_SWITCH_DOWN);

            prev_timer=timer;
            if (!mqtt.connected()) mqttConnect();
            else 
            {
                Serial.println("Sending msg");
                String data = "sensor_light "+String(light)+
                " sensor_temperature "+String(temp)+
                " sensor_desktop "+String(desktop)+
                " device_blinds "+String(blinds_opened)+
                " device_lamp "+ String(lamp);
                mqtt.publish(topic, data.c_str());
            }

            Serial.print("light: ");
            Serial.println(light);
            Serial.print("temp: ");
            Serial.println(temp);
            Serial.print("desktop: ");
            Serial.println(desktop);
            Serial.print("lamp: ");
            Serial.println(lamp);
            Serial.print("blinds: ");
            Serial.println(blinds_opened);

        }
    }



    blinds.manage();
	mqtt.loop();
    delay(200);
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
