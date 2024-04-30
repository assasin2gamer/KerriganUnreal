#include <Connector.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <array>
#include <string>
#include <map>
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
#include <filesystem>
#include <Connector.hpp>

namespace fs = std::filesystem;

MainWindow::MainWindow(int width, int height, const std::string &title)
    : sf::RenderWindow(sf::VideoMode(width, height), title, sf::Style::Default)
{
    fs::path currentDir = fs::current_path();
    std::string fontPath = (currentDir / "Resources" / "arial.ttf").string();

    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Error loading font" << std::endl;
        return;
    }
}

void MainWindow::drawButtons()
{
    // Create a rectangle shape for the button
    sf::RectangleShape button(sf::Vector2f(200, 50));
    button.setFillColor(sf::Color::Green);

    // Create a text object for the button
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    // Draw the buttons
    for (int i = 0; i < 3; ++i)
    {
        button.setPosition(50, 50 + i * 100);
        text.setPosition(60, 60 + i * 100);

        // Set button text based on its index
        switch (i)
        {
        case 0:
            text.setString("Connect");
            break;
        case 1:
            text.setString("ProcessChunk");
            break;
        case 2:
            text.setString("Start Processing");
            break;
        }

        // Check if the mouse is over the button
        if (isMouseOverButton(button))
        {
            // Make the button slightly darker
            button.setFillColor(sf::Color(100, 255, 100)); // Example darker color
        }
        else
        {
            button.setFillColor(sf::Color::Green);
        }

        // Draw the button and text
        draw(button);
        draw(text);
    }
}

bool MainWindow::isMouseOverButton(const sf::RectangleShape &button)
{
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*this));
    sf::FloatRect buttonBounds = button.getGlobalBounds();
    return buttonBounds.contains(mousePos);
}

void MainWindow::HandleEvents()
{
    sf::Event event;
    while (pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            close();
        }
        // Handle mouse click events
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*this));

                // Check if the mouse is over the Connect button
                sf::FloatRect connectButtonBounds = sf::FloatRect(50, 50, 200, 50);
                if (connectButtonBounds.contains(mousePos))
                {
                    Connect();
                }

                // Check if the mouse is over the Disconnect button
                sf::FloatRect disconnectButtonBounds = sf::FloatRect(50, 150, 200, 50);
                if (disconnectButtonBounds.contains(mousePos))
                {
                    ProcessChunk(*processing);
                }

                // Check if the mouse is over the Start Processing button
                sf::FloatRect analyzeButtonBounds = sf::FloatRect(50, 250, 200, 50);
                if (analyzeButtonBounds.contains(mousePos))
                {
                    Analyze();
                }
            }
        }
    }
}

void MainWindow::Connect()
{
    EmotivConn* emotivConn = new EmotivConn(); // Dynamically allocate on the heap
    // If crashes just ignore thread
    std::thread emotivConnThread(&EmotivConn::Start_Dev, emotivConn);
    emotivConnThread.detach();
   

}

void MainWindow::ProcessChunk(Processing& processing)
{
    processing.callProcessing();

}

void MainWindow::Analyze()
{
    std::thread processingThread(&Processing::get_stream, processing);
    processingThread.detach();

}