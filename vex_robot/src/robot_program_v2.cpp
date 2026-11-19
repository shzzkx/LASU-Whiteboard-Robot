#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#include "vex.h"
#include "drawing_data.h"

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

const float GAP_DIST = 5.0; // cm
const float RADIUS = 20.0; // cm
const float STEP_DIST = 1.0; // cm
const float MAX_LENGTH = 4232;
const float MAX_WIDTH = 2322;
const float DEG_POS1 = 90; //deg
const float DEG_POS2 = -90; //Pen down position

// Start function
void configureAllSensors(){
  BrainInertial.calibrate();
  while(BrainInertial.isCalibrating()){}
  Brain.Screen.setCursor(1,1);
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


Rope xy_to_length(const Point& point){

    Rope new_length{};

    float x = point.x;
    float y = point.y;

    new_length.rleft = sqrt(x*x+y*y);

    new_length.rright = sqrt((GAP_DIST - x)*(GAP_DIST-x)+y*y);

    return new_length;
}


float dist_to_deg(float dist){
    return dist*360/(2*3.14*RADIUS);
}

void lift_pen_up(bool& pen_up){
    if(pen_up == false){
        penMotor.spin(reverse, 20, percent);
        while(penMotor.position(degrees)>DEG_POS1){}
        penMotor.stop(brake);
        pen_up = true;
    }
}
void lower_pen_down(bool& pen_up){
    if(pen_up == true){
        penMotor.spin(forward, 20, percent);
        while(penMotor.position(degrees)<DEG_POS2){}
        penMotor.stop(brake);
        pen_up = false;
    }
}

void remote_control(bool& pen_up) {  

  double vLeft = 30;
  double vRight = 30;
  const double step = 1;   // percent changed per loop while a button is held

  // Holds position when stopped.
  leftMotor.setStopping(hold);
  rightMotor.setStopping(hold);
  penMotor.setStopping(hold);  

  for (;;) {
    // Pen up or down
    if (Controller.ButtonFDown.pressing()) {
      penDown(pen_up);
    }
    if (Controller.ButtonFUp.pressing()) {
      penUp(pen_up);
    }

    // Left motor speed
    if (Controller.ButtonLUp.pressing()) {
      vLeft = fmin(vLeft + step, 100);
    }
    if (Controller.ButtonLDown.pressing()) {
      vLeft = fmax(vLeft - step, -100);
    }

    // Right motor speed
    if (Controller.ButtonRUp.pressing()) {
      vRight = fmin(vRight + step, 100);
    }
    if (Controller.ButtonRDown.pressing()) {
      vRight = fmax(vRight - step, -100);
    }

    // Drive
    if (Controller.ButtonEUp.pressing()) {
      leftMotor.spin(forward, vLeft, percent);
      rightMotor.spin(forward, vRight, percent);
    } else if (Controller.ButtonEDown.pressing()) {
      leftMotor.spin(reverse, vLeft, percent);
      rightMotor.spin(reverse, vRight, percent);
    } else {
      leftMotor.stop();
      rightMotor.stop();
    }

    print(vLeft, vRight);
    Brain.Screen.clearLine(1);
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("L %.0f  R %.0f", vLeft, vRight);

    wait(50, msec);

    // TODO: Change button
    if (Controller.ButtonEUp.pressing())
      Brain.programStop();
  }
}


class Plotter
{
    private:
        Point current_point{};

    public:
        Plotter(){
            current_point = {0,0};
            pen_up = true;
        }
        Plotter(float x, float y, bool pen){
            current_point = {x,y};
            pen_up = pen;
        }
        ~Plotter();

        void set_point_x(float x1){
            current_point.x = x1;
        }

        void set_point_y(float y1){
            current_point.y = y1;
        }

        bool out_of_range(const Point& dist){
            if(dist.x < 0 || dist.y < 0 || dist.x > MAX_LENGTH || dist.y > MAX_WIDTH){
                displayText(1,1, "Input Out of Range");
                return true;
            }
            return false;
        }

        void move_point_to_point (const Point& destination){

            Rope new_rope = xy_to_length(destination);
            Rope current_rope = xy_to_length(&current_point);

            float r_left_value = new_rope.rleft;
            float r_right_value = new_rope.rright;

            float delta_left = current_rope.rleft - new_rope.rleft;
            float delta_right = current_rope.rright - new_rope.rright;
            // int numsteps = 1000;

            float slope = delta_left / delta_right;
            int percent_speed = 20;

            //convert length to degrees
            float left_deg = dist_to_deg(r_left_value);
            float right_deg = dist_to_deg(r_right_value);

            if(delta_left>0){
                leftMotor.spin(forward, slope*percent_speed, percent);
            }else{
                leftMotor.spin(reverse, slope*percent_speed, percent);
            }

            if(delta_right>0){
                rightMotor.spin(forward, percent_speed, percent);
            }else{
                rightMotor.spin(reverse, percent_speed, percent);
            }

            if(delta_left > delta_right){
                while(fabs(leftMotor.position(degrees)-left_deg) > 10){}
            }else{
                while(fabs(rightMotor.position(degrees)-right_deg) > 10){}
            }
            rightMotor.stop();
            leftMotor.stop();
        }

        void move_path(const Point& destination)
        {
            
            float delta_x = current_point.x - destination.x;
            float delta_y = current_point.y - destination.y;
            float hype_length = sqrt(delta_x*delta_x + delta_y*delta_y);

            float num_steps = sqrt(delta_x*delta_x + delta_y*delta_y) / STEP_DIST;
            
            Point nextPoint;
            for(int i=1; i<= num_steps; i++){
                nextPoint.x = current_point.x+i*STEP_DIST/hype_length*delta_x;
                nextPoint.y = current_point.y + i*STEP_DIST/hype_length*delta_y;
                move_point_to_point(&nextPoint);
            }
            string text = "Moved to " +  destination.x " , " + destination.y;
            displayText(1,1, text);
        }

};



void draw_contour(int contour_number, Plotter &plotter){
    int num_coord = coordData.numCoords[contour_number];
    Point point;
    for(int i=1; i<=num_coord; i++){
      point.x = coordData.coords[contour_number][i][0];
      point.y = coordData.coords[contour_number][i][1];
      plotter.move_path(&point);
    }
}

// Button for E-stop
// Button for controller control
int main(){

    // initialize
    Plotter plotter;

    //Start up
    configureAllSensors();
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Press the check button to start drawing");
    Brain.Screen.newLine();
    Brain.Screen.print("Press the X button to enter remote control mode");

    if (Brain.buttonCheck.pressing()) {
        isRemoteControl = false;
    } else if (Brain.buttonX.pressing()) {
        isRemoteControl = true;
    }

    if(isRemoteControl){
        remote_control()
    }else{
        // Read from .h file
        int numContours = coordData.numContours;
        for(int i=0; i<numContours; i++){
            // TODO: Fix this condition
            plotter.lift_pen_up();
            Point p;
            // static_cast<float> converts double coordinates (from header file)
            // into floats x and y for the struct
            p.x = static_cast<float>(coordData.coords[i][0][0]);
            p.y = static_cast<float>(coordData.coords[i][0][1]);
            plotter.move_path(&p);
            plotter.lower_pen_down();
            draw_contour(i, plotter);
            if (Controller.ButtonEUp.pressing())
                Brain.programStop();
        }

    // e stop
    
}