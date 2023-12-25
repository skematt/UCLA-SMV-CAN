#include <HardwareSerial.h>
#include <String>
#include "smv_utils.h"
#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "FiOS-XKWHX";
const char pass[] = "maps4932kin3727bbq";

WiFiClient net;
MQTTClient client;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESP32", "smv", "E07Y7LwX5z")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

String getTopic(int first, int last)
{
  String output = "/";
  output += readHardware(first) + "/";
  output += readDataType(first, last);
  return output;
}

HardwareSerial SerialPort(2); // use UART2
//giving the cast an initial value could cause bad data only for the first packet
DoubleCaster c = {0};
int first = 0;
int last = 0;
void setup()
{
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, 16, 17);
  WiFi.begin(ssid, pass);
  client.begin("smv.seas.ucla.edu", net);
}
void loop()
{
  client.loop();
  //delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  bool write_flag = false;
  if (SerialPort.available())
  {
    write_flag = true;
    for(int i = 0; i < 8; i++)
    {
      c.arr[i] = SerialPort.read();
    }
    first = (int)SerialPort.read();
    last = (int)SerialPort.read();
  }
  if (write_flag)
  {
    Serial.println((String)c.num);
    Serial.println(first);
    Serial.println(last);
    client.publish(getTopic(first, last), (String)c.num);
    Serial.println("First is: " + (String)first);
    Serial.println("Last is: " + (String)last);
    Serial.println("The topic is: " + getTopic(first, last));
  }
  
}