#include "../headers/BMP.h"
#include "../headers/jaccard.h"
#include "../headers/Normalizer.h"
#include "../headers/pHash.h"
#include "../headers/emd.h"
#include "../headers/formatter.h"
#include <cmath>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <set>
#include <stdexcept>
using namespace std;

#define TITLE "file1, file2, similarity"
#define GRAY_DIR "../outputGray/"
#define GRAY_BLUR_DIR "../outputGrayBlur/"
#define CSV_DIR "../outCSV/"
#define RESIZE_DIR "../resizedImage/"
#define JACCARD_CSV "jaccard.csv"
#define P_HASH_CSV "pHash.csv"
#define EMD_CSV "emd.csv"
#define OVERALL_CSV "overall.csv"
#define GRAPH_JSON_DIR "../graphFiles/"
#define GRAPH_JSON_OVERALL "overall_graph.gexf"
#define GRAPH_JSON_JACCARD "jaccard_graph.gexf"
#define GRAPH_JSON_PHASH "pHash_graph.gexf"
#define GRAPH_JSON_EMD "emd_graph.gexf"
#define KERNEL_RADIUS 20
#define SIGMA 1.0
#define HASH_WIDTH 8
#define HASH_HEIGHT 8
#define P_HASH_HEIGHT 32
#define P_HASH_WIDTH 32
#define SINGLE_PAIR_CSV "singlePairSimilarity.csv"
#define PROGRESS_BAR_WIDTH 40

constexpr double DEFAULT_JACCARD_WEIGHT = 0.20;
constexpr double DEFAULT_PHASH_WEIGHT = 0.40;
constexpr double DEFAULT_EMD_WEIGHT = 0.40;
double jaccardWeight = DEFAULT_JACCARD_WEIGHT;
double pHashWeight = DEFAULT_PHASH_WEIGHT;
double emdWeight = DEFAULT_EMD_WEIGHT;

void ensureDirectoryExists(const string &path)
{
    std::filesystem::create_directories(path);
}
void ensureOutputDirectories()
{
    ensureDirectoryExists(GRAY_DIR);
    ensureDirectoryExists(GRAY_BLUR_DIR);
    ensureDirectoryExists(CSV_DIR);
    ensureDirectoryExists(RESIZE_DIR);
    ensureDirectoryExists(GRAPH_JSON_DIR);
}
void printProgressBar(const string &label, int current, int total)
{
    double ratio = (double)current / total;
    int filled = (int)(ratio * PROGRESS_BAR_WIDTH);
    cout << "\r  " << label << " [";
    for (int i = 0; i < PROGRESS_BAR_WIDTH; ++i)
    {
        if (i < filled) cout << "#";
        else cout << "-";
    }
    cout << "] " << (int)(ratio * 100) << "%" << flush;
    if (current == total) cout << endl;
}
struct filePairSimilarity
{
    string file1;
    string file2;
    double similarity;
};

void printSimilarityResults(const vector<filePairSimilarity> &filePairs)
{
    ios::fmtflags previousFlags = cout.flags();
    streamsize previousPrecision = cout.precision();

    cout << TITLE << endl;
    for (const auto &pair : filePairs)
    {
        cout << pair.file1 << ", " << pair.file2 << ", "
             << fixed << setprecision(6) << pair.similarity << endl;
    }

    cout.flags(previousFlags);
    cout.precision(previousPrecision);
}

void printWeightSummary(double jaccardValue, double pHashValue, double emdValue)
{
    ios::fmtflags previousFlags = cout.flags();
    streamsize previousPrecision = cout.precision();

    cout << fixed << setprecision(4);
    cout << "  Jaccard: " << jaccardValue << endl;
    cout << "  pHash:   " << pHashValue << endl;
    cout << "  EMD:     " << emdValue << endl;

    cout.flags(previousFlags);
    cout.precision(previousPrecision);
}

