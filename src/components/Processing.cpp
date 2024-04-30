#include <array>
#include <iostream>
#include "Connector.hpp"
#include <Eigen/Dense>
#include <aes.h>
#include <pcap.h>
#include <hidapi.h>
#include <lsl_cpp.h>
#include <iostream>  //        input and output stream functionality
#include <string>    //          working with strings
#include <fstream>   //         file input and output operations
#include <sstream>   //         string stream processing
#include <vector>    //          dynamic arrays
#include <locale>    //
#include <codecvt>   //         converting between different character encodings
#include <iomanip>   //
#include <modes.h>   //                       ~
#include <filters.h> //                     ~
#include <hex.h>     //                         ~
#include <thread>

void Processing::get_stream() {
    std::cout << "Getting stream" << std::endl;
    
    while (true) {
        try {
            // Resolve the stream of interest
            std::vector<lsl::stream_info> found_streams = lsl::resolve_stream("type", "EEG");
            if (found_streams.empty()) {
                std::cerr << "No streams found." << std::endl;
                throw std::runtime_error("No streams found.");
            }

            // Print the streams
            for (const lsl::stream_info &stream : found_streams) {
                std::cout << "Found stream: " << stream.name() << " (" << stream.source_id() << ")" << std::endl;
            }

            lsl::stream_inlet inlet(found_streams[0]);
            
            while (true) {
                std::vector<float> sample;
                double ts = inlet.pull_sample(sample);
                /*
                    std::cout << "Sample at t=" << ts << ": ";
                    for (float value : sample) {
                        std::cout << value << " ";2
                    }
                    std::cout << std::endl;
                */
                // If sample is not 14 values, skip
                if (sample.size() == 14) {
                    storeData(sample);
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            // Add some handling or break condition here
        }
    }
}

void Processing::storeData(const std::vector<float> &newBuffer) {
    
    try {
        if (newBuffer.size() != 14) {
            std::cout << "Invalid data size" << std::endl;
            return;
        }
        // Add array to 2d array global
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(newBuffer);
        if (data.size() > 500) {
            data.erase(data.begin());
        }

        
        
    } catch (const std::exception &e) {
        std::cout << "Error storing data: " << e.what() << std::endl;
    }
    return;
}
std::vector<std::vector<float>> Processing::getData()
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::vector<float>> dataBuffer = data;
    return dataBuffer;
}

void Processing::callProcessing()
{
    std::vector<std::vector<float>> previous = getData();
    //wait 500ms
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::vector<std::vector<float>> current = getData();

    std::vector<std::vector<float>>  previousProcessed = startProcessing(previous);
    std::vector<std::vector<float>>  currentProcessed = startProcessing(current);

    
    

    // Save previous as csv and previousProcessed as csv
    std::ofstream previousFile;
    previousFile.open("previous.csv");
    for (int i = 0; i < previous.size(); i++) {
        for (int j = 0; j < previous[i].size(); j++) {
            previousFile << previous[i][j] << ",";
        }
        previousFile << std::endl;
    }
    previousFile.close();

    std::ofstream previousProcessedFile;
    previousProcessedFile.open("previousProcessed.csv");
    for (int i = 0; i < previousProcessed.size(); i++) {
        for (int j = 0; j < previousProcessed[i].size(); j++) {
            previousProcessedFile << previousProcessed[i][j] << ",";
        }
        previousProcessedFile << std::endl;
    }
    previousProcessedFile.close();





    


    return;
}

// take in a 2d vector of data
std::vector<std::vector<float>>  Processing::startProcessing(const std::vector<std::vector<float>> &sample)
{
    // Processing the data
    num_observations = sample[0].size(); // Assuming all inner vectors have the same size
    num_dimensions = sample.size();
    Eigen::MatrixXd X(num_dimensions, num_observations);

    for (int j = 0; j < num_dimensions; ++j)
    {
        for (int k = 0; k < num_observations; ++k)
        {
            X(j, k) = static_cast<double>(sample[j][k]);
        }
    }

    Eigen::MatrixXd S(num_dimensions, num_observations);
    Eigen::MatrixXd A(num_dimensions, num_dimensions);
    Eigen::MatrixXd W(num_dimensions, num_dimensions);

    // Get Entropy
    std::vector<float> waveletEntropy = WaveletEntropy(sample);

    //Save Entropy in csv
    std::ofstream waveletEntropyFile;
    waveletEntropyFile.open("waveletEntropy.csv");
    for (int i = 0; i < waveletEntropy.size(); i++) {
        waveletEntropyFile << waveletEntropy[i] << ",";
    }
    waveletEntropyFile.close();



    // Return blank
    std::vector<std::vector<float>> blank;
    return blank;
}

float Processing::ShannonEntropy(const std::vector<float> &vec) {
    float entropy = 0.0;
    int n = vec.size();
    std::vector<float> probabilities(n);

    // Calculate probabilities
    for (int i = 0; i < n; i++) {
        probabilities[i] = std::abs(vec[i]) / n; // Absolute value of the wavelet coefficients
    }

    // Calculate entropy
    for (int i = 0; i < n; i++) {
        if (probabilities[i] > 0.0) {
            entropy -= probabilities[i] * std::log2(probabilities[i]);
        }
    }

    return entropy;
}

std::vector<float> Processing::WaveletEntropy(const std::vector<std::vector<float>> &sample) {
    // Transpose the sample matrix
    int rows = sample.size();
    int cols = sample[0].size();
    std::vector<std::vector<float>> transposed(cols, std::vector<float>(rows));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            transposed[j][i] = sample[i][j];
        }
    }

    // Calculate Wavelet Entropy for each column
    std::vector<float> waveletEntropy(cols);
    for (int i = 0; i < cols; i++) {
        waveletEntropy[i] = ShannonEntropy(transposed[i]);
    }

    return waveletEntropy;
}
