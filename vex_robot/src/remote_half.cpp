#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// #include "drawing_data.h"


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


// START IQ MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END IQ MACROS


// Robot configuration code.
inertial BrainInertial = inertial();
motor rightMotor = motor(PORT7, false);
motor leftMotor = motor(PORT12, false);
motor penMotor = motor(PORT8, false);
controller Controller = controller();

// generating and setting random seed
void initializeRandomSeed(){
  wait(100,msec);
  double xAxis = BrainInertial.acceleration(xaxis) * 1000;
  double yAxis = BrainInertial.acceleration(yaxis) * 1000;
  double zAxis = BrainInertial.acceleration(zaxis) * 1000;
  // Combine these values into a single integer
  int seed = int(
    xAxis + yAxis + zAxis
  );
  // Set the seed
  srand(seed); 
}



void vexcodeInit() {

  // Initializing random seed.
  initializeRandomSeed(); 
}

#pragma endregion VEXcode Generated Robot Configuration

//----------------------------------------------------------------------------
//                                                                            
//    Module:       main.cpp                                                  
//    Author:       {author}                                                  
//    Created:      {date}                                                    
//    Description:  IQ project                                                
//                                                                            
//----------------------------------------------------------------------------

// Include the IQ Library
#include "iq_cpp.h"

// Allows for easier use of the VEX Library
using namespace vex;

typedef struct {
    int numContours;
    int numCoords[MAX_CONTOURS];
    double coords[MAX_CONTOURS][MAX_COORDS][2];
} CoordData;


const float GAP_DIST = 50; 
const float CF = 60;
const float ROPE_LENGTH = 1020;
const float MAX_WIDTH = 1020;
const float MAX_HEIGHT = 860;



// Start function
void configureAllSensors(){
  BrainInertial.calibrate();
  while(BrainInertial.isCalibrating()) {}
  Brain.Screen.setCursor(1,1);
  //holds poisiton when stopped
  //TODO: check if this is allowed
  leftMotor.setStopping(hold);
  rightMotor.setStopping(hold);
  //resets motor encoders at the start
  leftMotor.setPosition(0, degrees);
  rightMotor.setPosition(0, degrees);
  penMotor.setPosition(0, degrees);

}

struct Point
{
    float x;
    float y;
};

struct Rope{
    float rleft;
    float rright;
};

/* function that convert rope length to x and y (given by claude's stimutlation)
x = (L1² − L2² + D²) / (2D) + s/2
y = √(L1² − (x − s/2)²) 
*/ 

Point length_to_xy(Rope* rope){
    Point point{};
    float D = MAX_WIDTH - GAP_DIST; // distance between the two motors taken away from the coordinate system
    point.x = (rope->rleft*rope->rleft - rope->rright*rope->rright + D*D) / (2*D) + GAP_DIST/2;
    point.y = sqrt(rope->rleft*rope->rleft - (point.x - GAP_DIST/2)*(point.x - GAP_DIST/2));
    return point;
}

/* luna's formula to convert x and y to velocity
 dl/dt = 1 / (x^2 + y^2)^(1/2)* (dx/dt *x + dy/dt *y)
 */
float get_velocity(const Point &point, float dx, float dy) {
    return (1 / sqrt(point.x*point.x + point.y*point.y)) * (point.x*dx + point.y*dy);
}

// converts motor roation ot rope length change
float deg_to_dist(float deg){
    return deg*CF/360;
}

//controls to lift pen up or down
void lift_pen_up(bool& pen_up){
    if(pen_up == false){
        penMotor.spin(reverse, 20, percent);
        while(penMotor.position(degrees)> -90){}
        penMotor.stop(brake);
        pen_up = true;
    }
}
void lower_pen_down(bool& pen_up){
    if(pen_up == true){
        penMotor.spin(forward, 20, percent);
        while(penMotor.position(degrees)<0){}
        penMotor.stop(brake);
        pen_up = false;
    }
}


int main() {  
  bool pen_up = true;
  float vLeft = 0;
  float vRight = 0;
  
  configureAllSensors();

  //TODO: is this allowed? 
  for (;;) {
    // Pen up or down
    if (Controller.ButtonFDown.pressing()) {
      lower_pen_down(pen_up);
    }
    if (Controller.ButtonFUp.pressing()) {
      lift_pen_up(pen_up);
    }

    float dx = (float) Controller.AxisB.position() ;
    float dy = (float) Controller.AxisA.position();

    Rope current_rope = { ROPE_LENGTH, ROPE_LENGTH};

    //UPDATE rope length values
    current_rope.rleft += deg_to_dist(leftMotor.position(degrees));
    current_rope.rright += deg_to_dist(rightMotor.position(degrees));

    Point current_point = length_to_xy(&current_rope);
    // get motor velocity based on controller inputs x-y distance change
    vLeft = get_velocity({ current_point.x - GAP_DIST/2, current_point.y }, dx, dy);
    vRight = get_velocity({ current_point.x + GAP_DIST/2 - MAX_WIDTH, current_point.y }, dx, dy);

    if (Controller.ButtonEUp.pressing() ) {
      leftMotor.spin(forward, vLeft , dps);
      rightMotor.spin(forward, vRight, dps);
    // not really need is it?
    } else if (Controller.ButtonEDown.pressing() ) {
      leftMotor.spin(reverse, vLeft , dps);
      rightMotor.spin(reverse, vRight, dps);
    } else {
      leftMotor.stop();
      rightMotor.stop();
    }

    Brain.Screen.clearLine(1);
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("L %.0f  R %.0f", vLeft, vRight);
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("X %.0f  Y %.0f", current_point.x, current_point.y);
    Brain.Screen.setCursor(3, 1);
    Brain.Screen.print("LR %.0f  RR %.0f", current_rope.rleft, current_rope.rright);

    wait(90, msec);

    // TODO: Change button
    if (Controller.ButtonLDown.pressing())
      Brain.programStop();
  }
}
