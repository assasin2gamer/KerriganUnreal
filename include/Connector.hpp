#ifndef Conn_HPP
#define Conn_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <locale>
#include <codecvt>
#include <iomanip>

#include <Eigen/Dense>
#include <aes.h>
#include <pcap.h>
#include <hidapi.h>
#include <lsl_cpp.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <filesystem>



class EmotivConn
{
public:
    void addDataToGraph(const std::vector<float> &data);
    std::string WideCharToMultiByte(const wchar_t *wideCharString);
    hid_device *get_hid_device(const std::string man_string);
    void Start_Dev();
    int SRATE = 256;
    int READ_SIZE = 32;
    std::string man_string = "Emotiv";
    char delimiter = ',';
    std::vector<CryptoPP::byte> key;

    wchar_t *serialWChar;
    void hid_set_serial(hid_device *dev, const char *serial);

    struct HIDDeviceInfo
    {
        std::string manufacturer;
        int usage;
    };
    float convertEPOC_PLUS(CryptoPP::byte value_1, CryptoPP::byte value_2);

    std::vector<CryptoPP::byte> get_crypto_key(hid_device *device);
    lsl::stream_info get_stream_info();
    bool validate_data(std::string data);
    bool validate_data(int data);
    void swap_positions(std::vector<float> &vec, int a, int b);

    std::vector<float> decode_data(std::vector<CryptoPP::byte> &data, hid_device *dev);

    float convertEPOC_PLUS(const std::string &value_1, const std::string &value_2);
};

class Processing
{
public:
    public:
    void get_stream();
    std::vector<std::vector<float>> ICA_Filtering(Eigen::MatrixXd &X, Eigen::MatrixXd &S, Eigen::MatrixXd &A, Eigen::MatrixXd &W);
    bool analyzeStart;
    Eigen::VectorXd infomaxExtended(const Eigen::VectorXd &channelX, const Eigen::VectorXd &channelS, const Eigen::MatrixXd &A, const Eigen::MatrixXd &W);
    void storeData(const std::vector<float> &newBuffer);
    // 2d Vector to store the data
    std::vector<std::vector<float>> data = std::vector<std::vector<float>>(500, std::vector<float>(14));
    std::vector<std::vector<float>> getData();
    void callProcessing();
    std::vector<std::vector<float>>  startProcessing(const std::vector<std::vector<float>> &sample);
    std::vector<float> WaveletEntropy(const std::vector<std::vector<float>> &sample);
    float ShannonEntropy(const std::vector<float> &vec);

    std::vector<std::vector<float>> fastICA(Eigen::MatrixXd &X, Eigen::MatrixXd &S, Eigen::MatrixXd &A, Eigen::MatrixXd &W);
    std::vector<lsl::stream_info> found_streams;
    int num_observations; // Assuming these are class members
    int num_dimensions;   // Assuming these are class members
    std::mutex mtx;


};
#endif // Emotiv_HPP


#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <filesystem>

namespace fs = std::filesystem;


class MainWindow : public sf::RenderWindow
{
public:
    MainWindow(int width, int height, const std::string &title);
    void Run();
    void HandleEvents();
    void DrawInitial();
    void Connect();
    void Analyze();
    void drawButtons();
    Processing* processing = new Processing();
    void ProcessChunk(Processing& processing);
    bool isMouseOverButton(const sf::RectangleShape &button);
    bool graphOn;
private:
    sf::RectangleShape navBar;
    sf::RectangleShape sideBar;
    sf::RectangleShape bottomBar;
    sf::Font font;
};

#endif // MAIN_WINDOW_HPP