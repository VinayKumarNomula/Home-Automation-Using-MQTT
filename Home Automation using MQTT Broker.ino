
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<Servo.h>

Servo myservo;

#define led1 D0  // BLUE LED  PIR
#define led2 D1  // RED LED  GAS
#define led3 D5  // RED LED  FLAME
#define led4 D4  // BLUE LED  PIR

#include "DHT.h"  //U need to add DHT sensor library 
#define DHTPIN D2   // what pin we're connected to 
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);

#define gas_sensor A0
#define flame_sensor D3
#define pir_sensor D8

//servo D7

void reconnect();
void MQpublish();
String cmd;

// Update these with values suitable for your network.

const char* ssid = "vinnu";
const char* password = "9059045255";
const char* mqtt_server = "broker.mqtt-dashboard.com";


WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{
  Serial.begin(115200);
  dht.begin(); 
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 myservo.attach(D7);
 pinMode(D7,OUTPUT);
 pinMode(led1,OUTPUT);
 pinMode(led2,OUTPUT);
 pinMode(led4,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(gas_sensor,INPUT);
  pinMode(flame_sensor,INPUT);
   pinMode(pir_sensor,INPUT);
  
}

void loop() {

  delay(100);
  if (!client.connected())
  {
   reconnect();
  }
  
  client.loop();
  cmd ="";

MQpublish();
delay(2000); 
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  while (WiFi.status()!= WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{ 
  Serial.print("Message arrived[");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    //Serial.print((char)payload[i]);
    cmd = cmd + String((char)payload[i]);
  }
  Serial.println(cmd);
  

  // Switch on the LED if an 1 was received as first character
  if (cmd == "dooropen")
  {
    delay(20);
    myservo.write(180);
      // Turn the LED on (Note that LOW is the voltage level
    Serial.println("gate is open");
    client.publish("status1", "gate is open");
     delay(2000);
    } 
  else if (cmd == "doorclose")
  {
    delay(500);
    myservo.write(0);
    client.publish("status1", "gate is closed");
     delay(2000);
  }

   else if (cmd == "lighton")
  {
    digitalWrite(led4,HIGH);
    client.publish("status1", "hall light is on");
     delay(2000);
  }
   else if (cmd == "lightoff")
  {
    digitalWrite(led4,LOW);
    client.publish("status1", "hall light is off");
     delay(2000);
  }
  else {
    Serial.println("Waiting for Command");
  }
 
  cmd ="";

}
void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espClient")) {
      Serial.println("connected");
      client.subscribe("smarthome");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




void MQpublish()
{
/////////////////////////////////////   LED2
char str[20];
int gas_value =analogRead(A0);
sprintf(str,"%ld",gas_value);
 Serial.print("gas value : ");
 Serial.println(str);
  if(gas_value >300)
  {
     digitalWrite(led2,HIGH);
 //   client.publish("gas1","Alert!! combustible gas detected");
  //  delay(2000);
    client.publish("gas1",str); 
     
  }
  else
  {
     digitalWrite(led2,LOW);
 //   client.publish("gas1","All is Good");
 //    delay(2000);
    client.publish("gas1",str);
  }

///////////////////////////////////////////     LED1
  int pir1status= digitalRead(pir_sensor);
 char str1[20];
sprintf(str1,"%d",pir1status);
  Serial.print("pir1status : ");
   Serial.println(str1);
   if (pir1status==1)
  {
    digitalWrite(led1,HIGH);
    Serial.println("Motion detected");
    client.publish("motion1",str1); 
    }
    
  else 
  {
    digitalWrite(led1,LOW);
    Serial.println("No motion detected"); 
     client.publish("motion1",str1); 
  } 
////////////////////////////////////////////   LED3
char str2[20];
int flame = digitalRead(flame_sensor);
sprintf(str2,"%d",flame);
Serial.print("flame : ");
Serial.println(str2);
   if (flame==0)
  {
    digitalWrite(led3,HIGH);
    Serial.println("flame detected");
    client.publish("fire1",str2); 
    }
    
  else 
  {
    digitalWrite(led3,LOW);
    Serial.println("flame not detected");
    client.publish("fire1",str2);
  } 
/////////////////////////////////////// dht 11
char str4[20];
char str5[20];
  int h = dht.readHumidity();
  int t = dht.readTemperature();
    
  Serial.print("temperature:");
  Serial.println(t);
  Serial.print("Humidity:");
  Serial.println(h);
  
  sprintf(str4,"%ld",t);
  sprintf(str5,"%ld",h);
    client.publish("temp1",str4); 
    delay(2000);
     client.publish("hum1",str5); 
 
}


