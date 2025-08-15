#include "I2Cdev.h"
#include "MPU6050.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


//GPS
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
 
//GSM

static const int RXPin1 = 10, TXPin2 = 11;
SoftwareSerial sim(RXPin1, TXPin2);
#define N 4

String number[]={String("+8801718798034"),String("+8801843644100"),String("+8801751411712"),String("+8801790478408")};
String lati;
String longi;


// mpu6050 
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;


// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO


#define LED_PIN 13

//int data[STORE_SIZE][5]; //array for saving past data
//byte currentIndex=0; //stores current data array index (0-255)

boolean fall = false; //stores if a fall has occurred
boolean fall_r = false; //stores if a fall has recovered
boolean trigger1=false; //stores if first trigger (lower threshold) has occurred
boolean trigger2=false; //stores if second trigger (upper threshold) has occurred
boolean trigger3=false; //stores if third trigger (orientation change) has occurred

byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0; //stores the counts past since trigger 3 was set true

int angleChange=0;


void setup() {
    
    //MPU6050 setup code 
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(9600);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    delay(2000);

    //GSM setup code
    Serial.begin(9600);
    sim.begin(9600);
    delay(1000);
 
    //GPS stup cod(should be at the end of all setups definitely!)
    //naile warning ashbe
 
    ss.begin(GPSBaud);
    
}

void loop() {
  // GPS loop
    while (ss.available() > 0)
      if (gps.encode(ss.read()))
        displayInfo();
 
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      while(true);
    }
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);


  #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(AcX); Serial.print("\t");
        Serial.print(AcY); Serial.print("\t");
        Serial.print(AcZ); Serial.print("\t");
        Serial.print(GyX); Serial.print("\t");
        Serial.print(GyY); Serial.print("\t");
        Serial.println(GyZ);
    #endif
  
  //Normalizing
  //2050, 77, 1947 are values for calibration of accelerometer
  // values may be different in practical case
  ax = (AcX-2050)/16384.00;
  ay = (AcY-77)/16384.00;
  az = (AcZ-1947)/16384.00;

  //270, 351, 136 for gyroscope
  gx = (GyX+270)/131.07;
  gy = (GyY-351)/131.07;
  gz = (GyZ+136)/131.07;


  // calculating Amplitute vactor for 3 axis
  float Raw_AM = pow(pow(ax,2)+pow(ay,2)+pow(az,2),0.5);
  int AM = Raw_AM * 10;  // as values are within 0 to 1, I multiplied 
                         // it by for using if else conditions 

 Serial.println(AM);
 //Serial.println(PM);
 //delay(500);
 angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5);
 Serial.println(angleChange);
   if (trigger3==true){
     trigger3count++;
     //Serial.println(trigger3count);
     if (trigger3count>=1){ 
        if ((angleChange>=45)){ //if orientation changes remains between 0-10 degrees
            fall=true; trigger3=false; trigger3count=0;
            Serial.println(angleChange);
              }
        else{ //user regained normal orientation
           trigger3=false; trigger3count=0;
           Serial.println("TRIGGER 3 DEACTIVATED");
        }
      }
   }

  if (sim.available() > 0)
    Serial.write(sim.read());
  
  if (fall_r==true){ //in event of a fall detection
    fall=false;
    if (AM>=9 || angleChange>=40){
      Serial.println("RECOVERED FROM FALL");
    //send sms
      //GSM code loop
      for(int i=0;i<N;i++)
      {
         SendMessageR(i);
         delay(3000);
      }
    }
      fall_r=false;
    delay(1000);
   // exit(1);
 }
    
   
    
  if (fall==true){ //in event of a fall detection
    trigger3=false;
    fall_r=true;
    Serial.println("FALL DETECTED");
    //send sms
    
 
 
    //GSM code loop
    for(int i=0;i<N;i++)
    {
       SendMessage(i);
       delay(3000);
    }
    
   
    if (sim.available() > 0)
      Serial.write(sim.read());
    delay(15000);
    
  }
 /* if (trigger2count>=6){ //allow 0.5s for orientation change
    trigger2=false; trigger2count=0;
    Serial.println("TRIGGER 2 DECACTIVATED");
    }
  */
  if (trigger1count>=6){ //allow 0.5s for AM to break upper threshold
    trigger1=false; trigger1count=0;
    Serial.println("TRIGGER 1 DECACTIVATED");
    }
   
  if (trigger2==true){
    trigger2count++;
    //angleChange=acos(((double)x*(double)bx+(double)y*(double)by+(double)z*(double)bz)/(double)AM/(double)BM);
    angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5); Serial.println(angleChange);
    if (angleChange>=30 && angleChange<=400){ //if orientation changes by between 80-100 degrees
      trigger3=true; trigger2=false; trigger2count=0;
      Serial.println(angleChange);
      Serial.println("TRIGGER 3 ACTIVATED");
        }
    }
  if (trigger1==true){
    trigger1count++;
    if (AM>=20){ //if AM breaks upper threshold (3g)
      trigger2=true;
      Serial.println("TRIGGER 2 ACTIVATED");
      trigger1=false; trigger1count=0;
      }
    }
  if (AM>=12 && trigger2==false){ //if AM breaks lower threshold (0.4g)
    trigger1=true;
    Serial.println("TRIGGER 1 ACTIVATED");
    }
  //It appears that delay is needed in order not to clog the port
    delay(100);
  }

//GPS functions
void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    lati=String(gps.location.lat(),6);
    longi=String(gps.location.lng(),6);
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(lati);
    Serial.print(longi);
  }
 
  Serial.println();
}
 
//GSM functions
void SendMessage(int i)
{
  String num=number[i];
  Serial.println(num);
  delay(1000);
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + num + "\"\r");
  delay(1000);
  String SMS = "Fall detected at http://www.google.com/maps/place/"+lati+","+longi;
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
}

void SendMessageR(int i)
{
  String num=number[i];
  Serial.println(num);
  delay(1000);
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + num + "\"\r");
  delay(1000);
  String SMS = "Recovered from Fall";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
}
