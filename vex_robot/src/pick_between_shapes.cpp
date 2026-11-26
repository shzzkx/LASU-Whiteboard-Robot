#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


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
motor leftMotor = motor(PORT7, false);
motor rightMotor = motor(PORT12, true);
motor penMotor = motor(PORT8, false);
controller Controller = controller();
touchled TouchLED6 = touchled(PORT6);


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


// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

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
//const for remote control
const float CF = 60;  //mm
const float ROPE_LENGTH = 1020; // mm
const float MAX_WIDTH = 1020; // mm
const float MAX_HEIGHT = 860; //mm


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
const Point star[] = {{39.61, 15.73}, {32.52, 29.87}, {16.90, 32.37}, {28.16, 43.38}, {25.61, 59.00},
            {39.61, 51.70}, {53.66, 59.00}, {51.12, 43.38}, {62.37, 32.32}, {46.76, 29.87}, {39.61, 15.73}};
const Point triangle[] = {{42, 56}, {52, 22.36}, {62, 56}, {42, 56}};

void display_square() {
    Brain.Screen.setCursor(2, 7);
    //Brain.Screen.print("SQUARE");
    // brain.screen.print(square pixel art)
    // Display square the robot will actually draw
    for (int i = 0; i < ((sizeof(square) / sizeof(square[i]) - 1)); i++) {
        float start_x = square[i].x * 1.1 + 15;
        float start_y = square[i].y * 1.1 - 20;
        float new_x = square[i+1].x * 1.1 + 15;
        float new_y = square[i+1].y * 1.1 - 20;
        // Additions of +5 and -20 made to center the square properly on the screen
        Brain.Screen.drawLine(start_x, start_y, new_x, new_y);
    }
}

void display_star() {
    Brain.Screen.setCursor(2, 8);
    //Brain.Screen.print("STAR");
    // brain.screen.print(star pixel art)
    for (int i = 0; i < ((sizeof(star) / sizeof(star[i]) - 1)); i++) {
        float start_x = star[i].x + 35;
        float start_y = star[i].y - 10;
        float new_x = star[i+1].x + 35;
        float new_y = star[i+1].y - 10;
        // Additions of +35 and -10 made to center the star properly on the screen
        Brain.Screen.drawLine(start_x, start_y, new_x, new_y);
    }
}

void display_triangle() {
    Brain.Screen.setCursor(2, 6);
    //Brain.Screen.print("TRIANGLE");
    // brain.screen.print(triangle pixel art)
    for (int i = 0; i < ((sizeof(triangle) / sizeof(triangle[i]) - 1)); i++) {
        float start_x = triangle[i].x + 20;
        float start_y = triangle[i].y - 10;
        float new_x = triangle[i+1].x + 20;
        float new_y = triangle[i+1].y - 10;
        // Additions of +20 and -10 made to center the triangle properly on the screen
        Brain.Screen.drawLine(start_x, start_y, new_x, new_y);
    }
}

int pick_shape() {
    Brain.Screen.clearScreen();
    Brain.Screen.setPenColor(white);
    Brain.Screen.setCursor(5, 6);
    Brain.Screen.print("Use ARROWS to");
    Brain.Screen.setCursor(6, 1);
    Brain.Screen.print("select shape, press");
    Brain.Screen.setCursor(7, 2);
    Brain.Screen.print("TOUCH LED to draw!");

    bool pressed_next;

    int shape_num_counter = 1;
    while (!TouchLED6.pressing()) {
        for (int i = 0; i < 4; i++) {
          Brain.Screen.clearLine(i+1);
        }

        //Brain.Screen.setCursor(1, 1);
        //Brain.Screen.print("%d", shape_num_counter); // TEMP for debugging.

        if (shape_num_counter == 1) {
            display_square();
        }
        else if (shape_num_counter == 2) {
            display_star();
        }
        else if (shape_num_counter == 3) {
            display_triangle();
        }
        while(!Brain.buttonLeft.pressing() && !Brain.buttonRight.pressing()) {
          if (TouchLED6.pressing()) {
              Brain.Screen.clearScreen();
              if (shape_num_counter == 1) {
                  return 1;
              }
              else if (shape_num_counter == 2) {
                  return 2;
              }
              else if (shape_num_counter == 3) {
                  return 3;
              }
          }
        }
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
        leftMotor.stop(brake);
        rightMotor.stop(brake);
        penMotor.spin(reverse, 20, percent);
        while(penMotor.position(degrees)>0){}
        penMotor.stop(brake);
        pen_up = true;
    }
}
void lower_pen_down(bool& pen_up){
    if(pen_up == true){
        leftMotor.stop(brake);
        rightMotor.stop(brake);
        penMotor.spin(forward, 20, percent);
        while(penMotor.position(degrees)<90){}
        penMotor.stop(brake);
        pen_up = false;
    }
}

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

