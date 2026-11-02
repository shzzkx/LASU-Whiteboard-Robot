#include <opencv2/opencv.hpp>
#include <opencv2/geometry/2d.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

// TEMP for debugging file issues
#include <filesystem>
namespace fs = std::filesystem;


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

    // Example whiteboard area: 300 mm (width) x 200 mm (height).
    // TODO: Replace with actual whiteboard area dimensions.
    const double boardWidth_mm = 300.0;
    const double boardHeight_mm = 200.0;

    const double scaleX = boardWidth_mm / image.cols;
    const double scaleY = boardHeight_mm / image.rows;

    // Tune this to simplify more or less aggressively.
    const double epsilon = 2.0;

    const int MAX_CONTOURS = 5; // Maximum # of contours (shapes) per drawing.
        // E.g. Image of square will have one contour,
        // image of a square with a circle inside will have two contours, etc.
    const int MAX_COORDS = 30; // Maximum # of coordinates per contour.

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
        if (simplified.size() > (size_t)MAX_COORDS)
        {
            std::cerr << "ERROR: Contour with " << simplified.size()
                      << " coordinates (max is " << MAX_COORDS
                      << ".) This contour will be skipped.\n";
            continue;
        }

        // Add simplified points to paths vector.
        cleanedContours.push_back(simplified);
    }


    // Write into header file!
    std::ofstream fileWriter;
    fileWriter.open("../vex_robot/drawing_data.h");

    // Check if file opened.
    if (!fileWriter.is_open())
    {
        std::cerr << "ERROR: Could not open drawing_data.h for writing.\n";
        return 1;
    }
    else // If file opened successfully, then write the paths data.
    {

        fileWriter << std::fixed << std::setprecision(2); // Keep coords accurate to 2 decimal places.
        fileWriter << "#ifndef DRAWING_DATA_H\n";
        fileWriter << "#define DRAWING_DATA_H\n\n";
        fileWriter << "#define MAX_CONTOURS " << MAX_CONTOURS << "\n\n";
        fileWriter << "#define MAX_COORDS " << MAX_COORDS << "\n\n";
        
        fileWriter << "typedef struct {\n";
        fileWriter << "    int numContours;\n"; // How many contours (shapes) in the image.
        fileWriter << "    int numCoords[MAX_CONTOURS];\n"; // Array of how many coordinates there are per contour.
        fileWriter << "    double coords[MAX_CONTOURS][MAX_COORDS][2];\n"; // Array of coordinates per contour.
        fileWriter << "} CoordData;\n\n";

        // Create and write into CoordData object named coordData.
        // TODO: Name this a different name (so object name is not the same as the type name.)
        fileWriter << "static const CoordData coordData = {\n";
        
        // Fill in numContours with how many contours there are.
        // c++ vector.size() returns the number of elements in the vector.
        fileWriter << "    " << cleanedContours.size() << ",\n";

        // Fill in numCoords with the # of coordinates in each contour
        fileWriter << "    { ";
        for (const auto &shape : cleanedContours) // For contour in contours vector, write its size.
            fileWriter << shape.size() << ", ";
        fileWriter << "},\n";

        // Now to fill in the huge coords array with the actual coordinates of each contour.
        // Loop over # of different contours (shapes) in the image png.
        for (const auto &contour : cleanedContours)
        {
            // Create and start writing into CoordData object named coordData.
            // TODO: Name this a different name (so object name is not the same as the type name.)
            fileWriter << "static const CoordData coordData[] = {\n";

            // Loop over each coordinate in contour.
            for (const auto &p : contour)
            {
                // Scale to mm for whiteabord.
                // Also flip y-axis (origin is bottom-left.)
                double x_mm = p.x * scaleX;
                double y_mm = (image.rows - 1 - p.y) * scaleY;

                // Write coordinate into array.
                fileWriter << "        {" << x_mm << "}, {" << y_mm << "},\n";

                // TEMP for debugging
                // Print coordinates to console.
                std::cout << "  x=" << x_mm
                          << ", y=" << y_mm << '\n';
            }
            fileWriter << "    } },\n";
        }
        // End of writing into coordData object.
        fileWriter << "};\n\n";
    }

    // TEMP for debugging
    // Visually confirm contour tracing worked
    // by creating image of red contours overlayed on original image.
    // Red contours might be jagged but the actual coordinates will not be.
    cv::Mat debug = image.clone();
    cv::drawContours(debug, contours, -1, cv::Scalar(0, 0, 255), 2);
    cv::imwrite("debug_contours.png", debug);

    return 0;
}

/* --- Sample output for square_drawing.png ---
Path:
    x=221.534, y=163.432
    x=77.521, y=163.15
    x=77.521, y=34.0366
    x=221.849, y=34.0366
*/