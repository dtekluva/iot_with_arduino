//For more Information visit www.aeq-web.com?ref=arduinoide
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial mySerial(10, 11);         // RX, TX Pins
String apn = "web.gprs.mtnnigeria.net"; //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                    //APN-Password
String url = "http://arduinoiot.pythonanywhere.com/iot";  //URL for HTTP-POST-REQUEST
String json;   //String for the json Paramter (e.g. jason)
String time_stamp = ""; //String for time_stamp collection 
unsigned long   CO_Value = 0; //CO MQ7 sensor value holder MQ7
unsigned long   CH4_Value = 0; //H2 MQ5 sensor value holder
unsigned long   AQ_Value = 0; // MQ135 AIR QUALITY sensor value holder
unsigned long   H_Value = 0; // MQ8 HYDROGEN sensor value holder
unsigned short int avg_CO; //AVG VALUES HOLDER
unsigned short int avg_CH4;
unsigned short int avg_AQ;
unsigned short int avg_H;
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

      getSensorData(); //get average sensor val for the minute
      

      Serial.println(count);
//       if (count == 270){ //270/60secs equals 4mins and 30secs. 30secs extra is given for the sending of the post request to makke a total of 5min interval per data post
      if (count == 60){
                avg_CO = CO_Value / count ;
                avg_CH4 = CH4_Value / count ;
                avg_AQ = AQ_Value / count ;
                avg_H = H_Value / count ;
                
                Serial.println("Turning off sensor Heaters ....." );
                delay(200); //200milisecs
                Serial.println("SENDING AVERAGE READINGS TO SERVER " );
          
                digitalWrite(relay_pin, HIGH);
                
//                Serial.println(time_stamp);
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
                Serial.println(json);
              };
   
    delay(1000); //Wait 6 minute
}


void gsm_sendhttp() {

  mySerial.println("AT");
  runsl();//Print GSM Status an the Serial Output;
  delay(4000);
  mySerial.println("AT+CCLK?"); //CHECK STATUS OF RTC ONSTARTUP SYNCING
  runsl();
  delay(100);
  time_stamp = mySerial.readString();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
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
  to_json_();
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
  delay(100); 
}

//Print GSM Status
void runsl() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

}

void getSensorData(){
  int co_val = analogRead(0);
  int ch4_val =  analogRead(1);
  int aq_val =  analogRead(2);
  int h_val =  analogRead(3);
  
  CO_Value = CO_Value + co_val;
  CH4_Value = CH4_Value + ch4_val;
  AQ_Value = AQ_Value + aq_val;
  H_Value = H_Value + h_val;
  
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


String set_time_stamp() {
    mySerial.println("AT+CCLK?");//GET TIME
    runsl();
    delay(100);
//
//    time_stamp = mySerial.readString();
//    delay(500);
//    time_stamp.replace("OK", "");
//    Serial.println(time_stamp);
//    Serial.println(json);
     return 0;
};


void to_json_(){//serialize and prepare json data for sending
 StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  time_stamp.replace("\n", "");
  time_stamp.replace("\r", "");
  time_stamp.replace("AT+CCLK?", "");
  time_stamp.replace("+CCLK:", "");
  time_stamp.replace("OK", "");
  root["co"] = avg_CO;
  root["ch4"] = avg_CH4;
  root["aq"] = avg_AQ;
  root["h"] = avg_H;
  root["time"] = time_stamp;
  String place_holder;
  root.printTo(place_holder);
  json = place_holder;
};



// **COMMENTS , READ HERE**


//  mySerial.println("AT+CCLK?"); //CHECK STATUS OF RTC ONSTARTUP SYNCING
//  runsl();
//  delay(100);
//  mySerial.println("AT+CLTS=1");  //ENABLE REAL TIME CLOCK SYNCHRONIZATION AND SET TIME 
//  runsl();
//  delay(100);
//  mySerial.println("AT&W"); //SAVE NEW SETTINGS SO THAT SIM800L MODULE SETS TIME ON STARTUP SUBSEQUENTLY
//  runsl();
//  delay(100);


  //get realtime VALUE from network
//  mySerial.println("AT+CLTS=1");
//  runsl();
//  delay(100);
//  mySerial.println("AT+CLTS?");
//  runsl();
//  delay(100);
//  mySerial.println("AT&W");
//  runsl();
//  delay(100);
//  mySerial.println("AT+CCLK?");
//  runsl();
//  delay(100);

//  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); //Comment out, if you need username
//  runsl();
//  delay(100);
//  mySerial.println("AT+SAPBR=3,1,PWD," + apn_p); //Comment out, if you need password
//  runsl();

//  Serial.print("CO " + String(CO_Value) + "--" +  String(co_val));
//  Serial.print("  CH4 " + String(CH4_Value) + "--" +  String(ch4_val)); 
//  Serial.print("  AQ " + String(AQ_Value)+ "--" +  String(aq_val)); 
//  Serial.print("  H " + String(H_Value) + "--" +  String(h_val));
//  Serial.println("  COUNT " + String(count));
