/*
 * A simple example of how to subscribe and pub in a topic in aws iot
 * 
 * Based on: https://exploreembedded.com/wiki/AWS_IOT_with_Arduino_ESP32
 * FireBeetle: https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478
 * aws-root-ca: https://www.amazontrust.com/repository/AmazonRootCA1.pem
 * 
 * Remember to add your own values in secrets.h
 */

#include <AWS_IOT.h>
#include <WiFi.h>
#include "secrets.h"

AWS_IOT hornbill;

int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}



void setup() {
    Serial.begin(115200);
    delay(2000);

    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 5 seconds for connection:
        delay(5000);
    }

    Serial.println("Connected to wifi");

    if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);

        if(0==hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }

    delay(2000);

}

void loop() {

    if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);
    }
    if(tick >= 5)   // publish to topic every 5seconds
    {
        tick=0;
        sprintf(payload,"{ \"desired\": { \"name\": 1 } }",msgCount++);
        if(hornbill.publish(TOPIC_NAME,payload) == 0)
        {        
            Serial.print("Publish Message:");
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
    }  
    vTaskDelay(1000 / portTICK_RATE_MS); 
    tick++;
}
