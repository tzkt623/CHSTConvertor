#include "Convertor.h"
#include <format>
#include <cstdlib>
#include <windows.h>

namespace tezcat
{
    Convertor::Convertor()
    {

    }

    std::string replaceAll(std::string&& str)
    {
        std::replace(std::begin(str), std::end(str), '\\', '/');
        return str;
    }

    std::string wstring2String(const std::wstring& wstr)
    {
        // support chinese
        std::string res;
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
        if ( len <= 0 ) {
            return res;
        }
        char* buffer = new char[ len + 1 ];
        if ( buffer == nullptr ) {
            return res;
        }
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
        buffer[ len ] = '\0';
        res.append(buffer);
        delete[ ] buffer;
        return res;
    }

    std::wstring string2WString(const std::string& str)
    {
        std::wstring res;
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), nullptr, 0);
        if ( len < 0 ) {
            return res;
        }
        wchar_t* buffer = new wchar_t[ len + 1 ];
        if ( buffer == nullptr ) {
            return res;
        }
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
        buffer[ len ] = '\0';
        res.append(buffer);
        delete[ ] buffer;
        return res;
    }

    std::wstring s2ws(const std::string& str)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.from_bytes(str);
    }

    std::string ws2s(const std::wstring& wstr)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.to_bytes(wstr);
    }

    std::wstring s2ws20(const std::string& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    std::string ws2s20(const std::wstring& wstr)
    {
        // Calculating the length of the multibyte string
        size_t len = std::wcstombs(nullptr, wstr.c_str(), 0);

        // Creating a buffer to hold the multibyte string
        char* buffer = new char[ len ];
        buffer[ len ] = '\0';

        // Converting wstring to string
        std::wcstombs(buffer, wstr.c_str(), len);

        // Creating std::string from char buffer
        std::string str(buffer);

        // Cleaning up the buffer
        delete[ ] buffer;

        return str;
    }

    void Convertor::init()
    {
        mLocale = std::locale{ "zh_CH.UTF-8" };

        mSavePath = std_fs::current_path();
        mResPath = std_fs::current_path().parent_path().append("Res");

        const std::vector<std::string> paths
        {
            replaceAll(std_fs::current_path().parent_path().append("Res").append("Config").string())
        };

        mS2T = std::make_shared<opencc::SimpleConverter>("s2t.json", paths);
        mT2S = std::make_shared<opencc::SimpleConverter>("t2s.json", paths);
    }

    void Convertor::printS2T(std::string&& data)
    {
        std::cout << mS2T->Convert(data) << "\n";
    }

    void Convertor::printT2S(std::string&& data)
    {
        std::cout << mT2S->Convert(data) << "\n";
    }

    void Convertor::fprint(std::string inpath, std::string outpath)
    {
        auto path = std::format(("{}\\{}"), mResPath.string(), inpath);
        auto content = this->readFile2String(path, mLocale);
        if ( content.empty() )
        {
            return;
        }

        path = std::format("{}\\OrgList2.txt", mSavePath.string());
        std::fstream out_org_file(path, std::ios::out | std::ios::binary);
        out_org_file << content;
        out_org_file.flush();
        out_org_file.close();

        std_fs::path out_path(outpath);
        std::fstream out_file(out_path, std::ios::out | std::ios::binary);
        out_file << mS2T->Convert(content);
        out_file.flush();
        out_file.close();
    }

    void Convertor::progress(std::string templatePath, std::string inFile, std::string outFile)
    {
        auto template_content = this->readFile2String(mResPath.string() + templatePath, mLocale);
        if ( template_content.empty() )
        {
            return;
        }

        std::string temp_t(template_content);
        temp_t = temp_t.replace(temp_t.find_first_of('^'), 3, "!hhh!");
        std::cout << temp_t;
    }

    void Convertor::loadSChar(std::string inpath)
    {
        mSCharSet = this->readFile2String(inpath, mLocale);
        // 		std::remove_if(mSCharSet.begin(), mSCharSet.end(), [](char a)
        // 			{
        // 				return a == ' ';
        // 			});

        mTCharSet = mS2T->Convert(mSCharSet);

        std::ostringstream ostream;
        ostream << mSCharSet;

        //ostream << mTCharSet;
        std::string str = ostream.str();
        //std::cout << str << "\n";

// 		auto wstr = string2WString(str);
// 		std::wcout << wstr[508];


    }

    auto Convertor::createFile(std::string savePath, const std::locale& locale)
    {
        std::wfstream out_file;
        out_file.imbue(locale);
        out_file.open(savePath, std::ios::out);
        if ( !out_file.is_open() )
        {
            out_file.close();
            return std::tuple<bool, std::wfstream>{ false, std::move(out_file) };
        }

        return std::tuple<bool, std::wfstream>{ true, std::move(out_file) };
    }

    void Convertor::makeTemplate(std::string inpath, std::string saveFileName)
    {
        auto zh_content = this->readFile2StringW(std::format("{}\\{}", mResPath.string(), inpath), mLocale, true);
        auto [flag, save_file] = this->createFile(std::format("{}\\{}", mSavePath.string(), saveFileName), mLocale);
        if ( flag )
        {
            std::wstring temp_w;
            std::string temp_s;

            for ( auto i = 0; i < zh_content.size(); i++ )
            {
                temp_w = zh_content[ i ];
                temp_s = ws2s(temp_w);
                temp_s = mS2T->Convert(temp_s);
                temp_w = s2ws(temp_s);

                save_file << std::format(
                    L"text {{findWhat:\"{0}\"}} {{changeTo:\"{1}\"}} {{includeFootnotes:true, includeMasterPages:true, includeHiddenLayers:true, wholeWord:false}}\n"
                    , temp_w, zh_content[ i ]);
            }

            save_file.flush();
            save_file.close();
        }
    }

    void Convertor::change(std::string zhPath, std::string twPath)
    {
        auto zh_content = this->readFile2StringW(zhPath, mLocale);
        auto [flag, save_file] = this->createFile("ChangeList.txt", mLocale);
        if ( flag )
        {
            std::wstring temp_w;
            std::string temp_s;

            for ( auto i = 0; i < zh_content.size(); i++ )
            {
                temp_w = zh_content[ i ];
                temp_s = ws2s(temp_w);
                temp_s = mS2T->Convert(temp_s);
                temp_w = s2ws(temp_s);

                save_file << std::format(
                    L"text {{findWhat:\"{0}\"}} {{changeTo:\"{1}\"}} {{includeFootnotes:true, includeMasterPages:true, includeHiddenLayers:true, wholeWord:false}}\n"
                    , temp_w, zh_content[ i ]);
            }

            save_file.flush();
            save_file.close();
        }
    }

    std::string Convertor::readFile2String(const std::string& inpath, const std::locale& locale)
    {
        std_fs::path in_path(inpath);

        std::fstream in_file;
        in_file.imbue(locale);
        in_file.open(in_path, std::ios::in);
        if ( !in_file.is_open() )
        {
            return{};
        }

        std::string temp;
        std::string content;

        in_file.seekg(0, in_file.end);
        auto length = in_file.tellg();
        in_file.seekg(0, in_file.beg);

        content.resize(length);
        in_file.read(content.data(), length);

        // 		while (!in_file.eof())
        // 		{
        // 			std::getline(in_file, temp);
        // 			//in_file >> temp;
        // 			content.append(temp);
        // 		}
        in_file.close();

        return content;
    }

    std::wstring Convertor::readFile2StringW(const std::string& inpath, const std::locale& locale, bool ignSpace)
    {
        std_fs::path in_path(inpath);

        std::wifstream in_file;
        in_file.imbue(locale);
        in_file.open(in_path, std::ios::in | std::ios::binary);
        if ( !in_file.is_open() )
        {
            return{};
        }

        std::wstring temp;
        std::wstring content;

        // 		in_file.seekg(0, in_file.end);
        // 		auto length = in_file.tellg();
        // 		in_file.seekg(0, in_file.beg);
        // 
        // 		content.resize(length);
        // 		in_file.read(content.data(), length);

                //content.reserve(10000);
        if ( ignSpace )
        {
            while ( !in_file.eof() )
            {
                in_file >> temp;
                content.append(temp);
            }
        }
        else
        {
            while ( std::getline(in_file, temp) )
            {
                content.append(temp);
            }
        }

        in_file.close();

        return content;
    }

    void Convertor::writeFileW(const std::wstring& data, std::string&& outPath)
    {
        std::wfstream out_file(outPath, std::ios::out);
        if ( !out_file.is_open() )
        {
            return;
        }

        out_file << data;
        out_file.flush();
        out_file.close();
    }
}
