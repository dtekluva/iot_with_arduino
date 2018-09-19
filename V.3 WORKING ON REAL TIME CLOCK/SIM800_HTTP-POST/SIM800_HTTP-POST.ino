//For more Information visit www.aeq-web.com?ref=arduinoide

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>

SoftwareSerial mySerial(10, 11);         // RX, TX Pins
String apn = "web.gprs.mtnnigeria.net"; //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                    //APN-Password
String url = "http://halogenapp.pythonanywhere.com/iot";  //URL for HTTP-POST-REQUEST
String json;   //String for the json Paramter (e.g. jason)
int CO_Value = 0; //CO MQ7 sensor value holder MQ7
int CH4_Value = 0; //H2 MQ5 sensor value holder
int AQ_Value = 0; // MQ135 AIR QUALITY sensor value holder
int H_Value = 0; // MQ8 HYDROGEN sensor value holder
int avg_CO; //AVG VALUES HOLDER
int avg_CH4;
int avg_AQ;
int avg_H;
int count = 0;
int relay_pin = 8; //  Define relay trigger pin to high (pin 8)



void setup() {
  // Initialise relay trigger pin as output
  pinMode(relay_pin, OUTPUT); 
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(10000);
}

void loop() { // run over and over

//    json = {\"location_id\": "+ data1 + ", \"fill_percent\":" + data2 "};
//    json = "{\"location_id\": 238, \"fill_percent\": 90}";
    json = "{\"co\":" +  String(avg_CO) + ",\"ch4\":" +  String(avg_CH4) + ",\"aq\":" +  String(avg_AQ) + ",\"h\":" +  String(avg_H) +"}";

      
        getSensorData();
//get average sensor val for the minute

//    if (count == 270){ //270/60secs equals 4mins and 30secs. 30secs extra is given for the sending of the post request to makke a total of 5min interval per data post
    if (count == 60){
        avg_CO = CO_Value / count ;
        avg_CH4 = CH4_Value / count ;
        avg_AQ = AQ_Value / count ;
        avg_H = H_Value / count ;

        Serial.print(CO_Value);
        Serial.print(CH4_Value);
        Serial.print(AQ_Value);
        Serial.print(H_Value);
        
      Serial.println("Turning off sensor Heaters ....." );
      delay(200); //200milisecs
      Serial.println("SENDING AVERAGE READINGS TO SERVER " );

      digitalWrite(relay_pin, HIGH);
      gsm_sendhttp(); 

      //Start the GSM-Modul and start the transmisson

      Serial.print("CO =  ");
      Serial.print(avg_CO, DEC);               // prints the value read CO
      Serial.print(" PPM  ");
    
      Serial.print("METHANE =  ");
      Serial.print(avg_CH4, DEC);               // prints the value read CH4
      Serial.print(" PPM   ");
    
      Serial.print("AQ =  ");
      Serial.print(avg_AQ, DEC);               // prints the value read AIR QUALITY
      Serial.print(" PPM   ");
    
      Serial.print("HYDROGEN =  ");
      Serial.print(avg_H, DEC);               // prints the value read HYDROGEN
      Serial.println(" PPM");
      
      reset_sensor_values();
    };
   
    delay(1000); //Wait 6 minute
}


void gsm_sendhttp() {

 
  mySerial.println("AT");
  runsl();//Print GSM Status an the Serial Output;
  delay(4000);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
//  delay(100);
//  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); //Comment out, if you need username
//  runsl();
//  delay(100);
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
  mySerial.println("AT+CTZU=1");
  runsl();
//  mySerial.println("AT+CTZU=1");
//  runsl();
//  mySerial.println("AT+CCLK?");
//  runsl();
}

//Print GSM Status
void runsl() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

}

void getSensorData(){
  
  CO_Value = CO_Value + analogRead(0);
  CH4_Value = CH4_Value + analogRead(1);
  AQ_Value = AQ_Value + analogRead(2);
  H_Value = H_Value + analogRead(3);
  
  count = count + 1;
}

void reset_sensor_values(){
  CO_Value = 0;
  CH4_Value = 0;
  AQ_Value = 0;
  H_Value = 0;
  count = 0;
  
  Serial.println("Turning sensor Heaters back ON....." );
  digitalWrite(relay_pin, LOW);
}
