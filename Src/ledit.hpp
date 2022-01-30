#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstdio>
#include <sstream>
#include <exception>
#include <cstring>

/**
 * @name LEdit
 * @brief Minimal line editor library, in a reusable class.
 * 
 * @file ledit.hpp
 * 
 * @copyright MIT License
 * 
 */

#ifndef MXPSQL_LEdit_HPP
#define MXPSQL_LEdit_HPP

// check for c++ 17
#if defined(__cplusplus)
    #if __cplusplus < 201703L
        #error "LEdit requires C++17"
    #endif
#else
    #error "LEdit requires a C++ compiler"
#endif

#if __has_include(<unistd.h>) /* unix */
    #include <unistd.h>
    #define MXPSQL_LEdit_Unix
#elif __has_include(<Windows.h>) /* windows */
    #include <Windows.h>
    #define MXPSQL_LEdit_Windows
    #define popen _popen
#endif

namespace MXPSQL::LEdit{
    // internal tools
    namespace Internal{
    }

    /**
     * @brief Ledit class, for line editing.
     * 
     */
    class LEdit{
    private:
        std::string deafultPrompt;
        std::string historyPrompt = ">> ";
        std::vector<std::string>* history = nullptr;
        void commonInit();
        void commonDeinit();
    public:
        // constructor
        LEdit();
        LEdit(const char* prompt);
        
        // destructor
        ~LEdit();

        // readline function
        char* readline(const char* prompt, bool addToHistory, std::ostream& out, std::istream& in);
        std::string readlineStr(const char* prompt, bool addToHistory, std::ostream& out, std::istream& in);

        // deafult prompts set and gets
        void setDefaultPrompt(const char* prompt);
        void setDefaultPrompt(const std::string& prompt);
        std::string getDefaultPromptStr();
        char* getDefaultPrompt();

        // history function
        void initHistory();
        void deinitHistory();
        bool isHistoryInit();
        void addHistory(const char* str);
        void addHistory(const std::string& str);
        void clearHistory();
        void setHistory(std::vector<std::string>* history);
        std::vector<std::string>* getHistory();
        int saveHistory(const char* filename);
        int loadHistory(const char* filename);

        // operator overloads
        friend std::ostream& operator<<(std::ostream& out, const LEdit& le);
        friend std::istream& operator>>(std::istream& in, LEdit& le);
        friend bool operator==(const LEdit& le1, const LEdit& le2);
        friend bool operator!=(const LEdit& le1, const LEdit& le2);
        explicit operator bool();

    };

#if !defined(MXPSQL_LEdit_CUSTOM_IMPL)

    // LEdit class impl

    // constructors
    /**
     * @brief Construct a new LEdit::LEdit object with the default prompt
     * 
     */
    inline LEdit::LEdit(){
        this->deafultPrompt = "> ";
        commonInit();
    }

    /**
     * @brief Construct a new LEdit::LEdit object with your prompt
     * 
     * @param prompt your default prompt
     */
    inline LEdit::LEdit(const char* prompt) : deafultPrompt(prompt){
        commonInit();
    }

    // destructor
    /**
     * @brief Destroy the LEdit::LEdit object and deinitialize the history along with it
     * 
     */
    LEdit::~LEdit(){
        commonDeinit();
    }

    // common stuff

    /**
     * @brief common Initialization function for the LEdit class
     * 
     */
    void LEdit::commonInit(){
    }

    /**
     * @brief common Deinitialization function for the LEdit class
     * 
     */
    void LEdit::commonDeinit(){
        this->deinitHistory();
    }

    // line reading
    /**
     * @brief read line with the prompt and add history automatically if you want
     * 
     * @param prompt your prompt, if NULL, the default prompt
     * @param addToHistory if true, add the line to history
     * @param out the prompt output stream
     * @param in the prompt input stream
     * @return char* outputs a C-style string
     */
    char* LEdit::readline(const char* prompt = NULL, bool addToHistory = false, std::ostream& out = std::cout, std::istream& in = std::cin){
        std::string strLine = this->readlineStr(prompt, addToHistory, out, in);
        char* line = new char[strLine.length() + 1];

        strcpy(line, strLine.c_str());
        line[strLine.length()] = '\0';

        return line;
    }

