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
touchled TouchLED9 = touchled(PORT9);
motor rightMotor = motor(PORT7, false);
motor leftMotor = motor(PORT12, false);
motor penMotor = motor(PORT8, false);
controller Controller = controller();

// Converts a color to a string
const char* convertColorToString(color col) {
  if (col == colorType::red) return "red";
  else if (col == colorType::green) return "green";
  else if (col == colorType::blue) return "blue";
  else if (col == colorType::white) return "white";
  else if (col == colorType::yellow) return "yellow";
  else if (col == colorType::orange) return "orange";
  else if (col == colorType::purple) return "purple";
  else if (col == colorType::cyan) return "cyan";
  else if (col == colorType::black) return "black";
  else if (col == colorType::transparent) return "transparent";
  else if (col == colorType::red_violet) return "red_violet";
  else if (col == colorType::violet) return "violet";
  else if (col == colorType::blue_violet) return "blue_violet";
  else if (col == colorType::blue_green) return "blue_green";
  else if (col == colorType::yellow_green) return "yellow_green";
  else if (col == colorType::yellow_orange) return "yellow_orange";
  else if (col == colorType::red_orange) return "red_orange";
  else if (col == colorType::none) return "none";
  else return "unknown";
}


// Convert colorType to string
const char* convertColorToString(colorType col) {
  if (col == colorType::red) return "red";
  else if (col == colorType::green) return "green";
  else if (col == colorType::blue) return "blue";
  else if (col == colorType::white) return "white";
  else if (col == colorType::yellow) return "yellow";
  else if (col == colorType::orange) return "orange";
  else if (col == colorType::purple) return "purple";
  else if (col == colorType::cyan) return "cyan";
  else if (col == colorType::black) return "black";
  else if (col == colorType::transparent) return "transparent";
  else if (col == colorType::red_violet) return "red_violet";
  else if (col == colorType::violet) return "violet";
  else if (col == colorType::blue_violet) return "blue_violet";
  else if (col == colorType::blue_green) return "blue_green";
  else if (col == colorType::yellow_green) return "yellow_green";
  else if (col == colorType::yellow_orange) return "yellow_orange";
  else if (col == colorType::red_orange) return "red_orange";
  else if (col == colorType::none) return "none";
  else return "unknown";
}


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
const float PRIM = 6.0; // cm
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

const Point starting_location = {52, 86};

const Point square[] = {{52, 56}, {52, 36}, {72, 36}, {72, 56}, {52, 56}};
const Point star[] = {{1, 1}};
const Point triangle[] = {{42, 56}, {52, 22.36}, {62, 56}, {42, 56}};

void display_square() {
    Brain.Screen.setCursor(2, 7);
    //Brain.Screen.print("SQUARE");
    // brain.screen.print(square pixel art)
    // Display square the robot will actually draw
    for (int i = 0; i < ((sizeof(square) / sizeof(square[i]) - 1)); i++) {
        float start_x = square[i].x + 5;
        float start_y = square[i].y - 20;
        float new_x = square[i+1].x + 5;
        float new_y = square[i+1].y - 20;
        // Additions of +5 and -20 made to center the square properly on the screen
        Brain.Screen.drawLine(start_x, start_y, new_x, new_y);
    }
}

void display_star() {
    Brain.Screen.setCursor(2, 8);
    Brain.Screen.print("STAR");
    // brain.screen.print(star pixel art)
}

void display_triangle() {
    Brain.Screen.setCursor(2, 6);
    //Brain.Screen.print("TRIANGLE");
    // brain.screen.print(triangle pixel art)
    for (int i = 0; i < ((sizeof(triangle) / sizeof(triangle[i]) - 1)); i++) {
        float start_x = triangle[i].x + 10;
        float start_y = triangle[i].y - 10;
        float new_x = triangle[i+1].x + 10;
        float new_y = triangle[i+1].y - 10;
        // Additions of +10 and -10 made to center the triangle properly on the screen
        Brain.Screen.drawLine(start_x, start_y, new_x, new_y);
    }
}

