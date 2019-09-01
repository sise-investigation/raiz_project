/**
 * A simple example of how to subscribe and pub in a topic in aws iot
 * 
 * 
 * 
 * Based on: https://exploreembedded.com/wiki/AWS_IOT_with_Arduino_ESP32
 * FireBeetle: https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478   
 * The official link is broken (today, agoust 23). So you need to use Preferences = https://dl.espressif.com/dl/package_esp32_index.json   
 * Upload spedd: 115200   
 * aws-root-ca: https://www.amazontrust.com/repository/AmazonRootCA1.pem   
 *    
 * Remember to add your own values in secrets.h
 * 
 * @autor: Kevin Parra
 * @todo: - Sensors and controllers modules.
 *        - Change delay for millis() way.
 */

#include <AWS_IOT.h>
#include <WiFi.h>
#include "secrets.h"

AWS_IOT hornbill;

//Sensor variables
float temp_amb = 0.0;
float humid_amb = 0.0;
float soil_moisture = 0.0;
float light_level = 0.0;

//Setpoint variables
float light_set_point = 0.0;


int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
const int msg_size = 512;
char payload[msg_size]; ///< 512 characters for a message to AWS IOT
char rcvdPayload[msg_size]; ///< 512 characters for a message from AWS IOT

/**
 * Load data to send. It is useful load all data before to send if you
 * want check possible errors.
 */
void loadData() {
  String var1 = String("\"temp_amb\" : \"");
  var1+=temp_amb;
  var1+="\",\n";

  String var2 = String("\"humid_amb\" : \"");
  var2+=humid_amb;
  var2+="\",\n";

  String var3 = String("\"soil_moisture\" : \"");
  var3+=soil_moisture;
  var3+="\",\n";

  String var4 = String("\"light_level\" : \"");
  var4+=light_level;
  var4+="\"\n";
  
  String message = "{\n" + var1 + var2 + var3 + var4 + "}";
  message.toCharArray(payload,msg_size);
}

void recieveData() {
  if (msgReceived == 1) ///< when the subCallBackHandler recieved a message, change de msgRecieved variable to 1
  {
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.println(rcvdPayload);
  }
}

void sendData() {
  if (tick >= 5)  ///< publish to topic every 5seconds
  {
    tick = 0;
    // Dont do nothing: sprintf(payload, "Communication test", msgCount++);
    if (hornbill.publish(TOPIC_NAME, payload) == 0)
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

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void connectToRed() {
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

  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);

    if (0 == hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler))
    {
      Serial.println("Subscribe Successfull");
    }
    else
    {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while (1);
    }
  }
  else
  {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }

  delay(2000);

}

void setup() {
  Serial.begin(115200);
  connectToRed();
}

void loop() {
  loadData();
  sendData();
  Serial.println("---------------------------------");
  delay(2000);
}
