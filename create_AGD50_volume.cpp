
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

int main()
{
    // Input and output folders
    const string inputFolder = "AGD50";
    const string outputFolder = "output_AGD50/volume";

    fs::create_directories(outputFolder);

    const int startIndex = 0;
    const int endIndex = 500;

    vector<Mat> volumeSlices;

    int width = 0;
    int height = 0;
    int imageType = -1;

    // Read all 501 TIFF images in order
    for (int i = startIndex; i <= endIndex; ++i)
    {
        ostringstream number;
        number << setw(6) << setfill('0') << i;
        string id = number.str();

        string filename =
            inputFolder + "/full_AGD_50_" + id + ".tiff";

        Mat img = imread(filename, IMREAD_UNCHANGED);

        if (img.empty())
        {
            cerr << "ERROR: Cannot read " << filename << endl;
            return 1;
        }

        // Initialize image properties
        if (i == startIndex)
        {
            width = img.cols;
            height = img.rows;
            imageType = img.type();

            cout << "Image dimensions: "
                 << width << " x " << height << endl;
            cout << "OpenCV image type: "
                 << imageType << endl;
        }

        // Verify consistent dimensions and image type
        if (img.cols != width ||
            img.rows != height ||
            img.type() != imageType)
        {
            cerr << "ERROR: Inconsistent image: "
                 << filename << endl;
            return 1;
        }

        // Convert to float32 for consistent volume storage
        Mat floatImage;
        img.convertTo(floatImage, CV_32F);

        // Store the slice in numerical order
        volumeSlices.push_back(floatImage.clone());

        cout << "Loaded image " << id << endl;
    }

    const int depth = static_cast<int>(volumeSlices.size());

    if (depth != 501)
    {
        cerr << "ERROR: Expected 501 images, got "
             << depth << endl;
        return 1;
    }

    // Save the 3D volume as float32 RAW
    const string rawPath =
        outputFolder + "/AGD50_volume.raw";

    ofstream rawFile(rawPath, ios::binary);

    if (!rawFile)
    {
        cerr << "ERROR: Cannot create RAW file." << endl;
        return 1;
    }

    // Storage order: Z, Y, X
    for (const Mat& slice : volumeSlices)
    {
        for (int y = 0; y < height; ++y)
        {
            rawFile.write(
                reinterpret_cast<const char*>(
                    slice.ptr<float>(y)),
                static_cast<streamsize>(
                    width * sizeof(float))
            );

            if (!rawFile)
            {
                cerr << "ERROR: Failed writing RAW volume."
                     << endl;
                return 1;
            }
        }
    }

    rawFile.close();

    // Write metadata for Python / Google Colab
    const string metadataPath =
        outputFolder + "/metadata.txt";

    ofstream metaFile(metadataPath);

    if (!metaFile)
    {
        cerr << "ERROR: Cannot create metadata file." << endl;
        return 1;
    }

    metaFile << "depth=" << depth << "\n";
    metaFile << "height=" << height << "\n";
    metaFile << "width=" << width << "\n";
    metaFile << "dtype=float32\n";
    metaFile << "axis_order=ZYX\n";
    metaFile << "first_index=0\n";
    metaFile << "last_index=500\n";
    metaFile << "byte_order=native\n";

    metaFile.close();

    cout << "\nVolume created successfully!" << endl;
    cout << "Shape (Z,Y,X): ("
         << depth << ", " << height << ", "
         << width << ")" << endl;
    cout << "RAW file: " << rawPath << endl;
    cout << "Metadata: " << metadataPath << endl;

    return 0;
}

