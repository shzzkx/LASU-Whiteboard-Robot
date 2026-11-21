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
    { 16, },
    {
        {
            {42.10, 8.14},
            {32.01, 28.25},
            {10.02, 31.45},
            {9.81, 31.80},
            {25.81, 47.45},
            {22.06, 69.16},
            {22.20, 69.65},
            {42.10, 59.28},
            {62.06, 69.65},
            {62.20, 69.23},
            {58.45, 47.45},
            {74.45, 31.73},
            {73.89, 31.38},
            {52.25, 28.25},
            {42.37, 8.28},
            {42.10, 8.14},
        },
    }
};

#endif // DRAWING_DATA_H