    /**
     * @brief readline with the prompt and add history automatically if you want
     * 
     * @param prompt your prompt, if NULL, the default prompt
     * @param addToHistory if true, add the line to history
     * @param out the prompt output stream
     * @param in the prompt input stream
     * @return std::string outputs a C++ string
     */
    std::string LEdit::readlineStr(const char* prompt = NULL, bool addToHistory = false, std::ostream& out = std::cout, std::istream& in = std::cin){
        std::string line;
        out << (prompt == NULL ? deafultPrompt : prompt) << std::flush;

        std::getline(in, line);


        auto arrowHandler = [](char c, std::string historyPrompt, std::string& line, std::istream& in, std::ostream& out, std::vector<std::string>& history){
            bool run = true;
            std::string line2;
            int historyIndex = history.size() - 1;
            std::string historyObject = history[historyIndex];
            while(run){
                out << std::endl << std::flush;
                out << historyPrompt << " " << std::flush;
                std::getline(in, line2);
                
                if(line2.empty()){
                    line = historyObject;
                    run = false;
                }

                for(int i = 0; i < line2.length(); i++){
                    int iStr = (int) line[i];
                    
                    if(i + 2 < line2.length()){
                        int iStr2 = (int) line[i + 1];
                        int iStr3 = (int) line[i + 2];

                        if(iStr2 == 91){
                            if(iStr3 == 65){
                                // up
                                if(historyIndex > 0){
                                    historyIndex--;
                                    historyObject = history[historyIndex];
                                }
                                out << historyPrompt << ": " << historyObject << std::endl << std::flush;
                            }
                            else if(iStr3 == 66){
                                // down
                                // TBA
                            }
                        }
                    }
                }

                out << ">====" << std::endl << std::flush; 
            }

            int columns, rows = NULL;
            char cbar = '=';

            #if defined(MXPSQL_LEdit_Unix)
                FILE* sttyp;
                if((sttyp = popen("stty size", "r")) != NULL){
                    fscanf(sttyp, "%d %d", &rows, &columns);
                    pclose(sttyp);
            }

            #elif defined(MXPSQL_LEdit_Windows)
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

                columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            #endif

            std::string strbar = "";
            if(rows != NULL){
                for(int i = 0; i < columns; i++){
                    strbar += cbar;
                }

                out << strbar << std::endl << std::flush;
            }
        };

        bool run = true;

        // arrow key check
        for(int i = 0; (i<line.length()) && run; i++){
            int iStr = (int) line[i];

            if(iStr == 27){

                if(i + 2 < line.length()){
                    int iStr2 = (int) line[i+1];
                    int iStr3 = (int) line[i+2];

                    if(iStr2 == 91){
                        arrowHandler(iStr3, this->historyPrompt, line, in, out, *this->history);
                    }
                }

            }
            else{
                break;
            }
        }


        if(isHistoryInit()){
            addHistory(line);
        }

        return line;
    }


    // default prompt
    /**
     * @brief set the default prompt
     * 
     * @param prompt the prompt that you want to set it
     */
    void LEdit::setDefaultPrompt(const char* prompt){
        setDefaultPrompt(std::string(prompt));
    }

    /**
     * @brief set the default prompt
     * 
     * @param prompt the prompt you want t set it to
     */
    void LEdit::setDefaultPrompt(const std::string& prompt){
        this->deafultPrompt = prompt;
    }

    /**
     * @brief get the default prompt
     * 
     * @return std::string the default prompt
     */
    std::string LEdit::getDefaultPromptStr(){
        return this->deafultPrompt;
    }


    // history
    /**
     * @brief initialize the history functionality, if initialized, resets the functionality
     * 
     */
    void LEdit::initHistory(){
        if(isHistoryInit()){
            delete history;
        }
        history = new std::vector<std::string>();
    }

