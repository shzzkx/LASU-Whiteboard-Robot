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
motor rightMotor = motor(PORT7, false);
motor leftMotor = motor(PORT12, false);
motor penMotor = motor(PORT8, false);

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
const float DEG_POS1 = 0; //deg
const float DEG_POS2 = 40; //Pen down position

struct Point
{
    float x;
    float y;
};

struct Rope{
    float rleft;
    float rright;
};

Rope xy_to_length(Point* point){
    // Check if x, y is out of range
    if(point == nullptr){
        return Rope{};
    }
    Rope new_length{};

    float x = point->x;
    float y = point->y;

    new_length.rleft = sqrt(x*x+y*y);

    new_length.rright = sqrt((GAP_DIST - x)*(GAP_DIST-x)+y*y);

    return new_length;
}


float dist_to_deg(float dist){
    return dist*360/(2*3.14*RADIUS);
}

class Plotter
{
    private:
        Point current_point{};
        bool pen_up;

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
        bool out_of_range(Point* dist){
            if(dist->x > MAX_LENGTH || dist->y > MAX_WIDTH){
                //throw error
                return true;
            }
            return false;
        }
        void move_point_to_point (Point* destination){
            // float x_diff = point2->x - point1->x;
            // float y_diff = point2->y - point1->y;

            if(out_of_range(destination) || destination == nullptr){
                //output error
            }
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

        void move_path(Point* destination)
        {
            
            if(out_of_range(destination) || destination == nullptr){
                //output error
            }
            float delta_x = current_point.x - destination->x;
            float delta_y = current_point.y - destination->y;
            float hype_length = sqrt(delta_x*delta_x + delta_y*delta_y);

            float num_steps = sqrt(delta_x*delta_x + delta_y*delta_y) / STEP_DIST;
            
            Point nextPoint;
            for(int i=1; i<= num_steps; i++){
                nextPoint.x = current_point.x+i*STEP_DIST/hype_length*delta_x;
                nextPoint.y = current_point.y + i*STEP_DIST/hype_length*delta_y;
                move_point_to_point(&nextPoint);
            }
        }


        void lift_pen_up(){
            if(pen_up == false){
                penMotor.spin(reverse, 20, percent);
                while(penMotor.position(degrees)>DEG_POS1){}
                penMotor.stop(brake);
            }
        }
        void lower_pen_down(){
            if(pen_up == true){
                penMotor.spin(forward, 20, percent);
                while(penMotor.position(degrees)<DEG_POS2){}
                penMotor.stop(brake);
            }
        }

};

Plotter::~Plotter()
{
}


void draw_contour(int contour_number, Plotter &plotter){
    int num_coord = coordData::numCoords;
    Point point;
    for(int i=1; i<=num_coord; i++){
      point.x = coordData::coords[contour_number][i][0];
      point.y = coordData::coords[contour_number][i][1];
      plotter.move_path(&point);
    }
}


int main(){

    // initialize
    Plotter plotter;

    // Read from .h file
    int numContours = coordData::numContours;
    for(int i=0; i<numContours; i++){
        plotter.lift_pen_up();
        plotter.move_path({coordData::coords[i][0][0], coordData::coords[i][0][1]});
        plotter.lower_pen_down();
        draw_contour(i, plotter);
    }

    
}