string fileNameTrimmer(string fileName, string type, string extension)
{
    int n = fileName.size();
    int ptr = 0;
    string newFileName;
    for (int i = n - 1; i >= 0; i--)
    {
        if (fileName[i] == '/' || fileName[i] == '\\')
        {
            ptr = i;
            break;
        }
    }
    if (ptr == 0)
        ptr = -1;
    for (int i = ptr + 1; i < n - 4; i++)
    {
        newFileName.push_back(fileName[i]);
    }
    if (type != "")
        newFileName = newFileName + "_" + type + "." + extension;
    else
        newFileName = newFileName + "." + extension;
    return newFileName;
}
void generateGrayImages(vector<string> fileNames)
{
    int n = fileNames.size();

    for (int i = 0; i < n; ++i)
    {
        BMP originalImage(fileNames[i].c_str());
        string destination = GRAY_DIR + fileNameTrimmer(fileNames[i], "gray", "bmp");
        toGrayScale(originalImage, destination.c_str());
    }
}
void generateResizedImages(vector<string> fileNames, int outHeight, int outWidth)
{
    int n = fileNames.size();
    for (int i = 0; i < n; ++i)
    {
        string grayFileName = GRAY_DIR + fileNameTrimmer(fileNames[i], "gray", "bmp");
        BMP originalImage(grayFileName.c_str());
        string destination = RESIZE_DIR + fileNameTrimmer(fileNames[i], "resized", "bmp");
        resizeBilinear(originalImage, outHeight, outWidth, destination.c_str());
    }
}
void writeToCsv(string csvFileName, vector<filePairSimilarity> filePairs)
{
    fstream fout;

    fout.open(CSV_DIR + csvFileName, std::ios::out);
    cout << "Saving to: " << CSV_DIR + csvFileName << endl;
    fout << TITLE << "\n";
    for (auto x : filePairs)
    {
        fout << x.file1 << ", " << x.file2 << ", " << x.similarity << "\n";
    }

    fout.close();
}
void singlePairSimGen(string file1, string file2)
{
    namespace fs = std::filesystem;

    try {
        fs::path imagePath1(file1);
        fs::path imagePath2(file2);

        string extension1 = imagePath1.extension().string();
        string extension2 = imagePath2.extension().string();
        for (char &ch : extension1)
        {
            ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        }
        for (char &ch : extension2)
        {
            ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        }

        if (extension1 != ".bmp" || extension2 != ".bmp") {
            throw runtime_error("Files must be in .bmp format");
        }
        
        if (!fs::exists(imagePath1) || !fs::exists(imagePath2)) {
            throw runtime_error("One or both files not found");
        }
        if (!fs::is_regular_file(imagePath1) || !fs::is_regular_file(imagePath2)) {
            throw runtime_error("Both paths must point to image files");
        }
        
        // cout << "Generating grayscale images..." << endl;

        BMP img1(file1.c_str());
        BMP img2(file2.c_str());
        
        string baseName1 = fileNameTrimmer(file1, "", "bmp");
        string baseName2 = fileNameTrimmer(file2, "", "bmp");
        
        string grayName1 = fileNameTrimmer(file1, "gray", "bmp");
        string grayName2 = fileNameTrimmer(file2, "gray", "bmp");
        string grayDest1 = GRAY_DIR + grayName1;
        string grayDest2 = GRAY_DIR + grayName2;
        
        
        toGrayScale(img1, grayDest1.c_str());
        toGrayScale(img2, grayDest2.c_str());
        
        BMP grayImageOne(grayDest1.c_str());
        BMP grayImageTwo(grayDest2.c_str());
        
        double similarityScore = jaccardDistance(grayImageOne, grayImageTwo);
        cout << "\nJaccard Similarity: " << similarityScore << endl;
        
        // cout << "Generating resized images..." << endl;
        string resizeName1 = fileNameTrimmer(file1, "resized", "bmp");
        string resizeName2 = fileNameTrimmer(file2, "resized", "bmp");
        string resizeDest1 = RESIZE_DIR + resizeName1;
        string resizeDest2 = RESIZE_DIR + resizeName2;
        
        resizeBilinear(grayImageOne, P_HASH_HEIGHT, P_HASH_WIDTH, resizeDest1.c_str());
        resizeBilinear(grayImageTwo, P_HASH_HEIGHT, P_HASH_WIDTH, resizeDest2.c_str());
        
        BMP resImgOne(resizeDest1.c_str());
        BMP resImgTwo(resizeDest2.c_str());
        
        string hashOne = generateHash(resImgOne, HASH_HEIGHT, HASH_WIDTH);
        string hashTwo = generateHash(resImgTwo, HASH_HEIGHT, HASH_WIDTH);
        
        similarityScore = pHash(hashOne, hashTwo);
        cout << "pHash Similarity: " << similarityScore << endl;
        
        // EMD
        BMP origImg1(file1.c_str());
        BMP origImg2(file2.c_str());
        double emdScore = computeEmd(origImg1, origImg2);
        cout << "EMD Similarity: " << emdScore << endl;
        
        double jaccardScore = jaccardDistance(grayImageOne, grayImageTwo);
        double pHashScore = pHash(hashOne, hashTwo);
        double overall = jaccardWeight * jaccardScore + pHashWeight * pHashScore + emdWeight * emdScore;
        cout << "Overall Similarity: " << overall << endl;
        
        vector<filePairSimilarity> fileSim(4);
        fileSim[0].file1 = baseName1;
        fileSim[0].file2 = baseName2;
        fileSim[0].similarity = jaccardScore;
        
        fileSim[1].file1 = baseName1;
        fileSim[1].file2 = baseName2;
        fileSim[1].similarity = pHashScore;
        
        fileSim[2].file1 = baseName1;
        fileSim[2].file2 = baseName2;
        fileSim[2].similarity = emdScore;
        
        fileSim[3].file1 = baseName1;
        fileSim[3].file2 = baseName2;
        fileSim[3].similarity = overall;
        
        writeToCsv(SINGLE_PAIR_CSV, fileSim);
        cout << "\nComparison complete! Results saved to: " << CSV_DIR << SINGLE_PAIR_CSV << endl;
        
    } catch (const exception& e) {
        cout << "\nError: " << e.what() << endl;
    }
}
vector<filePairSimilarity> jaccardCalculate(vector<string> files)
{
    int n = files.size();
    int totalPairs = n * (n - 1) / 2;
    vector<filePairSimilarity> filePairs(totalPairs);
    int itr = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            string fileNameOne = fileNameTrimmer(files[i], "", "bmp");
            string fileNameTwo = fileNameTrimmer(files[j], "", "bmp");
            string grayStrOne = fileNameTrimmer(fileNameOne, "gray", "bmp");
            string grayStrTwo = fileNameTrimmer(fileNameTwo, "gray", "bmp");
            string g1 = grayStrOne;
            string g2 = grayStrTwo;
            grayStrOne = GRAY_DIR + grayStrOne;
            grayStrTwo = GRAY_DIR + grayStrTwo;

            const char *grayPtrOne = grayStrOne.c_str();
            const char *grayPtrTwo = grayStrTwo.c_str();

            BMP grayImageOne(grayPtrOne);
            BMP grayImageTwo(grayPtrTwo);

            double similarityScore = jaccardDistance(grayImageOne, grayImageTwo);

            filePairs[itr].file1 = g1;
            filePairs[itr].file2 = g2;
            filePairs[itr].similarity = similarityScore;

            itr++;
            printProgressBar("Jaccard", itr, totalPairs);
        }
    }
    writeToCsv(JACCARD_CSV, filePairs);
    return filePairs;
}
vector<filePairSimilarity> pHashCalculator(vector<string> files)
{
    int n = files.size();
    int totalPairs = n * (n - 1) / 2;
    vector<filePairSimilarity> filePairs(totalPairs);
    int itr = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            string fileNameOne = fileNameTrimmer(files[i], "resized", "bmp");
            string fileNameTwo = fileNameTrimmer(files[j], "resized", "bmp");
            string resOne = RESIZE_DIR + fileNameOne;
            string resTwo = RESIZE_DIR + fileNameTwo;

            const char *resPtrOne = resOne.c_str();
            const char *resPtrTwo = resTwo.c_str();

            BMP resImgOne(resPtrOne);
            BMP resImgTwo(resPtrTwo);

            string hashOne = generateHash(resImgOne, HASH_HEIGHT, HASH_WIDTH);
            string hashTwo = generateHash(resImgTwo, HASH_HEIGHT, HASH_WIDTH);

            double similarityScore = pHash(hashOne, hashTwo);

            filePairs[itr].file1 = fileNameOne;
            filePairs[itr].file2 = fileNameTwo;
            filePairs[itr].similarity = similarityScore;

            itr++;
            printProgressBar("pHash  ", itr, totalPairs);
        }
    }
    writeToCsv(P_HASH_CSV, filePairs);
    return filePairs;
}
vector<filePairSimilarity> emdCalculator(vector<string> files)
{
    int n = files.size();
    int totalPairs = n * (n - 1) / 2;
    vector<filePairSimilarity> filePairs(totalPairs);
    int itr = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            string fileNameOne = fileNameTrimmer(files[i], "", "bmp");
            string fileNameTwo = fileNameTrimmer(files[j], "", "bmp");

            BMP img1(files[i].c_str());
            BMP img2(files[j].c_str());

            double similarityScore = computeEmd(img1, img2);

            filePairs[itr].file1 = fileNameOne;
            filePairs[itr].file2 = fileNameTwo;
            filePairs[itr].similarity = similarityScore;

            itr++;
            printProgressBar("EMD    ", itr, totalPairs);
        }
    }
    writeToCsv(EMD_CSV, filePairs);
    return filePairs;
}
string stripExtension(const string &name)
{
    size_t dot = name.rfind('.');
    if (dot != string::npos)
        return name.substr(0, dot);
    return name;
}
void generateGraphJson(vector<filePairSimilarity> &pairs, string filenameToBeSaved)
{
    set<string> vertexSet;
    for (auto &p : pairs)
    {
        vertexSet.insert(stripExtension(p.file1));
        vertexSet.insert(stripExtension(p.file2));
    }
    vector<string> vertices(vertexSet.begin(), vertexSet.end());

    ostringstream gexf;
    gexf << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    gexf << "<gexf xmlns=\"http://gexf.net/1.3\" version=\"1.3\">\n";
    gexf << "  <graph mode=\"static\" defaultedgetype=\"undirected\">\n";
    gexf << "    <attributes class=\"edge\">\n";
    gexf << "      <attribute id=\"0\" title=\"similarity\" type=\"double\"/>\n";
    gexf << "    </attributes>\n";
    gexf << "    <nodes>\n";
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        gexf << "      <node id=\"" << vertices[i] << "\" label=\"" << vertices[i] << "\"/>\n";
    }
    gexf << "    </nodes>\n";
    gexf << "    <edges>\n";
    for (size_t i = 0; i < pairs.size(); ++i)
    {
        string src = stripExtension(pairs[i].file1);
        string tgt = stripExtension(pairs[i].file2);
        gexf << "      <edge id=\"" << i << "\" source=\"" << src
             << "\" target=\"" << tgt << "\" weight=\"" << pairs[i].similarity << "\">\n";
        gexf << "        <attvalues>\n";
        gexf << "          <attvalue for=\"0\" value=\"" << pairs[i].similarity << "\"/>\n";
        gexf << "        </attvalues>\n";
        gexf << "      </edge>\n";
    }
    gexf << "    </edges>\n";
    gexf << "  </graph>\n";
    gexf << "</gexf>\n";

    string outPath = string(GRAPH_JSON_DIR) + filenameToBeSaved;

    // Ensure the directory exists
    string mkdirCmd = "if not exist \"" + string(GRAPH_JSON_DIR) + "\" mkdir \"" + string(GRAPH_JSON_DIR) + "\"";
    system(mkdirCmd.c_str());

    ofstream fout(outPath);
    if (!fout.is_open())
    {
        cerr << "Error: Could not open " << outPath << endl;
        return;
    }
    fout << gexf.str();
    fout.close();
    cout << "Graph GEXF saved to: " << outPath << endl;
}
void overallScoreCalculator(vector<filePairSimilarity> &jaccardPairs,
                           vector<filePairSimilarity> &pHashPairs,
                           vector<filePairSimilarity> &emdPairs)
{
    int totalPairs = jaccardPairs.size();
    vector<filePairSimilarity> filePairs(totalPairs);
    for (int i = 0; i < totalPairs; i++)
    {
        double overall = jaccardWeight * jaccardPairs[i].similarity
                       + pHashWeight * pHashPairs[i].similarity
                       + emdWeight * emdPairs[i].similarity;

        filePairs[i].file1 = emdPairs[i].file1;
        filePairs[i].file2 = emdPairs[i].file2;
        filePairs[i].similarity = overall;

        printProgressBar("Overall", i + 1, totalPairs);
    }
    cout << "\nAll CSV generations are complete." << endl;
    cout << "Displaying " << OVERALL_CSV << " contents:" << endl;
    printSimilarityResults(filePairs);
    writeToCsv(OVERALL_CSV, filePairs);
    generateGraphJson(filePairs, GRAPH_JSON_OVERALL);
    generateGraphJson(jaccardPairs, GRAPH_JSON_JACCARD);
    generateGraphJson(pHashPairs, GRAPH_JSON_PHASH);
    generateGraphJson(emdPairs, GRAPH_JSON_EMD);

}
vector<string> getImagesFromDirectory(const string &path)
{
    vector<string> files;
    string winPath = path;
    for (auto &c : winPath)
    {
        if (c == '/')
        {
            c = '\\';
        }
    }

    string searchPath = winPath;
    if (!searchPath.empty() && searchPath.back() != '\\')
    {
        searchPath.push_back('\\');
    }

    system(("dir /b \"" + searchPath + "*.bmp\" > temp_filelist.txt").c_str());

    ifstream infile("temp_filelist.txt");
    string filename;
    while (getline(infile, filename))
    {
        filename.erase(0, filename.find_first_not_of(" \r\n\t"));
        filename.erase(filename.find_last_not_of(" \r\n\t") + 1);

        if (!filename.empty())
        {
            files.push_back(searchPath + filename);
        }
    }

    infile.close();
    system("del temp_filelist.txt");
    return files;
}

