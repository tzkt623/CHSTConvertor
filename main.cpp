#include "Convertor.h"

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    std::system("chcp 65001");

    tezcat::Convertor tconvertor;
    tconvertor.init();
    tconvertor.fprint("CHS7000.txt", "test.txt");
    //tconvertor.loadSChar("E:/Desktop/CHS7000.txt");
    //tconvertor.loadTChar("test.txt");
    tconvertor.makeTemplate("CHS7000.txt", "ChangeList2.txt");

    //std::system("pause");

    return 0;
}
