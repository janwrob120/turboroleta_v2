#include "WiFi.h"
#include "blinds.h"


const char *ssid     = "internety";
const char *password = "fffiiittt";

Blinds blinds;


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
    }
    else
    {
        digitalWrite(2,0);
    }

    blinds.manage();

    
    delay(10);


	
}