string normalizePathInput(string input)
{
    size_t start = input.find_first_not_of(" \t\r\n");
    if (start == string::npos)
    {
        return "";
    }

    size_t end = input.find_last_not_of(" \t\r\n");
    input = input.substr(start, end - start + 1);

    if (input.size() >= 2 && input.front() == '"' && input.back() == '"')
    {
        input = input.substr(1, input.size() - 2);
    }

    return input;
}

bool isBackCommand(const string &input)
{
    string normalized = normalizePathInput(input);

    for (char &ch : normalized)
    {
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }

    return normalized == "back" || normalized == "b";
}

bool tryParseWeightInput(const string &input, double &value)
{
    try
    {
        size_t processedCharacters = 0;
        value = stod(input, &processedCharacters);
        if (processedCharacters != input.size())
        {
            return false;
        }
    }
    catch (const exception &)
    {
        return false;
    }

    return value > 0.0 && value < 1.0;
}

bool promptForWeightValue(const string &label, double &value)
{
    while (true)
    {
        cout << label << " weight (0 < value < 1.0): ";

        string input;
        getline(cin, input);
        input = normalizePathInput(input);

        if (isBackCommand(input))
        {
            return false;
        }

        if (tryParseWeightInput(input, value))
        {
            return true;
        }

        cout << "Enter a numeric value strictly between 0 and 1.0, or type 'back' to cancel." << endl;
    }
}

