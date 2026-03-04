#include "../headers/BMP.h"
#include "../headers/jaccard.h"
#include "../headers/Normalizer.h"
#include "../headers/pHash.h"
#include "../headers/emd.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <stdexcept>
using namespace std;

#define TITLE "file1, file2, similarity"
#define INPUT_IMAGE_DIR "../inputImages"
#define GRAY_DIR "../outputGray/"
#define CSV_DIR "../outCSV/"
#define RESIZE_DIR "../resizedImage/"
#define JACCARD_CSV "jaccard.csv"
#define P_HASH_CSV "pHash.csv"
#define EMD_CSV "emd.csv"
#define JACCARD_WEIGHT 0.20
#define PHASH_WEIGHT 0.40
#define EMD_WEIGHT 0.40
#define OVERALL_CSV "overall.csv"
#define GRAPH_JSON_DIR "../graphFiles/"
#define GRAPH_JSON_FILE "overall_graph.gexf"
#define KERNEL_RADIUS 10
#define SIGMA 1.0
#define HASH_WIDTH 8
#define HASH_HEIGHT 8
#define P_HASH_HEIGHT 32
#define P_HASH_WIDTH 32
#define SINGLE_PAIR_CSV "singlePairSimilarity.csv"
#define PROGRESS_BAR_WIDTH 40
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
    try {
        // Validate file extensions
        if (file1.find(".bmp") == string::npos || file2.find(".bmp") == string::npos) {
            throw runtime_error("Files must be in .bmp format");
        }
        
        // Check if files exist
        ifstream check1(file1);
        ifstream check2(file2);
        if (!check1.good() || !check2.good()) {
            throw runtime_error("One or both files not found");
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
        double overall = JACCARD_WEIGHT * jaccardScore + PHASH_WEIGHT * pHashScore + EMD_WEIGHT * emdScore;
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
void generateGraphJson(vector<filePairSimilarity> &pairs)
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

    string outPath = string(GRAPH_JSON_DIR) + GRAPH_JSON_FILE;

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
        double overall = JACCARD_WEIGHT * jaccardPairs[i].similarity
                       + PHASH_WEIGHT * pHashPairs[i].similarity
                       + EMD_WEIGHT * emdPairs[i].similarity;

        filePairs[i].file1 = emdPairs[i].file1;
        filePairs[i].file2 = emdPairs[i].file2;
        filePairs[i].similarity = overall;

        printProgressBar("Overall", i + 1, totalPairs);
    }
    writeToCsv(OVERALL_CSV, filePairs);
    generateGraphJson(filePairs);
}
vector<string> getImagesFromDirectory(string path)
{
    vector<string> files;
    string winPath = path;
    for (auto &c : winPath)
    {
        if (c == '/')
            c = '\\';
    }

    system(("dir /b " + winPath + "\\*.bmp > temp_filelist.txt").c_str());

    ifstream infile("temp_filelist.txt");
    string filename;
    while (getline(infile, filename))
    {
        filename.erase(0, filename.find_first_not_of(" \r\n\t"));
        filename.erase(filename.find_last_not_of(" \r\n\t") + 1);

        if (!filename.empty())
        {
            files.push_back(winPath + "\\" + filename);
        }
    }
    infile.close();
    system("del temp_filelist.txt");
    return files;
}
int main()
{
    int choice = 0;
    string directoryInput;
    vector<string> files;
    string singleFile1, singleFile2;
    do
    {
        cout << "\n=== Welcome to ImCloneDetect tool ===" << endl;
        cout << "Press 1: Find pairwise similarity on a folder" << endl;
        cout << "Press 2: Find similarity of two images" << endl;
        cout << "Press 3: Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        switch (choice)
        {
        case 1:
            cout << "\n--- Pairwise Similarity ---" << endl;
            cout << "Enter the directory name (relative to ImCloneDetect/): ";
            getline(cin, directoryInput);
            directoryInput = "../" + directoryInput;
            files = getImagesFromDirectory(directoryInput);
            if (files.size() == 0)
            {
                cout << "No images found in the directory." << endl;
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
            cout << "Files must be in ImCloneDetect/inputImages" << endl;
            cout << "Enter the first file name: ";
            getline(cin, singleFile1);
            cout << "Enter the second file name: ";
            getline(cin, singleFile2);
            directoryInput = INPUT_IMAGE_DIR;
            singleFile1 = directoryInput + '/' + singleFile1;
            singleFile2 = directoryInput + '/' + singleFile2;
            cout << "\nComparing:" << endl;
            cout << "  File 1: " << singleFile1 << endl;
            cout << "  File 2: " << singleFile2 << endl;
            singlePairSimGen(singleFile1, singleFile2);
            break;
        case 3:
            cout << "\nThank you for using ImCloneDetect!" << endl;
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (choice != 3);
}