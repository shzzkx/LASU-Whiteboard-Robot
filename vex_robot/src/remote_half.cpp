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

#define MAX_CONTOURS 5

#define MAX_COORDS 30

typedef struct {
    int numContours;
    int numCoords[MAX_CONTOURS];
    double coords[MAX_CONTOURS][MAX_COORDS][2];
} CoordData;


const float GAP_DIST = 50; 
// BUG?: units - other lengths are mm. If spool circumference is 6 cm, use 60/(2*3.14).
const float RADIUS = 6/(2*3.14);
const float MAX_LENGTH = 860;
const float ROPE_LENGTH = 1000;
const float MAX_WIDTH = 520;
  
const float MAX_HEIGHT = 860;
const float DEG_POS1 = 90; 
const float DEG_POS2 = -90; 

// Start function
void configureAllSensors(){
  BrainInertial.calibrate();
  while(BrainInertial.isCalibrating()){}
  Brain.Screen.setCursor(1,1);
  leftMotor.setStopping(hold);
  rightMotor.setStopping(hold);
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



/* function that convert rope length to x and y 
x = (L1² − L2² + D²) / (2D) + s/2
y = √(L1² − (x − s/2)²) 
*/ 

Point length_to_xy(Rope* rope){
    Point point{};
    float D = MAX_WIDTH - GAP_DIST; // distance between the two motors
    point.x = (rope->rleft*rope->rleft - rope->rright*rope->rright + D*D) / (2*D) + GAP_DIST/2;
    point.y = sqrt(rope->rleft*rope->rleft - (point.x - GAP_DIST/2)*(point.x - GAP_DIST/2));
    return point;
}



float get_velocity(const Point &point, float dx, float dy) {
    return (1 / sqrt(point.x*point.x + point.y*point.y)) * (point.x*dx + point.y*dy);
}

float deg_to_dist(float deg){
    return deg*2*3.14*RADIUS/360;
}

void lift_pen_up(bool& pen_up){
    if(pen_up == false){
        //      penMotor.spinToPosition(-90, degrees, 20, velocityUnits::pct, true);
        penMotor.spin(reverse, 20, percent);
        while(penMotor.position(degrees)> -90){}
        penMotor.stop(brake);
        pen_up = true;
    }
}
void lower_pen_down(bool& pen_up){
    if(pen_up == true){
        
        penMotor.spin(forward, 20, percent);
        while(penMotor.position(degrees)<90){}
        penMotor.stop(brake);
        pen_up = false;
    }
}

bool out_of_range(const Point& dist){
    if(isnan(dist.x) || isnan(dist.y) || dist.x < 0 || dist.y < 0 || dist.x > MAX_WIDTH || dist.y > MAX_HEIGHT) {
        Brain.Screen.setCursor(4, 1);
        Brain.Screen.print("Input Out of Range");
        return true;
    }
    return false;
}   

    float vLeft = 0;
    float vRight = 0;
int main() {  
    bool pen_up = true;

  
  configureAllSensors();
  // holds poisiton when stopped
  leftMotor.setStopping(hold);
  rightMotor.setStopping(hold);
  penMotor.setStopping(hold);  

  for (;;) {
    // Pen up or down
    if (Controller.ButtonFDown.pressing()) {
      lower_pen_down(pen_up);
    }
    if (Controller.ButtonFUp.pressing()) {
      lift_pen_up(pen_up);
    }

  float dx = (float) Controller.AxisB.position();
  

  //  float dy = -(float) Controller.AxisA.position(); -- > ??
  float dy = - (float) Controller.AxisA.position();


    if (dx < 5 && dx > -5) {
        dx = 0;
    }
    if (dy < 5 && dy > -5) {
        dy = 0;
    }

    Rope current_rope = { ROPE_LENGTH, ROPE_LENGTH};
    //use motor encoders to get current rope length 
    current_rope.rleft += deg_to_dist(leftMotor.position(degrees));
    current_rope.rright += deg_to_dist(rightMotor.position(degrees));

    Point current_point = length_to_xy(&current_rope);
    
    
    vLeft = get_velocity({ current_point.x - GAP_DIST/2, current_point.y }, dx, dy);

    Point point2 = { current_point.x + GAP_DIST/2 - MAX_WIDTH, current_point.y };

    vRight = get_velocity(point2, dx, dy);

    vLeft = vLeft * 360 / (2 * 3.14 * RADIUS);
    vRight = vRight * 360 / (2 * 3.14 * RADIUS);
   

    if (Controller.ButtonEUp.pressing() && !out_of_range(current_point)) {
      leftMotor.spin(forward, vLeft , dps);
      rightMotor.spin(forward, vRight, dps);

   
    } else if (Controller.ButtonEDown.pressing() && !out_of_range(current_point)) {
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


    wait(50, msec);

    // TODO: Change button
    if (Controller.ButtonLDown.pressing())
      Brain.programStop();
  }
}



// Button for E-stop
// Button for controller control
