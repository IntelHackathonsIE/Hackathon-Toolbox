#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

///////////////////////////////////////////////////////
// WiFi Configuration 
///////////////////////////////////////////////////////
char ssid[] = "Guest_DCU_Innovation"; //  your network SSID (name) 
char pass[] = "innovate5443";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

///////////////////////////////////////////////////////
// CHANGE THE FOLLOWING VALUES FOR YOUR BOARD- NOTE That you should use eth0 MAC address for ID.
///////////////////////////////////////////////////////
 String clientName = String("d:quickstart:tmp-gal:984fee01a558");
 // Note on case sensitve tag - it is not just a number.....
 // String clientName = String("d:quickstart:tmp-gal:0CD292D3059F"); // WiFi MAC address 
 //98:4F:EE:01:A5:58
///////////////////////////////////////////////////////
// DO NOT NEED TO CHANGE ANYTHING BELOW HERE
///////////////////////////////////////////////////////
 
 char servername[]="quickstart.messaging.internetofthings.ibmcloud.com";
 String topicName = String("iot-2/evt/status/fmt/json");
 // char username[]="use-token-auth";
 // char password[]="O8O!Jnz)3BxBl?m7s*";
 
 WiFiClient wifiClient;
 PubSubClient client(servername, 1883, 0, wifiClient);
 
 float temperature;
 int sensorPin = A0;    // select the input pin for the potentiometer
 int DC_Motor_Plus = 2;
 int DC_Motor_Minus = 3;  
 int switchin = 7;
 int MotorOn; 

 void setup()
 {
    // Connect to the WiFi network 
    Serial.begin(9600);
  
    delay ( 5000 );
    Serial.println ( "Starting...." );
  
    // Setup our I/O 
  
    pinMode ( DC_Motor_Plus, OUTPUT );
    pinMode ( DC_Motor_Minus, OUTPUT ); 
    pinMode ( switchin, INPUT ); 
    
    digitalWrite ( DC_Motor_Plus, LOW );
    digitalWrite ( DC_Motor_Minus, LOW );
    
    MotorOn = 0;

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present"); 
      // don't continue:
     while(true);
    } 

    String fv = WiFi.firmwareVersion();
    if( fv != "1.1.0" )
      Serial.println("Please upgrade the firmware");
  
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) { 
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    } 
    Serial.println("Connected to wifi");
    printWifiStatus();
 }
 
 void loop()
 {
  char clientStr[34];
  clientName.toCharArray(clientStr,34);
  char topicStr[26];
  topicName.toCharArray(topicStr,26);
  getData();
 
  if ( digitalRead( switchin ) == HIGH ) 
  { 
    if (!client.connected()) {
      Serial.print("Trying to connect to: ");
      Serial.println(clientStr);
      client.connect(clientStr);
    }
    if (client.connected() ) {
      String json = buildJson();
      char jsonStr[200];
      json.toCharArray(jsonStr,200);
      boolean pubresult = client.publish(topicStr,jsonStr);
      Serial.print("attempt to send ");
      Serial.println(jsonStr);
      Serial.print("to ");
      Serial.println(topicStr);
      if (pubresult)
        Serial.println("successfully sent");
      else
        Serial.println("unsuccessfully sent");
    }
    delay(5000);
  }
  else // power on the motor 
  {
    if ( MotorOn == 1 )
    {
      digitalWrite(DC_Motor_Plus, LOW);  // make pin 3 high
      digitalWrite(DC_Motor_Minus, LOW);  // make pin 2 low
      delay ( 1500 ); 
      MotorOn = 0;
    }
    else 
    {
      digitalWrite(DC_Motor_Plus, HIGH);  // make pin 3 high
      digitalWrite(DC_Motor_Minus, LOW);  // make pin 2 low
      delay ( 1500 ); 
      MotorOn = 1;
    }
  }
}

String buildJson() {
  String data = "{";
  data+="\n";
  data+= "\"d\": {";
  data+="\n";
  data+="\"temp\": ";
  data+=(int)temperature;
  data+="\n";
  data+="}";
  data+="\n";
  data+="}";
  return data;
}
 
void getData() {
  int sensorValue = analogRead(sensorPin); 
  float resistance=(float)(1023-sensorValue)*10000/sensorValue; //get the resistance of the sensor;
  temperature= sensorValue; //1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;  
  Serial.print("Sensor value = ");
  Serial.println(temperature, 3);
} 

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
