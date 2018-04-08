#include <SPI.h> 
#include <RH_NRF24.h> 

RH_NRF24 nrf24; 


//Define echo pins for each individual ultrasonic sensor
#define echo1 7 
#define echo2 4


//Define trigger pins 
#define trig1 6
#define trig2 9 
  

//Define IR connections 
#define IR1 A0

//Update frequency
#define UPDATE 1 / 1 * 1000

long D_US1, D_US2, D_US3;          //Distances that the ulrasonic sensors read.
int D_IR1, D_IR2;             //Distances that the IR sensors read.
int US_Count, IR_Count, Total_Count = 0;  //Count of cars
long min_distance;              //The minimum distance that the sensors can read and still 
                      //not say that it's a car.
bool flag1, flag2;              //Flags that help determine if both the IR and US sensors
                      //detected a car. 
long ground, groundIR;                //Distance from the sensor to the ground
long sumGround, IRAvg;
int numMeasurements;

//Function that returns the distance between the US sensor and the closest objet, eg the ground or car
long USMeas(int trigPin, int echoPin) {  

  long duration;
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(50); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(50); 

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in inches) based on the speed of sound.
  return duration / 58.2 * 0.393701; 



} 

//Measuring function for the IR sensor.
int IRMeas( int IRpin ) { 

  int averaging = 0;        //  Holds value to average readings

  int distance = 0; 
  // Get a sampling of 5 readings from sensor
  for (int i=0; i<5; i++) {
    distance = analogRead( IRpin );
    averaging = averaging + distance;
    delay(55);      // Wait 55 ms between each read
    // According to datasheet time between each read
    //  is -38ms +/- 10ms. Waiting 55 ms assures each
    //  read is from a different sample
  }

  distance = averaging / 5;   // Average out readings
  return(distance);       // Return value 

  


}



void setup() {
  //Establish a BAUD rate of 9600 BPS
  Serial.begin(9600); 
  //Set dedicated outputs and inputs
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT); 
  pinMode(trig2, OUTPUT); 
  pinMode(echo2, INPUT); 


  //Set the distance from the sensor to the ground.
  numMeasurements = 10;
  sumGround = 0.0; 
  IRAvg = 0;
  int i;
  for(i = 0; i < numMeasurements; i++){
    sumGround += USMeas(trig1, echo1); 
    IRAvg += IRMeas(IR1); 
    delay(100);
  }
  ground = sumGround / numMeasurements; 
  groundIR = IRAvg / numMeasurements;  
  Serial.println("Ground Measurement: "); Serial.print(ground); Serial.println(); 
  Serial.println("IR Ground Measurement: "); Serial.print(groundIR); Serial.println(); 

  
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    

  
}


void loop() { 
  
  D_US1, D_US2, D_US3, D_IR1, D_IR2 = 0;  
  min_distance = ground - 43;             //Set the minimum distance.
  groundIR = groundIR - 10; //Still working with uncoverted data. This is measuring voltage
  //Ensure that the US sensors are triggered in the right order. Only triggering 
  //the sensors in the right order will increment the US car count.
  D_US1 = USMeas( trig1, echo1 ); delay(50);
  D_US2 = USMeas( trig2, echo2 ); delay(50);
  if(D_US1 < min_distance && D_US2 < min_distance) {
        flag1 = true;
  }
    
  

  //As before, we need to ensure that the IR sensors are tripped in the correct order in order to 
  //increment the IR car count.
  D_IR1 = IRMeas( IR1 ); 
  if(D_IR1 < groundIR){
      flag2 = true;
  } 
     
  
  //If both US and IR car counts have been incremented, increment the total car counts
  if( flag1 == true ){  
     
     uint8_t data[] = "True";
     nrf24.send(data, sizeof(data));  
     nrf24.waitPacketSent();
     // Now wait for a reply
     uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
     uint8_t len = sizeof(buf);
     Total_Count++;  
     
     } 

//Display Formated Measurements on the serial monitor
  Serial.println("Sensor 1 | Sensor 2 | IR1: ");  
  Serial.print(D_US1);Serial.print(" ");Serial.print(D_US2);Serial.print(" "); 
  Serial.print(D_IR1);Serial.print(" ");  
 
  Serial.println("Cars Passed:  "); Serial.print(Total_Count); 
  Serial.println();   
  flag1 = false; 
  flag2 = false; 

  
  
 
  
  //Take Measurements Every 1000ms = 1s  
  delay(UPDATE);   

  


}
