#include <iostream>
#include <cmath>

#include "vex.h"
using namespace vex;
using namespace std;

const float GAP_DIST = 5.0; // cm
const float RADIUS = 20.0; // cm
const float STEP_DIST = 1.0; // cm

brain Brain ;
inertialBrainInertial = inertial( ) ;
motor rightMotor(PORT1, bool(forward));
motor leftMotor(PORT6, bool(reverse));
// TODO: Add motor that controls pen.
motor groupmotors (rightMotor, leftMotor);

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
    return dist*360/(2*pi*RADIUS);
}

class Plotter
{

public:
    Point current_point{};
    Plotter(){
        current_point = {0,0};
    }
    Plotter(float x, float y){
        current_point = {x,y};
    }

    ~Plotter();

    void move_point_to_point (Point* destination){
        // float x_diff = point2->x - point1->x;
        // float y_diff = point2->y - point1->y;

        Rope new_rope = xy_to_length(destination);
        Rope current = xy_to_length(current_point);

        float r_left_value = new_rope.rleft;
        float r_right_value = new_rope.rright;

        float delta_left = current.rleft - new_rope.rleft;
        float delta_right = current.rright - new_rope.rright;
        // int numsteps = 1000;

        float slope = delta_left / delta_right;
        int percent = 20;

        //convert length to degrees
        float left_deg = dist_to_deg(r_left_value);
        float right_deg = dist_to_deg(r_right_value);

        if(delta_left>0){
            leftMotor.spin(forward, slope*percent, percent);
        }else{
            leftMotor.spin(reverse, slope*percent, percent);
        }

        if(delta_right>0){
            rightMotor.spin(forward, percent, percent);
        }else{
            rightMotor.spin(reverse, percent, percent);
        }

        if(delta_left > delta_right){
            while(abs(leftMotor.getPosition(degrees)-left_deg) > 10){}
        }else{
            while(abs(rightMotor.getPosition(degrees)-right_deg) > 10){}
        }
        rightMotor.stop();
        leftMotor.stop();
    }

    void move_path(Point* destination)
    {
        float delta_x = current_point.x - destination.x;
        float delta_y = current_point.y - destination.y;

        float num_steps = sqrt(delta_x*delta_x + delta_y*delta_y) / STEP_DIST;

        for(int i=1; i<= num_steps; i++){
            move_point_to_point({current_point.x+i*STEP_DIST, current_point.y + i*});
        }
    }

};

Plotter::~Plotter()
{
}

void move_point_to_point (Point* destination){
    // float x_diff = point2->x - point1->x;
    // float y_diff = point2->y - point1->y;

    Rope new_rope = xy_to_length(destination);
    float r_left_value = new_rope.rleft;
    float r_right_value = new_rope.rright;
    // int numsteps = 1000;

    float slope = r_left_value / r_right_value;
    int percent = 20;

    //convert length to degrees
    float left_deg = dist_to_deg(r_left_value);
    float right_deg = dist_to_deg(r_right_value);

    if(r_left_value>0){
        leftMotor.spin(forward, slope*percent, percent);
    }else{
        leftMotor.spin(reverse, slope*percent, percent);
    }

    if(r_right_value>0){
        rightMotor.spin(forward, percent, percent);
    }else{
        rightMotor.spin(reverse, percent, percent);
    }

    if(r_left_value > r_right_value){
        while(abs(leftMotor.getPosition(degrees)-left_deg) > 10){}
    }else{
        while(abs(rightMotor.getPosition(degrees)-right_deg) > 10){}
    }
    rightMotor.stop();
    leftMotor.stop();
}

void move_path(){

}


int main(){

    // initialize

    // Read from .h file

    // Convert into paths

}