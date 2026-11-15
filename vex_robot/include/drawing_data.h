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
            {197.75, 36.29},
            {69.20, 36.57},
            {69.20, 165.68},
            {198.03, 165.68},
            {197.75, 36.29},
        },
    }
};

#endif // DRAWING_DATA_H