int pick_shape() {
    Brain.Screen.clearScreen();
    Brain.Screen.setPenColor(white);
    Brain.Screen.setCursor(5, 1);
    Brain.Screen.print("Use ARROWS to");
    Brain.Screen.setCursor(6, 1);
    Brain.Screen.print("select shape, press");
    Brain.Screen.setCursor(7, 1);
    Brain.Screen.print("TOUCH LED to draw!");

    bool pressed_next;

    int shape_num_counter = 1;
    while (!TouchLED9.pressing()) {
        for (int i = 0; i < 4; i++) {
          Brain.Screen.clearLine(i+1);
        }

        Brain.Screen.setCursor(1, 1);
        Brain.Screen.print("%d", shape_num_counter); // TEMP for debugging.

        if (shape_num_counter == 1) {
            display_square();
        }
        else if (shape_num_counter == 2) {
            display_star();
        }
        else if (shape_num_counter == 3) {
            display_triangle();
        }
        while(!Brain.buttonLeft.pressing() && !Brain.buttonRight.pressing()) {}
        if (Brain.buttonRight.pressing()) pressed_next = true;
        else pressed_next = false;
        while(Brain.buttonLeft.pressing() || Brain.buttonRight.pressing()) {}
        if (pressed_next == true) {
            if (shape_num_counter < 3) { 
                shape_num_counter++;
            }
            else {
              shape_num_counter = 1;
            }
        }
        else {
            if (shape_num_counter > 1) { 
                shape_num_counter--;
            }
            else {
              shape_num_counter = 3;
            }
        }
    }

    Brain.Screen.clearScreen();

    if (shape_num_counter % 1 == 0) {
        return 1;
    }
    else if (shape_num_counter % 2 == 0) {
        return 2;
    }
    else if (shape_num_counter % 3 == 0) {
        return 3;
    }

    return -1;
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
    return dist*360/(PRIM);
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
        Plotter(Point given_point) {
          current_point = given_point;
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

        void move_point_to_point (const Point& destination, bool &e_stop){

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

            while (!leftDone || !rightDone || !Controller.ButtonEUp.pressing()) {
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
            if (Controller.ButtonEUp.pressing()) e_stop = true;

            current_point = destination;
        }

        void move_path(const Point& destination, bool &e_stop)
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
                    move_point_to_point(nextPoint, e_stop);
                }
                if (Controller.ButtonEUp.pressing()) e_stop = true;
                Brain.Screen.print("Moved to %.1f, %.1f", destination.x, destination.y);
            }
        }

};



void draw_shape(int num_coords, Plotter& plotter, const Point to_draw[], bool &e_stop){
  int i = 1;
  while (!e_stop && i < num_coords)
    plotter.move_path(to_draw[i], e_stop);
    i++;
    // Emergency stop function.
    if (Controller.ButtonEUp.pressing()) e_stop = true;
      // This will exit draw_shape, vector_path, and then call program stop from main
}

void vector_path(bool& pen_up, Plotter& plotter, const Point to_draw[]){ 
    bool e_stop = false;
    int num_coords = sizeof(to_draw) / sizeof(to_draw[0]);
    lift_pen_up(pen_up);
    plotter.move_path(to_draw[0], e_stop);
    lower_pen_down(pen_up);
    draw_shape(num_coords, plotter, to_draw, e_stop);
    lift_pen_up(pen_up);
    plotter.move_path(starting_location, e_stop);
}


// Button for E-stop
// Button for controller control
int main(){

    vexcodeInit();

    // initialize
    bool pen_up = false;
    bool isRemoteControl;
    Plotter plotter(starting_location);

    leftMotor.setStopping(hold);
    rightMotor.setStopping(hold);
    penMotor.setStopping(hold);  

    //Start up
    configureAllSensors();
    Brain.Screen.clearScreen();
    Brain.Screen.setFont(mono15);
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Press UP");
    Brain.Screen.newLine();
    Brain.Screen.print("for autonomous.");
    Brain.Screen.newLine();
    Brain.Screen.newLine();
    Brain.Screen.print("Press DOWN");
    Brain.Screen.newLine();
    Brain.Screen.print("for remote control.");

    while(!Brain.buttonUp.pressing() && !Brain.buttonDown.pressing()){}
    if (Brain.buttonUp.pressing()) {
        TouchLED9.on(green);
        isRemoteControl = false;
    } else if (Brain.buttonDown.pressing()) {
        TouchLED9.on(blue);
        isRemoteControl = true;
    }
    while(Brain.buttonUp.pressing() && Brain.buttonDown.pressing()){}

    if(isRemoteControl){
        remote_control(pen_up);
    }
    else {
        int shape_num = pick_shape();
        if (shape_num == 1) {
            //vector_path(pen_up, plotter, square);
            Brain.Screen.print("Square selected");
            wait(2, seconds);
        }
        else if (shape_num == 2) {
            //vector_path(pen_up, plotter, star);
            Brain.Screen.print("Star selected");
            wait(2, seconds);
        }
        else if (shape_num == 3) {
            //vector_path(pen_up, plotter, triangle);
            Brain.Screen.print("Tri selected");
            wait(2, seconds);
        }
        else {
          Brain.Screen.print("Error occured");
          wait(2, seconds);
        }

        //Brain.programStop();
    }
    // e stop
    
}