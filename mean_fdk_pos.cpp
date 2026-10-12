
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdint>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

int main()
{
    // Input folders
    const string folder1 = "pos1";
    const string folder2 = "pos2";
    const string folder3 = "pos3";

    // Output folders
    const string meanFolder = "output/mean";
    const string volumeFolder = "output/volume";

    fs::create_directories(meanFolder);
    fs::create_directories(volumeFolder);

    // Image indices: 000000 to 000500
    const int startIndex = 0;
    const int endIndex = 500;

    vector<Mat> volumeSlices;

    int width = 0;
    int height = 0;
    int originalType = -1;

    // Process all images in numerical order
    for (int i = startIndex; i <= endIndex; ++i)
    {
        ostringstream number;
        number << setw(6) << setfill('0') << i;
        const string id = number.str();

        const string file1 =
            folder1 + "/fdk_pos1_" + id + ".tiff";
        const string file2 =
            folder2 + "/fdk_pos2_" + id + ".tiff";
        const string file3 =
            folder3 + "/fdk_pos3_" + id + ".tiff";

        // Read images without changing their original type
        Mat img1 = imread(file1, IMREAD_UNCHANGED);
        Mat img2 = imread(file2, IMREAD_UNCHANGED);
        Mat img3 = imread(file3, IMREAD_UNCHANGED);

        if (img1.empty() || img2.empty() || img3.empty())
        {
            cerr << "ERROR: Missing or unreadable image at index "
                 << id << endl;
            return 1;
        }

        // Verify dimensions and types
        if (img1.size() != img2.size() ||
            img1.size() != img3.size() ||
            img1.type() != img2.type() ||
            img1.type() != img3.type())
        {
            cerr << "ERROR: Different dimensions or types at index "
                 << id << endl;
            return 1;
        }

        // Check consistency between all slices
        if (i == startIndex)
        {
            width = img1.cols;
            height = img1.rows;
            originalType = img1.type();

            cout << "Image dimensions: "
                 << width << " x " << height << endl;
            cout << "Input OpenCV type: "
                 << originalType << endl;
        }
        else if (img1.cols != width ||
                 img1.rows != height ||
                 img1.type() != originalType)
        {
            cerr << "ERROR: Inconsistent image at index "
                 << id << endl;
            return 1;
        }

        // Convert to float for safe averaging
        Mat f1, f2, f3;
        img1.convertTo(f1, CV_32F);
        img2.convertTo(f2, CV_32F);
        img3.convertTo(f3, CV_32F);

        // Pixel-wise mean of the three positions
        Mat meanFloat;
        add(f1, f2, meanFloat);
        add(meanFloat, f3, meanFloat);
        meanFloat /= 3.0f;

        // Convert back to the original image type
        Mat result;
        meanFloat.convertTo(result, originalType);

        // Save the individual mean TIFF
        const string outputFile =
            meanFolder + "/fdk_mean_" + id + ".tiff";

        if (!imwrite(outputFile, result))
        {
            cerr << "ERROR: Cannot save " << outputFile << endl;
            return 1;
        }

        // Store a copy as the next slice of the 3D volume
        volumeSlices.push_back(result.clone());

        cout << "Processed image " << id << endl;
    }


    // --------------------------------------------------
    // Save 3D volume as RAW float32
    // Volume axis order: Z, Y, X
    // --------------------------------------------------

    const int depth = static_cast<int>(volumeSlices.size());

    for (const Mat& slice : volumeSlices)
    {
        if (slice.type() != CV_32FC1)
        {
            cerr << "ERROR: Expected 32-bit float grayscale "
                 << "images (CV_32FC1)." << endl;
            cerr << "Actual type: " << slice.type() << endl;
            return 1;
        }
    }

    const string rawPath =
        volumeFolder + "/mean_volume.raw";

    ofstream rawFile(rawPath, ios::binary);

    if (!rawFile)
    {
        cerr << "ERROR: Cannot create RAW file." << endl;
        return 1;
    }

    // Write slices in order: Z, then Y, then X
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
                cerr << "ERROR: Failed while writing RAW data."
                     << endl;
                return 1;
            }
        }
    }

    rawFile.close();

    // Save metadata
    const string metadataPath =
        volumeFolder + "/metadata.txt";

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

    cout << "\n3D volume saved successfully!" << endl;
    cout << "Dimensions (Z,Y,X): "
         << depth << ", " << height << ", " << width << endl;
    cout << "RAW file: " << rawPath << endl;
    cout << "Metadata: " << metadataPath << endl;


    return 0;
}

