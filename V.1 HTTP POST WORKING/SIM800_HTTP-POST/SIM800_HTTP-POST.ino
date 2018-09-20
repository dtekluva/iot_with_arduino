//For more Information visit www.aeq-web.com?ref=arduinoide

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>

SoftwareSerial mySerial(10, 11);            // RX, TX Pins
String apn = "web.gprs.mtnnigeria.net";                       //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                     //APN-Password
String url = "http://halogenapp.pythonanywhere.com/iot";  //URL for HTTP-POST-REQUEST
String data1;   //String for the first Paramter (e.g. Sensor1)
String data2;   //String for the second Paramter (e.g. Sensor2)
String json;   //String for the json Paramter (e.g. jason)
int CO_Value; //CO MQ7 sensor value holder MQ7
int CH4_Value; //H2 MQ5 sensor value holder
int AQ_Value; // MQ135 AIR QUALITY sensor value holder
int H_Value; // MQ8 HYDROGEN sensor value holder
int count = 0;
int average_airquality = 0;
int sense_sum = 0;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(10000);
}

void loop() { // run over and over

  
    data1 = "123";
    data2 = "ABC";
//    json = {\"location_id\": "+ data1 + ", \"fill_percent\":" + data2 "};
//    json = "{\"location_id\": 238, \"fill_percent\": 90}";
    json = "{\"sensor\":" +  String(average_airquality) + "}";

    sense_sum = sense_sum  + getSensorData(); //get average sensor val for the minute

    
    if (count == 60){
      average_airquality = sense_sum/count;
      Serial.print("SENDING AVERAGE CO READING TO SERVER : " );
      Serial.print(average_airquality);
      Serial.println("PPM" );
      gsm_sendhttp(average_airquality); //Start the GSM-Modul and start the transmisson
      count = 0;
      sense_sum = 0;
    };
   
    delay(1000); //Wait one minute

}


void gsm_sendhttp(int value) {
  
  
 
  mySerial.println("AT");
  runsl();//Print GSM Status an the Serial Output;
  delay(4000);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
  delay(100);
//  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); //Comment out, if you need username
//  runsl();
  delay(100);
//  mySerial.println("AT+SAPBR=3,1,PWD," + apn_p); //Comment out, if you need password
//  runsl();
  delay(100);
  mySerial.println("AT+SAPBR =1,1");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=2,1");
  runsl();
  delay(2000);
  mySerial.println("AT+HTTPINIT");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CID,1");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=URL," + url);
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CONTENT,application/json");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPDATA=192,10000");
  runsl();
  delay(100);
//  mySerial.println("params=" + data1 + "~" + data2);
  mySerial.println(json);
  runsl();
  delay(10000);
  mySerial.println("AT+HTTPACTION=1");
  runsl();
  delay(5000);
  mySerial.println("AT+HTTPREAD");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPTERM");
  runsl(); 
}

//Print GSM Status
void runsl() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

}

int getSensorData(){
  
  static int value_array [4]; // pointer to return array since C cannot reeturn array
  CO_Value = value_array[0] = analogRead(0);
  CH4_Value = value_array[1] = analogRead(1);
  AQ_Value = value_array[2] = analogRead(2);
  H_Value = value_array[3] = analogRead(3);
  
  Serial.print("CO =  ");
  Serial.print(CO_Value, DEC);               // prints the value read CO
  Serial.print(" PPM  ");

  Serial.print("METHANE =  ");
  Serial.print(CH4_Value, DEC);               // prints the value read CH4
  Serial.print(" PPM   ");

  Serial.print("AQ =  ");
  Serial.print(AQ_Value, DEC);               // prints the value read AIR QUALITY
  Serial.print(" PPM   ");

  Serial.print("METHANE =  ");
  Serial.print(H_Value, DEC);               // prints the value read HYDROGEN
  Serial.println(" PPM");
  
  count = count + 1;
  return value_array ;
}
