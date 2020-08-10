
//LIBRARIES

#include <SPI.h>
#include <Pixy.h>

//PINS

#define ctrl_pin 3
#define left_minus_pin 4
#define right_minus_pin 7
#define left_override 5
#define right_override 6

#define trigger_pin 9
#define echo_pin 8

#define wheelEncoder 2

//VARIABLES

Pixy camera; //global variable to manage pixy connection.

int distance;
long actual_time =0;
bool obstacle = false;
bool stopped = true;

volatile int wheelBar=0;
volatile float travelled_dist = 0.0;


void setup() {
  
  pinMode(ctrl_pin,OUTPUT);
  analogWrite(ctrl_pin,0);
  pinMode (left_minus_pin, OUTPUT);
  digitalWrite( left_minus_pin, LOW );
  pinMode (right_minus_pin, OUTPUT);
  digitalWrite( right_minus_pin, LOW );
  pinMode (left_override, OUTPUT);
  digitalWrite(left_override, LOW);
  pinMode (right_override, OUTPUT);
  digitalWrite(right_override, LOW);
  
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  
  pinMode(wheelEncoder, INPUT);

  attachInterrupt(digitalPinToInterrupt(wheelEncoder), wheelDistance, CHANGE);
  //Interrupt function calls the distance function whenever there is a change in the wheelencoder (when it goes from detecting the wheel bar to not detecting it, or viceversa)

  camera.init();
  
//Initialize serial communications at 9600 bps, using channel C and PAN ID 3008.
  
  Serial.begin(9600);  
  Serial.print("+++");
  delay(1500);
  Serial.println("ATID 3008, CH C, CN\n");
  delay(1100);
  
 //This clears up the data sent during the setup process.
 while(Serial.read() != -1) {};
}

void loop() {
  
  actual_time = millis();

  //each 0.5 secs check if there's an obstacle.
  
  if(actual_time%500 == 0){
    obstacleDetection();
  }

  //If the number of wheel bars "covered" by the wheelencoder is 10 means a full rotation of the wheel (it's 10 and not 5 because it counts twice each bar).
  
  if(wheelBar%10==0){
    travelled_dist = (wheelBar/10)*0.16;
  }

  //each 2 seconds send to the PC the distance covered by the buggy.
  
  if(actual_time%2000==0){
    Serial.print(" BUGGY HAS COVERED (m): ");
    Serial.print(travelled_dist);
    Serial.print('\n');
    }

  //Pixy camera comes in play here:
    
  static int j = 0;
  j ++;
  int k;
  uint16_t blocks; //Alias for unsigned integer
  
  if (j % 50 == 0){//check the pixy each 50 iterations

  //Grabbing blocks...
  
  blocks = camera.getBlocks(10);
  
  if (blocks>0){
    for(int i = 0; i<blocks; i++){
      
      //The x location of the center of the detected object(0-319).
      
      k = camera.blocks[i].x; 

      //If the first colour code signature(10) is spotted in the center of the camera(x values of pixels ranging from 120 to 200)-->Left override.
      
      if(camera.blocks[i].signature == 10 && k > 120 && k < 200){
        
          digitalWrite(left_override, HIGH);
          Serial.print("LEFT EYE OVERRIDE \n");
          delay(1000);
        }
        
      //If the second colour code signature(28) is spotted in the center of the camera(x values of pixels ranging from 120 to 200)-->Right override.
      
      else if (camera.blocks[i].signature == 28 && k > 120 && k < 200){
        
          digitalWrite(right_override, HIGH);
          Serial.print("RIGHT EYE OVERRIDE \n");
          delay(1000);
        }

      else {
        digitalWrite(right_override, LOW);
        digitalWrite(left_override, LOW);
        }
      }
    }
  }
  
  else{
    digitalWrite(right_override, LOW);
    digitalWrite(left_override, LOW);
    }
  }

//DEFINING THE FUNCTIONS TO MOVE AND STOP THE BUGGY.

void goBuggy(){
 analogWrite( ctrl_pin, 150 );
 obstacle = false;
 stopped = false;
}

void stopBuggy(){
 analogWrite( ctrl_pin, 0 );
 stopped = true;
}

void continueBuggy(){
 if (obstacle == false){
  goBuggy();
  }
}

//wheelDistance function is created to use it in the interrupt function and loop.

void wheelDistance(){
  wheelBar++;
}  

//SerialEvent occurs when there's new data available.

void serialEvent (){
  while (Serial.available()){
    char letter = Serial.read();
    Serial.println(letter);
    if(letter == 'g'){
      goBuggy();
    }
    if(letter =='s'){
        stopBuggy();
    }
  }
}

//Definition of the obstacle detection function.

boolean obstacleDetection(){

//Send for 10ms ultrasonic waves (LOW, then HIGH, then LOW to avoid bugs)
    
  long duration;
  digitalWrite (trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite (trigger_pin, LOW);
  duration = pulseIn(echo_pin, HIGH);
  distance = duration/58; //divided by 58 to convert distance to cm's.

  //If there's an object near (15cms or less), stop the buggy and report the distance.
  
  if(distance < 15 && distance > 0 && stopped == false){ 
    obstacle = true;
    stopBuggy();
    Serial.print(distance);
    Serial.print('\n');
  }

  //Continue if there's nothing.
  
  else
  {
    continueBuggy();
   }
}