void configureAlgorithmWeights()
{
    while (true)
    {
        cout << "\n--- Configure Algorithm Weights ---" << endl;
        cout << "Default weights:" << endl;
        printWeightSummary(DEFAULT_JACCARD_WEIGHT, DEFAULT_PHASH_WEIGHT, DEFAULT_EMD_WEIGHT);
        cout << "Current weights:" << endl;
        printWeightSummary(jaccardWeight, pHashWeight, emdWeight);
        cout << "The three weights must each be between 0 and 1.0 and must add up to 1.0." << endl;
        cout << "Type 'back' at any prompt to return without changing weights." << endl;

        double newJaccardWeight = 0.0;
        double newPHashWeight = 0.0;
        double newEmdWeight = 0.0;

        if (!promptForWeightValue("Jaccard", newJaccardWeight) ||
            !promptForWeightValue("pHash", newPHashWeight) ||
            !promptForWeightValue("EMD", newEmdWeight))
        {
            cout << "Returning to the main menu without changing weights." << endl;
            return;
        }

        double totalWeight = newJaccardWeight + newPHashWeight + newEmdWeight;
        if (std::fabs(totalWeight - 1.0) > 1e-9)
        {
            ios::fmtflags previousFlags = cout.flags();
            streamsize previousPrecision = cout.precision();

            cout << fixed << setprecision(6);
            cout << "The three weights must add up to 1.0. Current total: " << totalWeight << endl;

            cout.flags(previousFlags);
            cout.precision(previousPrecision);

            cout << "Please enter the weights again, or type 'back' to cancel." << endl;
            continue;
        }

        jaccardWeight = newJaccardWeight;
        pHashWeight = newPHashWeight;
        emdWeight = newEmdWeight;

        cout << "Algorithm weights updated successfully." << endl;
        cout << "New weights:" << endl;
        printWeightSummary(jaccardWeight, pHashWeight, emdWeight);
        return;
    }
}

