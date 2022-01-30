#include <iostream>
#include "ledit.hpp"

int main(int argc, char** argv){
    std::cout << "Hello World!" << std::endl;
    std::cout << "This is a test!" << std::endl;
    std::cout << "Type X to quit" << std::endl;

    while(true){
        std::cout << ">> ";
        char c = std::cin.get();
        char c2 = std::cin.get();

        if(c == 'X'){
            break;
        }

        std::cout << "Char1: " << (c - '0') << std::endl;
        std::cout << "Char2: " << (c2 - '0') << std::endl << std::endl;

    }

    return 0;
}