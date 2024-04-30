#include <iostream>
#include "Connector.hpp"
#include <hidapi.h>
#include <lsl_cpp.h>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <aes.h>
#include <modes.h>
#include <filters.h>
#include <hex.h>
#include <thread>
#include <vector>
#include <string>

std::string EmotivConn::WideCharToMultiByte(const wchar_t *wideCharString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wideCharString);
}

hid_device *EmotivConn::get_hid_device(const std::string man_string)
{
    hid_init();
    hid_device_info *devices = hid_enumerate(0, 0);
    hid_device_info *curDevice = devices;
    while (curDevice)
    {
        std::string manufacturer = WideCharToMultiByte(curDevice->manufacturer_string);
        // std::cout << "Manufacturer: " << manufacturer << std::endl;
        if (manufacturer == man_string)
        {
            // std::cout << "Found Emotiv Device\n";
            hid_device *device = hid_open_path(curDevice->path);
            hid_free_enumeration(devices);
            
            wchar_t *serialWChar = curDevice->serial_number; // Retrieve serial number
          
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string serial = converter.to_bytes(serialWChar);

            std::cout << "Serial: " << serial << std::endl;
           
            

            if (device == nullptr || serial.size() != 16)
            {
                curDevice = curDevice->next;
            }
            else{
                if (serial.size() == 16){
                    this->serialWChar = serialWChar;
                    return device; // Return device
                }
                
            }
            
        }
        curDevice = curDevice->next;
        
    }
    return nullptr;
}

void EmotivConn::swap_positions(std::vector<float> &vec, int a, int b)
{
    float temp = vec[a];
    vec[a] = vec[b];
    vec[b] = temp;
}

float EmotivConn::convertEPOC_PLUS(const std::string &value_1, const std::string &value_2)
{
    int int_value_1 = std::stoi(value_1);
    int int_value_2 = std::stoi(value_2);
    double edk_value = ((int_value_1 * 0.128205128205129) + 4201.02564096001) + ((int_value_2 - 128) * 32.82051289);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(8) << edk_value;
    return std::stof(ss.str());
}

std::vector<CryptoPP::byte> EmotivConn::get_crypto_key(hid_device *dev)
{

    // print global serialWChar
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string serial = converter.to_bytes(this->serialWChar);

    // std::cout << "Serial: " << serial << std::endl;

    // convert to wstring
    std::wstring wserial = std::wstring(this->serialWChar);
    // convert to vector
    std::vector<CryptoPP::byte> sn(wserial.begin(), wserial.end());

    // Error handling
    if (sn.size() != 16)
    {
        std::cerr << "Invalid serial number size: " << sn.size() << std::endl;
        return {};
    }

    return sn;
}

float EmotivConn::convertEPOC_PLUS(CryptoPP::byte value_1, CryptoPP::byte value_2)
{
    double edk_value = ((value_1 * 0.128205128205129) + 4201.02564096001) + ((value_2 - 128) * 32.82051289);
    return static_cast<float>(edk_value);
}
void EmotivConn::addDataToGraph(const std::vector<float> &data)
{
}
std::vector<float> EmotivConn::decode_data(std::vector<CryptoPP::byte> &data, hid_device *dev)
{
    std::string delimiter = ","; // Define the delimiter

    // If key is not valid, try getting a new key
    
        
    
    if (key.size() != 16)
    {
        std::cerr << "Invalid key size: " << key.size() << std::endl;
        return {};
    }

    
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = {0}; // IV is just a formality so just set to 0

    // Set up the encryption object with ECB mode
    try{
        CryptoPP::AES::Encryption aesEncryption(key.data(), key.size());
        CryptoPP::ECB_Mode_ExternalCipher::Encryption ecbEncryption(aesEncryption, iv);
    }
    catch(const CryptoPP::Exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return {};
    }
    

    // std::cout << "113 - break" << std::endl;
    //  Return if the key is invalid
    if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH)
    {
        return {};
    }

    // Set up decryption object
    CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption decryption((const CryptoPP::byte *)key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    std::vector<CryptoPP::byte> decryptedData(data.size());

    // Decrypt the data
    decryption.ProcessData(decryptedData.data(), data.data(), data.size());

    // Print the decrypted data

    // std::cout << "Decrypted Data: ";
    /*
    for (auto &el : decryptedData)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)el << " ";
    }
    std::cout << std::endl;
*/
    // Process decrypted data
    std::string packet_data;
    std::vector<float> packet_floats;

   

    // Index 2 to 15 with a step of 2
    for (size_t i = 2; i < 16; i += 2)
    {
        packet_data += std::to_string(convertEPOC_PLUS(decryptedData[i], decryptedData[i + 1])) + delimiter;
    }

    // Index 18 to the end of the data with a step of 2
    for (size_t i = 18; i < decryptedData.size(); i += 2)
    {
        packet_data += std::to_string(convertEPOC_PLUS(decryptedData[i], decryptedData[i + 1])) + delimiter;
    }
    packet_data.pop_back(); // Remove the trailing delimiter

    // print packet data

    // Split the string into tokens using the delimiter
    size_t pos = 0;
    while ((pos = packet_data.find(delimiter)) != std::string::npos)
    {
        std::string token = packet_data.substr(0, pos);
        packet_floats.push_back(std::stof(token));
        packet_data.erase(0, pos + delimiter.length());
    }

    // Process the remaining part of the string after the loop
    if (!packet_data.empty())
    {
        packet_floats.push_back(std::stof(packet_data));
    }

    std::swap(packet_data[0], packet_data[2]);

    // Swap positions of AF4 and F4
    std::swap(packet_data[11], packet_data[13]);

    // Swap positions of F7 and FC5
    std::swap(packet_data[1], packet_data[3]);

    // Swap positions of FC6 and F8
    std::swap(packet_data[10], packet_data[12]);
    // print the packet floats\

    /*
    std::cout << "Packet Floats: ";

    for (auto &el : packet_floats)
    {
        std::cout << el << " ";
    }
    std::cout << std::endl;
    */
    // Check if the data is valid
    if (packet_floats.size() != 14)
    {
        std::cerr << "Invalid data size: " << packet_floats.size() << std::endl;
        return {};
    }
    else
    {
        // graphWindowRef.addData(packet_floats);
        return packet_floats;
    }
}

