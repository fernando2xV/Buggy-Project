//LIBRARIES.

import processing.serial.*;
import controlP5.*;

//GLOBAL VARIABLES.

Serial port; //Setting up the serial port for data
ControlP5 p5; //p5 will be used for the buttons
Button GoButton, StopButton; //Buttons for the interface

void setup()
{
  size(700,600);
  background(255,0,0);
  printArray( Serial.list() );
  
  String portName = Serial.list()[0]; //number depends on the port.

  port = new Serial(this, portName, 9600);
  port.write("+++");
  delay(1100); // Let the setup hapen
  port.write("ATID 3008, CH C, CN"); // Select ID and channel
  delay(1100); // Again, wait for the device to settle down
  
  port.bufferUntil( 10); // ASCII 10 is a "linefeed", AKA '\n', the thing that comes at the end of a println.
  
  // Set up ControlP5 and create two buttons
  
  p5 = new ControlP5(this);
  
  GoButton = p5.addButton("Start");
  StopButton = p5.addButton("Stop");
 
  GoButton.setPosition(100,100);
  GoButton.setSize(200,40);
  StopButton.setPosition(400,100);
  StopButton.setSize(200,40);
}

//Function that sends data when buttons are pressed.

public void controlEvent( ControlEvent ev ){
  println( ev.getController().getName() ); // Debug
  
  if( ev.isFrom( GoButton ) ){
    port.write("g");
    port.write("\n");
  }
  if( ev.isFrom( StopButton ) ){
    port.write("s");
    port.write("\n");
  }
}

// Event that fires when there is incoming data.

void serialEvent(Serial p) {
  println("Received: " + p.readString());
}

//Draw function must be included if we want the code to run continuously. Note that it's empty.

void draw(){


}
