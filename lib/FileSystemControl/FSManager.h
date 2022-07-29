#include <Arduino.h>

class FSManager{
    public:
        FSManager();
        void beginFS();
        String readFileString(String nameFile);
        void writeFile(String nameFile, String msg);
        void createFile(String nameFile);
        

};

