
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <cstdint>

// Volume dimensions
constexpr size_t Z = 501;
constexpr size_t Y = 501;
constexpr size_t X = 501;
constexpr size_t N = Z * Y * X;

// --------------------------------------------------
// 2. Input and output filenames
// --------------------------------------------------
const std::string fileAGD50 =
            "/home/nboudjem/Downloads/Walnut1/Reconstructions/output_AGD50/volume_AGD50/AGD50_volume.raw";

const std::string fileFDK =
            "/home/nboudjem/Downloads/Walnut1/Reconstructions/output_FDK_pos/volume_FDK_pos/mean_volume.raw";

const std::string fileAGD50Norm =
            "/home/nboudjem/Downloads/Walnut1/Reconstructions/AGD50_normalized.raw";

const std::string fileFDKNorm =
            "/home/nboudjem/Downloads/Walnut1/Reconstructions/FDK_normalized.raw";

const std::string fileResidual =
            "/home/nboudjem/Downloads/Walnut1/Reconstructions/residual.raw";


// Load exactly N float32 values from a RAW file
std::vector<float> loadRaw(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot open file: " + filename);
    }

    const std::streamsize fileSize = file.tellg();
    const std::streamsize expectedSize =
        static_cast<std::streamsize>(N * sizeof(float));

    if (fileSize != expectedSize) {
        throw std::runtime_error(
            "Incorrect file size: " + filename +
            "\nExpected " + std::to_string(expectedSize) +
            " bytes, got " + std::to_string(fileSize) +
            " bytes.\nCheck the volume dimensions and data type.");
    }

    file.seekg(0, std::ios::beg);

    std::vector<float> volume(N);

    if (!file.read(
            reinterpret_cast<char*>(volume.data()),
            expectedSize)) {
        throw std::runtime_error(
            "Incomplete read: " + filename);
    }

    file.close();

    for (size_t i = 0; i < N; ++i) {
        if (!std::isfinite(volume[i])) {
            throw std::runtime_error(
                "NaN or Inf detected in: " + filename);
        }
    }

    std::cout << "Loaded: " << filename << '\n';

    return volume;
}

// Save N float32 values to a RAW file
void saveRaw(
    const std::string& filename,
    const std::vector<float>& volume)
{
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot create output file: " + filename);
    }

    file.write(
        reinterpret_cast<const char*>(volume.data()),
        static_cast<std::streamsize>(
            volume.size() * sizeof(float)));

    if (!file) {
        throw std::runtime_error(
            "Error writing output file: " + filename);
    }

    file.close();

    std::cout << "Saved: " << filename << '\n';
}

// Normalize a volume by its own maximum
float normalizeByOwnMaximum(std::vector<float>& volume)
{
    const float maximum =
        *std::max_element(volume.begin(), volume.end());

    if (!std::isfinite(maximum) || maximum <= 0.0f) {
        throw std::runtime_error(
            "Volume maximum must be finite and positive.");
    }

    for (float& value : volume) {
        value /= maximum;
    }

    return maximum;
}

int main()
{
    try {
        std::cout << "Volume dimensions: "
                  << Z << " x " << Y << " x " << X << '\n';

        std::cout << "Expected input size: "
                  << N * sizeof(float) << " bytes\n\n";

        // 1. Load the original volumes
        std::vector<float> volumeAGD50 = loadRaw(fileAGD50);
        std::vector<float> volumeFDK   = loadRaw(fileFDK);

        // 2. Normalize each volume independently
        const float maxAGD50 =
            normalizeByOwnMaximum(volumeAGD50);

        const float maxFDK =
            normalizeByOwnMaximum(volumeFDK);

        std::cout << "\nOriginal AGD50 maximum: "
                  << maxAGD50 << '\n';

        std::cout << "Original FDK maximum: "
                  << maxFDK << '\n';

        std::cout << "AGD50 normalized maximum: "
                  << *std::max_element(
                         volumeAGD50.begin(),
                         volumeAGD50.end())
                  << '\n';

        std::cout << "FDK normalized maximum: "
                  << *std::max_element(
                         volumeFDK.begin(),
                         volumeFDK.end())
                  << '\n';

        // 3. Save the independently normalized volumes
        saveRaw(fileAGD50Norm, volumeAGD50);
        saveRaw(fileFDKNorm, volumeFDK);

        // 4. Compute residual:
        //    residual = normalized AGD50 - normalized FDK
        std::vector<float> residual(N);

        float residualMin = std::numeric_limits<float>::max();
        float residualMax = std::numeric_limits<float>::lowest();
        double sumSquared = 0.0;

        for (size_t i = 0; i < N; ++i) {
            residual[i] = volumeAGD50[i] - volumeFDK[i];

            residualMin = std::min(residualMin, residual[i]);
            residualMax = std::max(residualMax, residual[i]);

            sumSquared +=
                static_cast<double>(residual[i]) * residual[i];
        }

        const double rmse =
            std::sqrt(sumSquared / static_cast<double>(N));

        // 5. Save the residual without further normalization
        saveRaw(fileResidual, residual);

        std::cout << "\nResidual statistics:\n";
        std::cout << "Minimum: " << residualMin << '\n';
        std::cout << "Maximum: " << residualMax << '\n';
        std::cout << "RMSE:    " << rmse << '\n';

        std::cout << "\nProcessing completed successfully.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << '\n';
        return 1;
    }

    return 0;
}