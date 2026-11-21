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
    { 5, },
    {
        {
            {85.03, 15.60},
            {29.76, 15.72},
            {29.76, 71.24},
            {85.15, 71.24},
            {85.03, 15.60},
        },
    }
};

#endif // DRAWING_DATA_H