void remote_control(bool& pen_up) {  
  float vLeft = 0;
  float vRight = 0;
  
  configureAllSensors();
  
  while(!Controller.ButtonEDown.pressing()) {
    // Pen up or down
    if (Controller.ButtonFDown.pressing()) {
      lower_pen_down(pen_up);
    }
    if (Controller.ButtonFUp.pressing()) {
      lift_pen_up(pen_up);
    }

    float dx = (float) Controller.AxisB.position()*5 ;
    float dy = (float) Controller.AxisA.position()*5;

    Rope current_rope = { ROPE_LENGTH, ROPE_LENGTH};

    //UPDATE rope length values
    current_rope.rleft += deg_to_dist(leftMotor.position(degrees));
    current_rope.rright += deg_to_dist(rightMotor.position(degrees));

    Point current_point = length_to_xy(&current_rope);
    // get motor velocity based on controller inputs x-y distance change
    vLeft = get_velocity({ current_point.x - GAP_DIST/2, current_point.y }, dx, dy);
    vRight = get_velocity({ current_point.x + GAP_DIST/2 - MAX_WIDTH, current_point.y }, dx, dy);

    if (Controller.AxisB.position() != 0 || Controller.AxisB.position() != 0 ) {
      leftMotor.spin(forward, vLeft , dps);
      rightMotor.spin(forward, vRight, dps);
    }
    else {
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

        float get_point_x() {
          return current_point.x;
        }
        float get_point_y() {
          return current_point.y;
        }

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

        void move_point_to_point (const Point& destination, bool &e_stop, const int BASE_SPEED = 40){

            Rope new_rope = xy_to_length(destination);
            Rope current_rope = xy_to_length(current_point);
            
            float left_deg = dist_to_deg(new_rope.rleft - current_rope.rleft);
            float right_deg = dist_to_deg(new_rope.rright - current_rope.rright);

            float big = fmax(fabs(left_deg), fabs(right_deg));
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
                //if (Controller.ButtonEUp.pressing()) e_stop = true;
                wait(5, msec);
            }

            current_point = destination;
        }

        void move_path(const Point& destination, bool &e_stop)
        {
            if(!out_of_range(destination)){
                Brain.Screen.setCursor(5, 1);
                Brain.Screen.print("Moving to %.1f, %.1f", destination.x, destination.y);
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
                Brain.Screen.setCursor(6, 1);
                Brain.Screen.setFont(mono15);
                Brain.Screen.print("Moved to %.1f, %.1f", destination.x, destination.y);
            }
        }

};



void draw_shape(int num_coords, Plotter& plotter, const Point to_draw[], bool &e_stop){
  int i = 1;
  Brain.Screen.newLine();
  while (!e_stop && i < num_coords) {
    plotter.move_path(to_draw[i], e_stop);
    i++;
    // Emergency stop function.
    if (Controller.ButtonEUp.pressing()) e_stop = true;
      // This will exit draw_shape, vector_path, and then call program stop from main
  }
}

void vector_path(bool& pen_up, Plotter& plotter, const Point to_draw[], bool &e_stop, int num_coords){
    lift_pen_up(pen_up);
    plotter.move_path(to_draw[0], e_stop);
    lower_pen_down(pen_up);
    // draw_shape(num_coords, plotter, to_draw, e_stop);
    draw_shape(num_coords, plotter, to_draw, e_stop);
    Brain.Screen.setCursor(1,1);
}


// Button for E-stop
// Button for controller control
int main(){

    vexcodeInit();

    // initialize
    bool pen_up = false;
    bool isRemoteControl;
    bool e_stop = false;
    Plotter plotter(starting_location);

    leftMotor.setStopping(hold);
    rightMotor.setStopping(hold);
    penMotor.setStopping(hold);
    penMotor.setPosition(90, degrees); 

    //Start up
    configureAllSensors();
    Brain.Screen.clearScreen();
    Brain.Screen.setFont(mono15);
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Press LEFT");
    Brain.Screen.newLine();
    Brain.Screen.print("for autonomous.");
    Brain.Screen.newLine();
    Brain.Screen.newLine();
    Brain.Screen.print("Press RIGHT");
    Brain.Screen.newLine();
    Brain.Screen.print("for remote control.");

    while(!Brain.buttonUp.pressing() && !Brain.buttonDown.pressing()){}
    if (Brain.buttonUp.pressing()) {
        TouchLED6.on(green);
        isRemoteControl = false;
    } else if (Brain.buttonDown.pressing()) {
        TouchLED6.on(blue);
        isRemoteControl = true;
    }
    while(Brain.buttonUp.pressing() && Brain.buttonDown.pressing()){}

    if(isRemoteControl){
        remote_control(pen_up);
    }
    else {
        //bool temp = false;
        //Point p1 = {52, 46};
        //Point p2 = {72, 46};
        //plotter.move_point_to_point(p1, temp);
        
        //plotter.move_path(p2, temp);
        
        //plotter.move_path(p1, temp);
        int shape_num = pick_shape();
        Brain.Screen.setCursor(2, 1);
        Brain.Screen.print("Drawing");
        Brain.Screen.setCursor(3, 1);
        if (shape_num == 1) {
            Brain.Screen.print("Square...");
            vector_path(pen_up, plotter, square, e_stop, 5);
        }
        else if (shape_num == 2) {
            Brain.Screen.print("Star...");
            vector_path(pen_up, plotter, star, e_stop, 11);
        }
        else if (shape_num == 3) {
            Brain.Screen.print("Triangle...");
            vector_path(pen_up, plotter, triangle, e_stop, 4);
        }
        else {
          Brain.Screen.print("Error occurred");
          wait(2, seconds);
        }
    }
    Brain.Screen.clearScreen();
    Brain.Screen.setFont(mono20);
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("Drawing");
    Brain.Screen.setCursor(4, 1);
    Brain.Screen.print("finished.");
    lift_pen_up(pen_up);
    plotter.move_point_to_point(starting_location, e_stop, 30);
    Brain.programStop();
    
}