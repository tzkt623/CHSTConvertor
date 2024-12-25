#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>
#include <locale>
#include <codecvt>
#include <tuple>

#include "opencc/opencc.h"

namespace std_fs = std::filesystem;

namespace tezcat
{
    class Convertor
    {
    public:
        Convertor();

        void init();

    public:
        void printS2T(std::string&& data);
        void printT2S(std::string&& data);

        void fprint(std::string inpath, std::string outpath);

        void progress(std::string templatePath, std::string inFile, std::string outFile);

        void loadSChar(std::string inpath);

        auto createFile(std::string savePath, const std::locale& locale);
        void makeTemplate(std::string inpath, std::string saveFileName);

        void change(std::string zhPath, std::string twPath);

    private:
        std::string readFile2String(const std::string& inpath, const std::locale& locale);
        std::wstring readFile2StringW(const std::string& inpath, const std::locale& locale, bool ignSpace = false);

        void writeFileW(const std::wstring& data, std::string&& outPath);
    private:
        std::shared_ptr<opencc::SimpleConverter> mS2T;
        std::shared_ptr<opencc::SimpleConverter> mT2S;
        std_fs::path mSavePath;
        std_fs::path mResPath;

        std::string mSCharSet;
        std::string mTCharSet;
        std::locale mLocale;
    };
}

