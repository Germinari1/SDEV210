#ifndef getValidNumericInput_H
#define getValidNumericInput_H
#include <iostream>
#include <string>

template<typename T>
T getValidNumericInput(const std::string prompt) {
    T value;
    do {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cout << "Invalid choice, please enter a number!" << std::endl;
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
        }
        else {
            std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
            break; // Exit the loop if input is valid
        }
    } while (true);
    return value;
}

#endif