bool isBmpPath(const std::filesystem::path &path)
{
    string extension = path.extension().string();
    for (char &ch : extension)
    {
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }

    return extension == ".bmp";
}

void runFormatterFromMenu()
{
    namespace fs = std::filesystem;

    cout << "\n--- Format a File or Folder ---" << endl;
    cout << "Enter the full path of the file or folder to format" << endl;
    cout << "Type 'back' to return to the main menu." << endl;
    cout << "Path: ";

    string pathInput;
    getline(cin, pathInput);
    pathInput = normalizePathInput(pathInput);

    if (isBackCommand(pathInput))
    {
        cout << "Returning to the main menu." << endl;
        return;
    }

    if (pathInput.empty())
    {
        cout << "Path cannot be empty." << endl;
        return;
    }

    fs::path targetPath(pathInput);
    if (!targetPath.is_absolute())
    {
        cout << "Please enter the full path, for example: C:\\Users\\Name\\project\\file.cpp" << endl;
        return;
    }

    try
    {
        if (!fs::exists(targetPath))
        {
            cout << "The provided path does not exist." << endl;
            return;
        }

        if (fs::is_regular_file(targetPath))
        {
            cout << "Formatting file: " << targetPath.string() << endl;
            formatAFile(targetPath.string());
            return;
        }

        if (fs::is_directory(targetPath))
        {
            cout << "Formatting supported source files in folder: " << targetPath.string() << endl;
            formatAFolder(targetPath.string());
            return;
        }

        cout << "The path must point to a regular file or a folder." << endl;
    }
    catch (const fs::filesystem_error &e)
    {
        cout << "Unable to access the provided path: " << e.what() << endl;
    }
}