    /**
     * @brief uninitialize the history functionality if initialized
     * 
     */
    void LEdit::deinitHistory(){
        if(isHistoryInit()){
            delete history;
            history = nullptr;
        }
    }

    /**
     * @brief check if history is initialized
     * 
     * @return true initialized
     * @return false uninitialized
     */
    bool LEdit::isHistoryInit(){
        return history != nullptr;
    }

    /**
     * @brief add a line to history
     * 
     * @param str the line you want to add
     */
    void LEdit::addHistory(const char* str){
        if(history == nullptr){
            initHistory();
        }
        history->push_back(std::string(str));
    }

    /**
     * @brief add a line to history
     * 
     * @param str the line you want to add
     */
    void LEdit::addHistory(const std::string& str){
        if(history == nullptr){
            initHistory();
        }
        history->push_back(str);
    }

    /**
     * @brief clear the history
     * 
     */
    void LEdit::clearHistory(){
        if(history != nullptr){
            history->clear();
        }
    }

    /**
     * @brief set the history
     * 
     * @param history the history you want to set
     */
    void LEdit::setHistory(std::vector<std::string>* history){
        this->history = history;
    }

    /**
     * @brief get the history
     * 
     * @return std::vector<std::string>* get the history
     */
    std::vector<std::string>* LEdit::getHistory(){
        return this->history;
    }

    /**
     * @brief save the history to a file
     * 
     * @param filename the file path to save the file
     * @return int if unsuccessful (the file handle is bad or history is uninitialized), return -1, otherwise return 0
     */
    int LEdit::saveHistory(const char* filename){
        if(history == nullptr){
            return -1;
        }
        std::ofstream file(filename);
        if(!file.is_open() || !file.good()){
            return -1;
        }
        for(auto& str : *history){
            file << str << std::endl;
        }
        file.close();
        return 0;
    }

    /**
     * @brief loads the history
     * 
     * @param filename the file path to read the file
     * @return int if unsuccessful (the file handle is bad or history is uninitialized), return -1, otherwise return 0
     */
    int LEdit::loadHistory(const char* filename){
        if(history != nullptr){
            delete history;
        }
        history = new std::vector<std::string>();
        std::ifstream file(filename);
        if(!file.is_open()| !file.good()){
            return -1;
        }
        std::string line;
        while(std::getline(file, line)){
            history->push_back(line);
        }
        file.close();
        return 0;
    }


    // operator overloads

    /**
     * @brief output the default prompt
     * 
     * @param out the output stream
     * @param le the LEdit class
     * @return std::ostream& the output stream that is used
     */
    inline std::ostream& operator<<(std::ostream& out, const LEdit& le){
        out << le.deafultPrompt;
        return out;
    }

    /**
     * @brief set the default prompt
     * 
     * @param in the input stream
     * @param le the LEdit class
     * @return std::istream& the input stream that is used
     */
    inline std::istream& operator>>(std::istream& in, LEdit& le){
        std::string line;
        std::getline(in, line);
        le.setDefaultPrompt(line);
        return in;
    }

    /**
     * @brief compare the two objects
     * 
     * @param le1 LEdit to compare
     * @param le2 another LEdit to compare
     * @return true the object are the same
     * @return false the object are different
     */
    inline bool operator==(const LEdit& le1, const LEdit& le2){
        return (le1 == le2);
    }

    /**
     * @brief compare the two objects
     * 
     * @param le1 LEdit to compare
     * @param le2 another LEdit to compare
     * @return true the object are different
     * @return false the object are the same
     */
    inline bool operator!=(const LEdit& le1, const LEdit& le2){
        return !(le1 == le2);
    }

    /**
     * @brief check if history is initialized
     * 
     * @return true history is initialized
     * @return false history is uninitialized
     */
    inline LEdit::operator bool(){
        return isHistoryInit();
    }
#endif
}

#endif