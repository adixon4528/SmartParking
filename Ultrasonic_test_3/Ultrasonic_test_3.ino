
//Define echo pins for each individual ultrasonic sensor
#define echo1 8 
#define echo2 4 
#define echo3 2 

//Define trigger pins 
#define trig1 9 
#define trig2 5 
#define trig3 3   

//Define IR connections 
#define IR1 A0 
#define IR2 A1

//Update frequency
#define UPDATE 1 / 20 * 1000

long D_US1, D_US2, D_US3;					//Distances that the ulrasonic sensors read.
int D_IR1, D_IR2;							//Distances that the IR sensors read.
int US_Count, IR_Count, Total_Count = 0;	//Count of cars
long min_distance;							//The minimum distance that the sensors can read and still 
											//not say that it's a car.
bool flag1, flag2;							//Flags that help determine if both the IR and US sensors
											//detected a car. 
long ground;								//Distance from the sensor to the ground
long sumGround;
int numMeasurements;

//Function that returns the distance between the US sensor and the closest objet, eg the ground or car
long USMeas(int trigPin, int echoPin) {  

	long duration;
	digitalWrite(trigPin, HIGH); 
	delayMicroseconds(2); 

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10); 

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);

	//Calculate the distance (in inches) based on the speed of sound.
	return duration / 58.2 * 0.393701;



} 

//Measuring function for the IR sensor.
int IRMeas( int IRpin ) { 

	int averaging = 0;				//  Holds value to average readings

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

	distance = averaging / 5;		// Average out readings
	return(distance);				// Return value

}



void setup() {
	//Establish a BAUD rate of 9600 BPS
	Serial.begin(9600); 
	//Set dedicated outputs and inputs
	pinMode(trig1, OUTPUT);
	pinMode(echo1, INPUT); 
	pinMode(trig2, OUTPUT); 
	pinMode(echo2, INPUT); 
	pinMode(trig3, OUTPUT); 
	pinMode(echo3, INPUT);

	//Set the distance from the sensor to the ground.
	numMeasurements = 10;
	sumGround = 0.0;
	int i;
	for(i = 0; i < numMeasurements; i++){
		sumGround += USMeas(trig1, echo1);
	}
	ground = sumGround / numMeasurements;
}


void loop() {
	D_US1, D_US2, D_US3, D_IR1, D_IR2 = 0;  
	min_distance = ground - 43;							//Set the minimum distance.
	flag1, flag2 = false;
	//Ensure that the US sensors are triggered in the right order. Only triggering 
	//the sensors in the right order will increment the US car count.
	D_US1 = USMeas( trig1, echo1 ); 
	if(D_US1 < min_distance){
		D_US2 = USMeas( trig2, echo2 ); 
		if(D_US2 < min_distance){
			D_US3 = USMeas( trig3, echo3 ); 
			if(D_US3 < min_distance) {
				flag1 = true;
				US_Count++;
			}
		}
	}

	//As before, we need to ensure that the IR sensors are tripped in the correct order in order to 
	//increment the IR car count.
	D_IR1 = IRMeas( IR1 ); 
	if(D_IR1 < min_distance){
		D_IR2 = IRMeas( IR2 );
		if(D_IR2 < min_distance){
			flag2 = true;
			IR_Count++;
		}
	}

	//If both US and IR car counts have been incremented, increment the total car counts
	if(flag1 && flag2) Total_Count++;

	//Display Formated Measurements on the serial monitor
	Serial.println("Sensor 1 | Sensor 2 | Sensor 3 | IR1 | IR2: ");  
	Serial.print(D_US1);Serial.print(" ");Serial.print(D_US2);Serial.print(" "); 
	Serial.print(D_US3);Serial.print(" ");Serial.print(D_IR1);Serial.print(" "); 
	Serial.print(D_IR2);Serial.print(" "); 
	Serial.println(" ");  

	//Take Measurements Every 1000ms = 1s  
	delay(UPDATE);  


}
