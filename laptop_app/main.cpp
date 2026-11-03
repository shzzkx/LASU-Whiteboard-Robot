#include <opencv2/opencv.hpp>
#include <opencv2/geometry/2d.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

// TEMP for debugging file issues
#include <filesystem>
namespace fs = std::filesystem;

/*
FATAL ERROR CODES
1: Drawing image could not be loaded.
2: Too many contours (shapes) in the image.
3: No usable contours in the image (either blank image, or every contour needed more than 30 coords.)
4: Could not open drawing_data.h for file writing.

WARNING ERROR CODES
1. Contour needed more than 30 coords. This contour will be skipped, valid contours will still be drawn.
*/

/*
IMAGE SPECIFICATIONS
- Image must be black and white (or grayscale).
- Image cannot be a png.
*/

int main()
{
    // B1: Load the input image.
    cv::Mat image = cv::imread("../drawings/square_drawing.png");

    // TEMP for debugging
    std::cout << "CWD: " << fs::current_path() << '\n';
    std::string path = "../drawings/square_drawing.png";
    std::cout << "Exists: " << fs::exists(path) << '\n';
    std::cout << "Absolute: " << fs::absolute(path) << '\n';

    if (image.empty())
    {
        std::cerr << "Could not load square_drawing.png\n";
        return 1;
    }

    // B2: Convert color to grayscale.
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // B3: Convert to a binary image.
    // Dark lines become white; light background becomes black.
    cv::Mat binary;
    cv::threshold(
        gray, binary, 180, 255, cv::THRESH_BINARY_INV);

    // B4: Extract contour point sequences.
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(
        binary,
        contours,
        cv::RETR_EXTERNAL, // Previously RETR_LIST, but that caused contour lines drawn around (insetad of on) outline
        cv::CHAIN_APPROX_SIMPLE);

    
    // TEMP for debugging
    // Visually confirm what contour tracing looks like
    // by creating image of red contours overlayed on original image.
    // Red contours might be jagged but the actual coordinates will not be.
    // This will run even if FATAL error occurs.
    cv::Mat debug = image.clone();
    cv::drawContours(debug, contours, -1, cv::Scalar(0, 0, 255), 2);
    cv::imwrite("debug_contours.png", debug);

    // Example whiteboard area: 300 mm (width) x 200 mm (height).
    // TODO: Replace with actual whiteboard area dimensions.
    const double boardWidth_mm = 300.0;
    const double boardHeight_mm = 200.0;

    // Scale factor is based on whichever axis runs out of drawing space first.
    // This preserves aspect ratio.
    const double scale = std::min(boardWidth_mm / image.cols,
                              boardHeight_mm / image.rows);

    // Increasing epsilon will simplify the contour more.
    // This is an opencv thing.
    const double epsilon = 2.0;

    const int MAX_CONTOURS = 5; // Maximum # of contours (shapes) per drawing.
                                // E.g. Image of square will have one contour,
                                // image of a square with a circle inside will have two contours, etc.
    const int MAX_COORDS = 30;  // Maximum # of coordinates per contour.

    // Vector to store contours' information.
    // (C++ vector is just a dynamic array.)
    // (Type Point is from OpenCV, and represents a 2D point with integer coordinates.)
    std::vector<std::vector<cv::Point>> cleanedContours;
    for (const auto &contour : contours)
    {
        // Ignore tiny contours, often noise or specks.
        if (cv::contourArea(contour) < 10.0)
        {
            continue;
        }

        // B5a: Simplify the contour.
        // Create vector to hold simplified contour points.
        std::vector<cv::Point> simplified;
        cv::approxPolyDP(
            contour, simplified, epsilon, true);

        // Check if the number of coordinates the contour requires exceeds the max of 30.
        if (simplified.size() + 1 > (size_t)MAX_COORDS) // +1 to account for going back to first coord at end.
        {
            std::cerr << "WARNING: Contour with " << simplified.size() + 1
                      << " coordinates (max is " << MAX_COORDS
                      << ".) This contour will be skipped.\n";
            continue;
        }

        // Add simplified points to paths vector.
        cleanedContours.push_back(simplified);
    }

    /*
    Suppose two shapes in image - one square and one triangle.
    cleanedContours = {
        { 4 points },    // square
        { 3 points }     // triangle
    }
    */

    // Check if the number of contours the image requries exceeds the max of 5.
    if (cleanedContours.size() > (size_t)MAX_CONTOURS)
    {
        std::cerr << "FATAL: " << cleanedContours.size()
                  << " contours (max is " << MAX_CONTOURS
                  << ".) This image is too complex and will not be drawn.\n";
        return 2;
    }

    // Imagine if no contours were simple enough (less than 30 coords needed)
    // or if blank image passed in accidentally.
    if (cleanedContours.empty())
    {
        std::cerr << "FATAL: No usable contours found. "
                  << "Check the image, the threshold (180), "
                  << "and the minimum area (10.0).\n";
        return 3;
    }

    // Write into header file!
    std::ofstream fileWriter;
    fileWriter.open("../vex_robot/drawing_data.h");

    // Check if file opened.
    if (!fileWriter.is_open())
    {
        std::cerr << "FATAL: Could not open drawing_data.h for writing.\n";
        return 4;
    }
    else // If file opened successfully, then write the paths data.
    {

        fileWriter << std::fixed << std::setprecision(2); // Keep coords accurate to 2 decimal places.
        fileWriter << "#ifndef DRAWING_DATA_H\n";
        fileWriter << "#define DRAWING_DATA_H\n\n";
        fileWriter << "#define MAX_CONTOURS " << MAX_CONTOURS << "\n\n";
        fileWriter << "#define MAX_COORDS " << MAX_COORDS << "\n\n";

        fileWriter << "typedef struct {\n";
        fileWriter << "    int numContours;\n";                            // How many contours (shapes) in the image.
        fileWriter << "    int numCoords[MAX_CONTOURS];\n";                // Array of how many coordinates there are per contour.
        fileWriter << "    double coords[MAX_CONTOURS][MAX_COORDS][2];\n"; // Array of coordinates per contour.
        fileWriter << "} CoordData;\n\n";

        // Create and write into CoordData object named coordData.
        // We're going to use static const aggregate initialisation which will look like some weird bracket thing.
        // However static const object using aggregate initialisation is the best way to do it.
        // Alternative would be loading values of CoordData object
        // using a function, but the data would be stored in flash and copied to RAM at startup.
        // With static const object program will read CoordData object data
        // straight from flash memory.

        // TODO: Name coordData a different name (so object name is not the same as the type name.)
        fileWriter << "static const CoordData coordData = {\n";

        // Fill in numContours with how many contours there are.
        // c++ vector.size() returns the number of elements in the vector.
        fileWriter << "    " << cleanedContours.size() << ",\n";

        // Fill in numCoords with the # of coordinates in each contour
        fileWriter << "    { ";
        for (const auto &shape : cleanedContours) // For contour in contours vector, write its size.
            fileWriter << shape.size() + 1 << ", "; // +1 to account for going back to first coord at end.
        fileWriter << "},\n";
        fileWriter << "    {\n";

        // Now to fill in the huge coords array with the actual coordinates of each contour.
        // Loop over # of different contours (shapes) in the image png.
        for (const auto &contour : cleanedContours)
        {
            fileWriter << "        {\n";
            // Loop over each coordinate in contour.
            for (const auto &coord : contour)
            {
                // Scale coords to mm for whiteabord.
                // NOTE: If we want origin to be in bottom left,
                // use double y_mm = (image.rows - 1 - coord.y) * scaleY;
                double x_mm = coord.x * scale;
                double y_mm = coord.y * scale;

                // Write coordinate into array.
                fileWriter << "            {" << x_mm << ", " << y_mm << "},\n";

                // TEMP for debugging
                // Print coordinates to console.
                std::cout << "  x=" << x_mm
                          << ", y=" << y_mm << '\n';
            }
            // Append the first coordinate to the end
            // so that the robot will draw a closed shape.
            double x_mm = contour.front().x * scale;
            double y_mm = contour.front().y * scale;
            fileWriter << "            {" << x_mm << ", " << y_mm << "},\n";
            fileWriter << "        },\n";
        }
        // End of writing into the header file.
        fileWriter << "    }\n";
        fileWriter << "};\n\n";
        fileWriter << "#endif // DRAWING_DATA_H\n";
        fileWriter.close();
    }

    return 0;
}