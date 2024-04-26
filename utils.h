#ifndef utils_H
#define utils_H
#include <iostream>
#include <string>
#include <vector>

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

template<typename T>
T getValidRangeInput(const std::string& prompt, const T& minValue, const T& maxValue) {
    T value;
    do {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < minValue || value > maxValue) {
            std::cout << "Invalid choice, please enter a number between " << minValue << " and " << maxValue << "!" << std::endl;
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

// Displays paginated list of items
template<typename T>
void displayItemPage(const std::vector<T>& items, int page, int pageSize, std::string menuText) {
    int startIndex = (page - 1) * pageSize;
    int endIndex = page * pageSize;

    /*
    - However, if endIndex, is greater than the size of the vector,
    then set endIndex to the size of the vector to avoid going over.
    */
    if (endIndex > items.size()) {
        endIndex = items.size();
    }

    // Display menu text and the items
    std::cout << menuText << std::endl;
    for (int i = startIndex; i < endIndex; ++i) {
        std::cout << i + 1 << ". " << items[i] << std::endl;
    }
}


template<typename T>
T selectPaginatedItems(std::vector<T>& items, int pageSize, std::string menuName, std::string prompt) {

    int page = 1;
	const int maxPage = static_cast<int>(std::ceil(static_cast<float>(items.size()) / pageSize));
	do {
        // Construct text for the menu and render the menu for a particular page
		std::string itemMenuText = menuName + " (Page " + std::to_string(page) + " / " + std::to_string(maxPage) + ")";
		displayItemPage(items, page, pageSize, itemMenuText); 

		// Prompt for menu selection
        int menuChoice;

        std::cout << prompt << " (0 to exit, -1 for previous page, -2 for next page): ";
		std::cin >> menuChoice;

		// Check if the input is valid integer
		if (std::cin.fail()) {
			std::cout << "Invalid input. Please enter a valid number." << std::endl;
			std::cin.clear();
			std::cin.ignore(64, '\n');
		}
		else if (menuChoice == 0) {
            // Exit loop and return instance created by default constructor
            return T();
			
		}
		else if (menuChoice == -1) {
			// Move to previous page
			page -= 1;
			if (page < 1) {
				page = 1;
			}
		}
		else if (menuChoice == -2) {
			// Move to next page
			page += 1;
			if (page > maxPage) {
				page = maxPage;
			}
        } else if (menuChoice < 1 || menuChoice > items.size()) {
            // If out of range value that isn't 0, -1, or -2
            std::cout << "Please enter a list value between 1 and " << items.size() << "!" << std::endl;
        } else {
            // Else, a list value, so index it from items and return it
            T item = items[menuChoice - 1];
            return item;
		}

        std::cout << std::endl;
	} while (true);
}


// Controls paginated item menu, for viewing purposes only, rather than selecting data
template<typename T>
void navigatePaginatedItems(const std::vector<T>& items, int pageSize, std::string menuName) {
    int page = 1;
    const int maxPage = static_cast<int>(std::ceil(static_cast<float>(items.size()) / pageSize));
    do {
        std::string itemMenuText = menuName + " (Page " + std::to_string(page) + " / " + std::to_string(maxPage) + ")";
        displayItemPage(items, page, pageSize, itemMenuText);


        // Prompt for menu selection
        int menuChoice;
        std::cout << "Select number to navigate (0 to exit, -1 for previous page, -2 for next page): ";
        std::cin >> menuChoice;

        // Check if the input is valid
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
            std::cin.clear();
            std::cin.ignore(64, '\n');
        }
        else if (menuChoice == 0) {
            break; // Exit the loop
        }
        else if (menuChoice == -1) {
            // Move to previous page
            page -= 1;
            if (page < 1) {
                page = 1;
            }
        }
        else if (menuChoice == -2) {
            // Move to next page
            page += 1;
            if (page > maxPage) {
                page = maxPage;
            }
        } else {
            std::cout << "Invalid numerical choice. Please try again!" << std::endl;
        }

        std::cout << std::endl;
    } while (page <= maxPage);
}

char promptYesOrNo(std::string prompt) {
    char choice;
    do {
        std::cout << prompt;
        std::cin >> choice;

        // Convert the choice to lowercase
        choice = std::tolower(choice);

        // Check if the choice is valid
        if (choice != 'y' && choice != 'n') {
            std::cout << "Invalid choice. Please enter 'y' or 'n'." << std::endl;
        }
    } while (choice != 'y' && choice != 'n');

    return choice;
}



#endif