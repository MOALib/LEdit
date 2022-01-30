#include <iostream>
#include "ledit.hpp"


MXPSQL::LEdit::LEdit* leditor = new MXPSQL::LEdit::LEdit();

int main(int argc, char** argv){
    std::cout << "Hello World!" << std::endl;
    std::cout << "This is a test!" << std::endl;

    leditor->initHistory();

    while(true){
        std::string str = leditor->readlineStr();
        leditor->addHistory(str);
        // std::cout << ">";
        // char str = getchar();
        for(int i = 0; i < str.length(); i++){
            std::cout << "# (" << i << ") > " << str[i] << ": " << (int)str[i] << std::endl;
        }

        std::cout << std::endl;
    }

    leditor->deinitHistory();

    delete leditor;

    return 0;
}