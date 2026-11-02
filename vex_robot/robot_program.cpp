#include <iostream>
#include <cmath>
#include <cintstd>

#include "vex.h"
using namespace vex;
using namespace std;

typedef struct Point
{
    float x;
    float y;
};

typedef struct Rope{
    float rleft;
    float rright;
};


Rope xy_to_length(Point* point){
    // Check if x, y is out of range
    const float GAP_DIST;
    if(point == nullptr){
        return Rope{};
    }
    Rope new_length{};

    float x = point->x;
    float y = point->y;

    new_length.rleft = sqrt(x*x+y*y)

    new_length.rright = sqrt((GAP_DIST - x)*(GAP_DIST-x)+y*y)

    return new_length;
}


void move_point_to_point (point* destination, float* rleft, float* rright){
    // float x_diff = point2->x - point1->x;
    // float y_diff = point2->y - point1->y;

    Rope new_rope = xy_to_length(destination);
    float r_left_value = new_rope.rleft;
    float r_right_value = new_rope.rright;
    // int numsteps = 1000;

    float slope = r_left_value / r_right_value;
    int percent = 20;

    //convert length to degrees

    rightMotor.spin(forward, percent, percent);
    leftMotor.spin(forward, slope*percent, percent);

    if(r_left_value > r_right_value){
        while(abs(leftMotor.getPosition(degrees)-r_left_value) > 10){}
    }else{
        while(abs(rightMotor.getPosition(degrees)-r_right_value) > 10){}
    }
    rightMotor.stop();
    leftMotor.stop();
}

void move_path();


const float GAP_DIST = 5;
const
brain Brain ;
inertialBrainInertial = inertial( ) ;
motor rightMotor(PORT1, bool(forward));
motor leftMotor(PORT6, bool(reverse));
motor groupmotors (rightMotor, leftMotor);

int main(){

    // initialize

    // Read from .h file

    // Convert into paths

}