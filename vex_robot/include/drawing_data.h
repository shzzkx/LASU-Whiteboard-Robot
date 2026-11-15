#ifndef DRAWING_DATA_H
#define DRAWING_DATA_H

#define MAX_CONTOURS 5

#define MAX_COORDS 30

typedef struct {
    int numContours;
    int numCoords[MAX_CONTOURS];
    double coords[MAX_CONTOURS][MAX_COORDS][2];
} CoordData;

static const CoordData coordData = {
    1,
    { 17, },
    {
        {
            {97.90, 18.93},
            {74.43, 65.70},
            {23.30, 73.14},
            {22.82, 73.95},
            {60.03, 110.36},
            {51.29, 160.84},
            {51.62, 161.97},
            {97.90, 137.86},
            {141.75, 161.00},
            {144.34, 161.97},
            {144.66, 161.00},
            {135.92, 110.36},
            {173.14, 73.79},
            {171.84, 72.98},
            {121.52, 65.70},
            {98.54, 19.26},
            {97.90, 18.93},
        },
    }
};

#endif // DRAWING_DATA_H