lsl::stream_info EmotivConn::get_stream_info()
{
    const int SRATE = 256;
    const std::vector<std::string> ch_names = {"AF3", "F7", "F3", "FC5", "T7", "P7", "O1", "O2", "P8", "T8", "FC6", "F4", "F8", "AF4"};
    const int n_channels = ch_names.size();

    lsl::stream_info info("Epoc X", "EEG", n_channels, SRATE, lsl::cf_float32);
    lsl::xml_element chns = info.desc().append_child("channels");
    for (const auto &label : ch_names)
    {
        lsl::xml_element ch = chns.append_child("channel");
        ch.append_child_value("label", label.c_str());
        ch.append_child_value("unit", "microvolts");
        ch.append_child_value("type", "EEG");
        ch.append_child_value("scaling_factor", "1");
    }

    lsl::xml_element cap = info.desc().append_child("cap");
    cap.append_child_value("name", "easycap-M1");
    cap.append_child_value("labelscheme", "10-20");

    // std::cout << "StreamInfo: " << info.as_xml() << "\n";

    // Error handling for stream info
    if (info.channel_count() != n_channels)
    {
        // std::cerr << "Invalid channel count: " << info.channel_count() << std::endl;
        return {};
    }

    return info;
}

bool EmotivConn::validate_data(std::string data)
{
    return data.size() == 32;
}

bool EmotivConn::validate_data(int data)
{

    // std::cout << "Data verifying: " << data << std::endl;
    return data == 32;
}

void EmotivConn::Start_Dev()
{

    const int SRATE = 256;
    const int READ_SIZE = 32;
    const std::string man_string = "Emotiv";
    const char delimiter = ',';

    std::cout << "Emotiv Started\n";
    while (true)
    {
        lsl::stream_outlet outlet(get_stream_info());
        hid_device *dev = get_hid_device(man_string);
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    
       while (dev == nullptr)
       {
              std::cout << "Device not found\n";
              dev = get_hid_device(man_string);
              std::string serial = converter.to_bytes(serialWChar);
              if (serial.size() != 16){
                  std::cout << "Serial number not valid\n";
                  dev == nullptr;
              }
              else{
                  std::cout << "Serial number valid\n";
              }
       }
       key = get_crypto_key(dev);
    
       

        std::cout << "Device: " << dev << std::endl;

        if (dev == nullptr)
        {
            std::cout << "Device not found\n";
        }
        else
        {
            while (true)
            {
                // Try catch block for reading data

                // std::cout << "Reading data was run";

                try
                {

                    // std::cout << "Reading data\n";
                    std::vector<unsigned char> dataBuffer(READ_SIZE);

                    int numBytesRead = hid_read(dev, dataBuffer.data(), READ_SIZE);

                    // Print the data buffer
                    // std::cout << "Data Buffer: ";
                   /* for (auto &el : dataBuffer)
                    {
                        // std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)el << " ";
                    }*/
                    // std::cout << std::endl;

                    //if (validate_data(numBytesRead))
                    //{
                        // std::cout << "NumBytesRead: " << numBytesRead << std::endl;

                        std::vector<float> decodedDataBuffer = decode_data(dataBuffer, dev);
                        // Error handling for decode data if the data is not valid

                        if (decodedDataBuffer.size() == 0)
                        {
                            std::cout << "Data not valid\n";
                            throw(decodedDataBuffer);
                            break;
                        }
                        else
                        {

                            outlet.push_sample(decodedDataBuffer);
                        }
                     //}
                    /*else
                    {
                        std::cout << "Data not valid\n";
                        break;
                    }*/
                }
                catch (std::vector<float> decodedDataBuffer)
                {
                    std::cout << "Error reading data\n";

                    // Print the decoded data buffer

                    for (auto &el : decodedDataBuffer)
                    {
                        std::cout << el << " ";
                    }
                    std::cout << std::endl;
                    break;
                }
            }

            std::cout << "Emotiv Terminating\n";
            // wait for input to close
            std::cin.get();
            hid_close(dev);
            hid_exit();
        }
    }
}