int main()
{
    int choice = 0;
    string directoryInput;
    vector<string> files;
    string singleFile1, singleFile2;
    ensureOutputDirectories();
    do
    {
        cout << "\n=== Welcome to ImCloneDetect tool ===" << endl;
        cout << "Press 1: Find pairwise similarity on a folder" << endl;
        cout << "Press 2: Find similarity of two images" << endl;
        cout << "Press 3: Format a file or folder (full path required)" << endl;
        cout << "Press 4: Configure algorithm weights" << endl;
        {
            ios::fmtflags previousFlags = cout.flags();
            streamsize previousPrecision = cout.precision();

            cout << fixed << setprecision(2);
            cout << "Current weights: Jaccard = " << jaccardWeight
                 << ", pHash = " << pHashWeight
                 << ", EMD = " << emdWeight << endl;

            cout.flags(previousFlags);
            cout.precision(previousPrecision);
        }
        cout << "Press 5: Exit" << endl;
        cout << "Enter your choice: ";
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a number from 1 to 5." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice)
        {
        case 1:
            cout << "\n--- Pairwise Similarity ---" << endl;
            cout << "Enter the full path of the directory containing BMP images." << endl;
            cout << "Type 'back' to return to the main menu." << endl;
            cout << "Directory: ";
            getline(cin, directoryInput);
            directoryInput = normalizePathInput(directoryInput);
            if (isBackCommand(directoryInput))
            {
                cout << "Returning to the main menu." << endl;
                break;
            }
            if (directoryInput.empty())
            {
                cout << "Directory name cannot be empty." << endl;
                break;
            }

            try
            {
                std::filesystem::path inputDirectory(directoryInput);
                if (!inputDirectory.is_absolute())
                {
                    cout << "Please enter the full path, for example: C:\\Users\\Name\\Pictures" << endl;
                    break;
                }
                if (!std::filesystem::exists(inputDirectory))
                {
                    cout << "The provided directory does not exist." << endl;
                    break;
                }
                if (!std::filesystem::is_directory(inputDirectory))
                {
                    cout << "The provided path must point to a directory." << endl;
                    break;
                }

                files = getImagesFromDirectory(inputDirectory.string());
                if (files.empty())
                {
                    cout << "No BMP images found in the directory." << endl;
                    break;
                }
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                cout << "Unable to access the provided directory: " << e.what() << endl;
                break;
            }

            cout << "Processing " << files.size() << " images..." << endl;
            generateGrayImages(files);
            generateResizedImages(files, P_HASH_HEIGHT, P_HASH_WIDTH);
            {
                auto jaccardPairs = jaccardCalculate(files);
                auto pHashPairs = pHashCalculator(files);
                auto emdPairs = emdCalculator(files);
                overallScoreCalculator(jaccardPairs, pHashPairs, emdPairs);
            }
            cout << "\nPairwise similarities generated successfully!" << endl;
            cout << "Results saved to: " << CSV_DIR << endl;
            cout << "  - Jaccard: " << JACCARD_CSV << endl;
            cout << "  - pHash: " << P_HASH_CSV << endl;
            cout << "  - EMD: " << EMD_CSV << endl;
            cout << "  - Overall: " << OVERALL_CSV << endl;
            break;
        case 2:
            cout << "\n--- Compare Two Images ---" << endl;
            cout << "Enter the full path of each BMP image." << endl;
            cout << "Type 'back' at any prompt to return to the main menu." << endl;
            cout << "First image path: ";
            getline(cin, singleFile1);
            singleFile1 = normalizePathInput(singleFile1);
            if (isBackCommand(singleFile1))
            {
                cout << "Returning to the main menu." << endl;
                break;
            }
            if (singleFile1.empty())
            {
                cout << "First file name cannot be empty." << endl;
                break;
            }
            cout << "Second image path: ";
            getline(cin, singleFile2);
            singleFile2 = normalizePathInput(singleFile2);
            if (isBackCommand(singleFile2))
            {
                cout << "Returning to the main menu." << endl;
                break;
            }
            if (singleFile2.empty())
            {
                cout << "Second file name cannot be empty." << endl;
                break;
            }

            try
            {
                std::filesystem::path imagePath1(singleFile1);
                std::filesystem::path imagePath2(singleFile2);

                if (!imagePath1.is_absolute() || !imagePath2.is_absolute())
                {
                    cout << "Please enter full paths for both images, for example: C:\\Users\\Name\\Pictures\\image.bmp" << endl;
                    break;
                }
                if (!std::filesystem::exists(imagePath1) || !std::filesystem::exists(imagePath2))
                {
                    cout << "One or both image paths do not exist." << endl;
                    break;
                }
                if (!std::filesystem::is_regular_file(imagePath1) || !std::filesystem::is_regular_file(imagePath2))
                {
                    cout << "Both paths must point to files." << endl;
                    break;
                }
                if (!isBmpPath(imagePath1) || !isBmpPath(imagePath2))
                {
                    cout << "Both files must be BMP images." << endl;
                    break;
                }
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                cout << "Unable to access one or both image paths: " << e.what() << endl;
                break;
            }

            cout << "\nComparing:" << endl;
            cout << "  File 1: " << singleFile1 << endl;
            cout << "  File 2: " << singleFile2 << endl;
            singlePairSimGen(singleFile1, singleFile2);
            break;
        case 3:
            runFormatterFromMenu();
            break;
        case 4:
            configureAlgorithmWeights();
            break;
        case 5:
            cout << "\nThank you for using ImCloneDetect!" << endl;
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (choice != 5);
}
