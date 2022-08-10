#include <Arduino.h>

class FSManager{
    public:
        FSManager();
        void beginFS();
        void createFile(String nameFile);
        String readFileString(String nameFile);
        void writeFile(String nameFile, String msg, boolean includeDate);
};

