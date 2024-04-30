#include "Connector.hpp"
#include <windows.h>
#include <lsl_cpp.h>
#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <future>
#include <array>
#include <mutex>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

// Function to start EmotivConn on a separate thread


// Entry point for Windows GUI applications
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MainWindow mainWindow(300, 600, "EEG Data Visualizer"); 

    
    

    

    // Main loop
    while (mainWindow.isOpen()) {
        mainWindow.clear(sf::Color::White);
        mainWindow.HandleEvents();
        mainWindow.drawButtons();
        mainWindow.display();
    }

   

    return 0;
}