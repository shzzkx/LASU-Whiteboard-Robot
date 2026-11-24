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


const float GAP_DIST = 7.5; // cm
const float RADIUS = 20.0; // cm
const float STEP_DIST = 5.0; // cm
const float MAX_X = 116;
const float MAX_Y = 86;
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

const Point square[] = {{1, 1}};
const Point star[] = {{1, 1}};

// Colors: 0 is red, 120 is green, 240 is blue
const int square_color = 0;
const int star_color = 120;

void display_square() {
    Brain.Screen.setPenColor(square_color);
    for (int i = 0; i < 10; i++) {
        Brain.Screen.clearLine(i+1);
    }
    Brain.Screen.setCursor(6, 20);
    Brain.Screen.print("SQUARE");
    // brain.screen.print(square pixel art)
}

void display_star() {
    Brain.Screen.setPenColor(star_color);
    for (int i = 0; i < 10; i++) {
        Brain.Screen.clearLine(i+1);
    }
    Brain.Screen.setCursor(6, 20);
    Brain.Screen.print("STAR");
    // brain.screen.print(star pixel art)
}

int pick_shape() {
    int shape_num_counter = 0;

    Brain.Screen.setCursor(11, 17);
    Brain.Screen.print("Click the arrows to choose a shape,");
    Brain.Screen.setCursor(12, 14);
    Brain.Screen.print("then click the touch LED to start drawing!");

    while (!TouchLED9.is_pressing()) {
        if (shape_num_counter % 1 == 0) {
            display_square();
        }
        else if (shape_num_counter % 2 == 0) {
            display_star();
        }
        while(!Brain.buttonLeft.pressing() && !Brain.buttonRight.pressing()) {}
        if (Brain.buttonRight.pressing()) shape_num_counter++;
        else shape_num_counter--;
        while(Brain.buttonLeft.pressing() && Brain.buttonRight.pressing()) {}
    }

    if (shape_num_counter % 1 == 0) {
        return 1;
    }
    else if (shape_num_counter % 2 == 0) {
        return 2;
    }
}

Rope xy_to_length(const Point& point){

    Rope new_length{};

    float x = point.x;
    float y = point.y;

    new_length.rleft = sqrt(x*x+y*y);

    new_length.rright = sqrt((MAX_X - GAP_DIST - x)*(MAX_X - GAP_DIST-x)+y*y);

    return new_length;
}


float dist_to_deg(float dist){
    return dist*360/(6.0);
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
  

  for (;;) {
    // Pen up or down
    if (Controller.ButtonFDown.pressing()) {
      lower_pen_down(pen_up);
    }
    if (Controller.ButtonFUp.pressing()) {
      lift_pen_up(pen_up);
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
      leftMotor.spin(forward, vRight, percent);
      rightMotor.spin(forward,vLeft , percent);
    } else if (Controller.ButtonEDown.pressing()) {
      leftMotor.spin(reverse,vRight , percent);
      rightMotor.spin(reverse, vLeft, percent);
    } else {
      leftMotor.stop();
      rightMotor.stop();
    }

    Brain.Screen.clearLine(1);
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("L %.0f  R %.0f", vLeft, vRight);

    wait(50, msec);

    // // TODO: Change button
    // if (Controller.ButtonEUp.pressing())
    //   Brain.programStop();
  }
}


class Plotter
{
    private:
        Point current_point{};

    public:
        Plotter(){
            current_point = {0,0};
        }
        Plotter(float x, float y){
            current_point = {x,y};
        }
        // ~Plotter();

        void set_point_x(float x1){
            current_point.x = x1;
        }

        void set_point_y(float y1){
            current_point.y = y1;
        }

        bool out_of_range(const Point& dist){
            if(dist.x < 0 || dist.y < 0 || dist.x > MAX_X || dist.y > MAX_Y){
                Brain.Screen.print("Input Out of Range");
                return true;
            }
            return false;
        }

