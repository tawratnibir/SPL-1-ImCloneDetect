#include "../headers/formatter.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace
{
    bool isClangFormatInstalled()
    {
        return system("clang-format --version > nul 2>&1") == 0;
    }

    void printMissingClangFormatMessage()
    {
        cout << "clang-format is NOT installed." << endl;
        cout << "Please install LLVM clang-format to enable formatting." << endl;
    }

    bool isSupportedSourceFile(const std::filesystem::path &path)
    {
        string extension = path.extension().string();
        transform(extension.begin(), extension.end(), extension.begin(),
                  [](unsigned char ch) { return static_cast<char>(tolower(ch)); });

        return extension == ".c" || extension == ".cc" || extension == ".cpp"
            || extension == ".cxx" || extension == ".h" || extension == ".hh"
            || extension == ".hpp" || extension == ".hxx";
    }
}

void formatAFile(const string &filePath) {
    if (isClangFormatInstalled()) {
        string command = "clang-format -i \"" + filePath + "\"";
        system(command.c_str());
    } else{
        printMissingClangFormatMessage();
    }
}

void formatAFolder(const string &directoryPath)
{
    if (isClangFormatInstalled())
    {
        namespace fs = std::filesystem;

        vector<fs::path> files;

        for (const auto &entry : fs::directory_iterator(directoryPath))
        {
            if (entry.is_regular_file() && isSupportedSourceFile(entry.path()))
            {
                files.push_back(entry.path());
            }
        }

        sort(files.begin(), files.end());

        if (files.empty())
        {
            cout << "No supported source files found in the selected folder." << endl;
            return;
        }

        for (const auto &file : files) {
            cout << file.string() << endl;
            formatAFile(file.string());
        }

        return;
    }
    else
    {
        printMissingClangFormatMessage();
    }
}
