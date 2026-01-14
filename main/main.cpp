#include "../headers/BMP.h"
#include "../headers/jaccard.h"
#include "../headers/Normalizer.h"
#include "../headers/pHash.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

#define TITLE "file1, file2, similarity"
#define INPUT_IMAGE_DIR "../inputImages"
#define GRAY_DIR "../outputGray/"
#define GRAY_BLUR_DIR "../outputGrayBlur/"
#define BLUR_DIR "../blurredImages/"
#define CSV_DIR "../outCSV/"
#define RESIZE_DIR "../resizedImage/"
#define JACCARD_CSV "jaccard2.csv"
#define P_HASH_CSV "pHash2.csv"
#define KERNEL_RADIUS 10
#define SIGMA 1.0
#define HASH_WIDTH 8
#define HASH_HEIGHT 8
#define P_HASH_HEIGHT 32
#define P_HASH_WIDTH 32
#define SINGLE_PAIR_CSV "singlePairSimilarity.csv"
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
void generateGsBllurImages(vector<string> fileNames)
{
    int n = fileNames.size();

    for (int i = 0; i < n; ++i)
    {
        BMP originalImage(fileNames[i].c_str());
        string destination = BLUR_DIR + fileNameTrimmer(fileNames[i], "blur", "bmp");
        toGsBlur(originalImage, KERNEL_RADIUS, SIGMA, destination.c_str());
        // toGrayScale(originalImage, destination.c_str());
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
        if (file1.length() < 4 || file1.substr(file1.length() - 4) != ".bmp" || 
            file2.length() < 4 || file2.substr(file2.length() - 4) != ".bmp") {
            throw runtime_error("Files must be in .bmp format");
        }
        
        // Check if files exist
        ifstream check1(file1);
        ifstream check2(file2);
        if (!check1.good() || !check2.good()) {
            throw runtime_error("One or both files not found");
        }
        
        cout << "Generating grayscale images..." << endl;

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
        
        cout << "Generating resized images..." << endl;
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
        
        vector<filePairSimilarity> fileSim(2);
        fileSim[0].file1 = grayName1;
        fileSim[0].file2 = grayName2;
        fileSim[0].similarity = jaccardDistance(grayImageOne, grayImageTwo);
        
        fileSim[1].file1 = resizeName1;
        fileSim[1].file2 = resizeName2;
        fileSim[1].similarity = pHash(hashOne, hashTwo);
        
        writeToCsv(SINGLE_PAIR_CSV, fileSim);
        cout << "\nComparison complete! Results saved to: " << CSV_DIR << SINGLE_PAIR_CSV << endl;
        
    } catch (const exception& e) {
        cout << "\nError: " << e.what() << endl;
    }
}
void jaccardCalculate(vector<string> files)
{
    int n = files.size();
    vector<filePairSimilarity> filePairs(n * (n - 1) / 2);
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

            cout << fileNameOne << " " << fileNameTwo << " " << similarityScore << endl;

            itr++;
        }
    }
    writeToCsv(JACCARD_CSV, filePairs);
}
void pHashCalculator(vector<string> files)
{
    int n = files.size();
    vector<filePairSimilarity> filePairs(n * (n - 1) / 2);
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

            cout << fileNameOne << " " << fileNameTwo << " " << similarityScore << endl;

            itr++;
        }
    }
    writeToCsv(P_HASH_CSV, filePairs);
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
            jaccardCalculate(files);
            pHashCalculator(files);
            cout << "\nPairwise similarities generated successfully!" << endl;
            cout << "Results saved to: " << CSV_DIR << endl;
            cout << "  - Jaccard: " << JACCARD_CSV << endl;
            cout << "  - pHash: " << P_HASH_CSV << endl;
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