        void move_point_to_point (const Point& destination){

            Rope new_rope = xy_to_length(destination);
            Rope current_rope = xy_to_length(current_point);
            
            float left_deg = dist_to_deg(new_rope.rleft - current_rope.rleft);
            float right_deg = dist_to_deg(new_rope.rright - current_rope.rright);
        
            //check if max > 0.1?

            float big = fmax(fabs(left_deg), fabs(right_deg));
            const float BASE_SPEED = 20;   // percent
            float speed_left = BASE_SPEED * fabs(left_deg) / big;
            float speed_right = BASE_SPEED * fabs(right_deg) / big;

            //convert length to degrees
            double startL = leftMotor.position(degrees);
            double startR = rightMotor.position(degrees);

            bool leftDone  = (fabs(left_deg) < 0.1);
            bool rightDone = (fabs(right_deg) < 0.1);

            if(!leftDone){
                if(left_deg>0){
                    leftMotor.spin(forward, speed_left, percent);
                }else{
                    leftMotor.spin(reverse, speed_left, percent);
                }
            }

            if(!rightDone){
                if(right_deg>0){
                    rightMotor.spin(forward, speed_right, percent);
                }else{
                    rightMotor.spin(reverse, speed_right, percent);
                }
            }

            while (!leftDone || !rightDone) {
                if (!leftDone && fabs(leftMotor.position(degrees) - startL) >= fabs(left_deg)) {
                    leftMotor.stop(brake);
                    leftDone = true;
                }
                if (!rightDone && fabs(rightMotor.position(degrees) - startR) >= fabs(right_deg)) {
                    rightMotor.stop(brake);
                    rightDone = true;
                }
                wait(5, msec);
            }

            current_point = destination;
        }

        void move_path(const Point& destination)
        {
            if(!out_of_range(destination)){
                float delta_x = destination.x - current_point.x;
                float delta_y = destination.y - current_point.y;
                float hype_length = sqrt(delta_x*delta_x + delta_y*delta_y);

                int num_steps = (int)ceil(hype_length / STEP_DIST);
                
                Point start = current_point;
                Point nextPoint;
                for(int i=1; i<= num_steps; i++){
                    nextPoint.x = start.x+i*STEP_DIST/hype_length*delta_x;
                    nextPoint.y = start.y + i*STEP_DIST/hype_length*delta_y;
                    move_point_to_point(nextPoint);
                }
                Brain.Screen.print("Moved to %.1f, %.1f", destination.x, destination.y);
            }
        }

};



void draw_shape(int num_coords, Plotter& plotter, const Point to_draw[]){
    for(int i=1; i<num_coords; i++) { // Starts at 1 bc we already moved to first point
      plotter.move_path(to_draw[i]);
      if (Controller.ButtonEUp.pressing()) // TODO: Change button bindings
        i = num_coords + 1;
        // This will exit draw_shape, vector_path, and then call program stop from main
    }
}

void vector_path(bool& pen_up, Plotter& plotter, const Point to_draw[]){
    int num_coords = sizeof(to_draw) / sizeof(to_draw[0]);
    lift_pen_up(pen_up);
    plotter.move_path(to_draw[0]);
    lower_pen_down(pen_up);
    draw_shape(num_coords, plotter, to_draw);
    lift_pen_up(pen_up);
}


// Button for E-stop
// Button for controller control
int main(){

    vexcodeInit();

    // initialize
    bool pen_up = false;
    bool isRemoteControl;
    Plotter plotter(52, 86);

    leftMotor.setStopping(hold);
    rightMotor.setStopping(hold);
    penMotor.setStopping(hold);  

    //Start up
    configureAllSensors();
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Press UP to start autonomous mode.");
    Brain.Screen.newLine();
    Brain.Screen.print("Press DOWN to start remote control mode.");

    while(!Brain.buttonUp.pressing() && !Brain.buttonDown.pressing()){}
    if (Brain.buttonUp.pressing()) {
        isRemoteControl = false;
    } else if (Brain.buttonDown.pressing()) {
        isRemoteControl = true;
    }

    if(isRemoteControl){
        remote_control(pen_up);
    } else {
        int shape_num = pick_shape();
        if (shape_num == 1) {
            draw_shape(pen_up, plotter, square);
        }
        else if (shape_num == 2) {
            draw_shape(pen_up, plotter, star);
        }

        Brain.programStop();
    }
    // e stop
    
}