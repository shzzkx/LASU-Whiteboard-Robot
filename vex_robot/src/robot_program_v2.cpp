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

static const CoordData coordData = {
    1,
    { 5, },
    {
        {
            {197.75, 36.29},
            {69.20, 36.57},
            {69.20, 165.68},
            {198.03, 165.68},
            {197.75, 36.29},
        },
    }
};

const float GAP_DIST = 7.5; // cm
const float RADIUS = 20.0; // cm

const float STEP_DIST = 5.0; // cm
const float MAX_X = 116;
const float MAX_Y = 86;
const float DEG_POS1 = 90; //deg
const float DEG_POS2 = -90; //Pen down position

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



void draw_contour(int contour_number, Plotter &plotter){
    int num_coord = coordData.numCoords[contour_number];
    Point point;
    for(int i=1; i<num_coord; i++){
      point.x = coordData.coords[contour_number][i][0];
      point.y = coordData.coords[contour_number][i][1];
      plotter.move_path(point);
    }
}

void vector_path(bool& pen_up, Plotter& plotter){
  int numContours = coordData.numContours;
  for(int i=0; i<numContours; i++){
      // TODO: Fix this condition
      lift_pen_up(pen_up);
      Point p;
      // static_cast<float> converts double coordinates (from header file)
      // into floats x and y for the struct
      p.x = static_cast<float>(coordData.coords[i][0][0]);
      p.y = static_cast<float>(coordData.coords[i][0][1]);
      plotter.move_path(p);
      lower_pen_down(pen_up);
      draw_contour(i, plotter);
      if (Controller.ButtonEUp.pressing()) // TODO: Change button bindings
          Brain.programStop();
  }
}

// Button for E-stop
// Button for controller control
int main(){

    vexcodeInit();

    // initialize
    bool pen_up = true;
    bool isRemoteControl;

    leftMotor.setStopping(hold);
    rightMotor.setStopping(hold);
    penMotor.setStopping(hold);  

    //Start up
    configureAllSensors();
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Press the check button to start drawing");
    Brain.Screen.newLine();
    Brain.Screen.print("Press the X button to enter remote control mode");

    while(!Brain.buttonUp.pressing() && !Brain.buttonDown.pressing()){}
    if (Brain.buttonUp.pressing()) {
        isRemoteControl = false;
    } else if (Brain.buttonDown.pressing()) {
        isRemoteControl = true;
    }

    if(isRemoteControl){
        remote_control(pen_up);
    }else{
        Plotter plotter(52, 86);
        Point p{52, 46};
        plotter.move_point_to_point(p);

        Point p2{82, 48};
        plotter.move_path(p2);
        // plotter.move_point_to_point(p2);

        // vector_path(pen_up, plotter);
        Brain.programStop();
    }
    // e stop
    
}
