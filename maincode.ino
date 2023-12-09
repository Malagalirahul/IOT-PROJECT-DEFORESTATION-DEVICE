#include <Wire.h> 
#include <SoftwareSerial.h>                 //Include WIre library for using I2C 
const int MPU_addr=0x68;         //I2C MPU6050 Address
int16_t axis_X,axis_Y,axis_Z;    
const int flameSensorPin = 5;
int buzzer = 10;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;
int pos = 0;  


// Pin configuration for GSM module
#define GSM_TX 7
#define GSM_RX 8


SoftwareSerial gsmSerial(GSM_TX, GSM_RX);
// Phone number to send SMS
char phoneNumber[] = "+919632747119";  

void setup()
{
    Serial.begin(9600);
  pinMode(flameSensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Wire.begin();                        //Begins I2C communication
  Wire.beginTransmission(MPU_addr);    //Begins Transmission with MPU6050
  Wire.write(0x6B);                    //Puts MPU6050 in Sleep Mode
  Wire.write(0);                       //Puts MPU6050 in power mode 
  Wire.endTransmission(true);          //Ends Trasmissio  
  gsmSerial.begin(9600);

}
void loop()
{
  Wire.beginTransmission(MPU_addr); //Begins I2C transmission 
  Wire.write(0X3B);                 //Start with register 0x3B (ACCEL_XOUT_H)             
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); //Request 14 Registers from MPU6050
  axis_X=Wire.read()<<8|Wire.read(); //Obtain 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
  axis_Y=Wire.read()<<8|Wire.read(); //0x3B (ACCEL_YOUT_H) & 0x3C (ACCEL_YOUT_L)
  axis_Z=Wire.read()<<8|Wire.read(); //0x3B (ACCEL_ZOUT_H) & 0x3C (ACCEL_ZOUT_L)
    int xAng = map(axis_X,minVal,maxVal,-90,90); 
    int yAng = map(axis_Y,minVal,maxVal,-90,90);
    int zAng = map(axis_Z,minVal,maxVal,-90,90);
   x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);     //Formula to calculate x values in degree
     int pos = map(x,0,180,0,180); // As X value is from 0 to 360 deg
     int flameValue = digitalRead(flameSensorPin);
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameValue);
     Serial.println("Angle:");
      Serial.println(x);
       Serial.println(pos );

      if (pos >= 260){
      digitalWrite(buzzer, HIGH);
      sendSMS("Deforestation alert! immediate action required.");
      }
      else{
        digitalWrite(buzzer, LOW);
      }
delay(1500);
if (flameValue == 0){
      digitalWrite(buzzer, HIGH);
      sendSMS("Flame alert! immediate action required.");
      }
      else{
        digitalWrite(buzzer, LOW);
      }
delay(1500);                                                                                                                                                                                                                                                                          
     
}


void sendSMS(const char *message) {
  // Send AT commands to configure GSM module
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set the SMS mode to text
  delay(1000);
  
  // Compose the SMS message
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");

  delay(1000);
  gsmSerial.println(message);

  delay(1000);
  gsmSerial.write(26); // Send Ctrl+Z to indicate the end of the message
}