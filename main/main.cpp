#include "../headers/BMP.h"
#include "../headers/jaccard.h"
#include "../headers/Normalizer.h"
#include "../headers/pHash.h"

#define title "file1, file2, similarity"
#define grayDir "../outputGray/"
#define csvDir "../outCSV"
#define resizeDir "../resizedImage/"
#define hashWidth 8
#define hashHeight 8
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
        if (fileName[i] == '/')
        {
            ptr = i;
            break;
        }
    }
    for (int i = ptr+1; i < n - 4; i++)
    {
        newFileName.push_back(fileName[i]);
    }
    if (type != "")
        newFileName = newFileName + "_" + type + "." + extension;
    else
        newFileName = newFileName + "." + extension;
    return newFileName;
}
void writeToCsv(string csvFileName, vector<filePairSimilarity> filePairs)
{
    fstream fout;

    fout.open(csvDir+csvFileName, std::ios::out);
    cout << csvDir + csvFileName << endl;
    fout << title << "\n";
    for(auto x:filePairs) {
        fout << x.file1 << ", " << x.file2 << ", " << x.similarity << "\n";
    }

    fout.close();
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
            grayStrOne = grayDir + grayStrOne;
            grayStrTwo = grayDir + grayStrTwo;

            const char *grayPtrOne = grayStrOne.c_str();
            const char *grayPtrTwo = grayStrTwo.c_str();
            // cout << grayPtrOne << "\n" << grayPtrTwo << endl;
            BMP imageOne(files[i].c_str());
            BMP imageTwo(files[j].c_str());

            toGrayScale(imageOne, grayPtrOne);
            toGrayScale(imageTwo, grayPtrTwo);

            BMP grayImageOne(grayPtrOne);
            BMP grayImageTwo(grayPtrTwo);

            double similarityScore = jaccardDistance(grayImageOne, grayImageTwo);

            filePairs[itr].file1 = fileNameOne;
            filePairs[itr].file2 = fileNameTwo;
            filePairs[itr].similarity = similarityScore;

            cout << fileNameOne << " " << fileNameTwo << " " << similarityScore << endl;

            itr++;
        }
    }
}
void pHashCalculator(vector<string> files) {
    int n = files.size();
    vector<filePairSimilarity> filePairs(n * (n - 1) / 2);
    int itr = 0;
    for(int i=0;i<n;i++) {
        for(int j=i+1;j<n;j++) {
            string fileNameOne = fileNameTrimmer(files[i], "", "bmp");
            string fileNameTwo = fileNameTrimmer(files[j], "", "bmp");
            string resOne = resizeDir + fileNameOne;
            string resTwo = resizeDir + fileNameTwo;

            const char* resPtrOne = resOne.c_str();
            const char* resPtrTwo = resTwo.c_str();

            BMP resImgOne(resPtrOne);
            BMP resImgTwo(resPtrTwo);

            string hashOne = generateHash(resImgOne, hashWidth, hashHeight);
            string hashTwo = generateHash(resImgTwo, hashWidth, hashHeight);

            double similarityScore = pHash(hashOne, hashTwo);

            filePairs[itr].file1 = fileNameOne;
            filePairs[itr].file2 = fileNameTwo;
            filePairs[itr].similarity = similarityScore;

            cout << fileNameOne << " " << fileNameTwo << " " << similarityScore << endl;

            itr++;
        }
    }
    writeToCsv("/pHashSims.csv", filePairs);
}
int main()
{
    freopen("res.txt", "w", stdout);
    // BMP image("../practice/bubble_test_1.bmp");
    // string fileName = image.fileName;
    // string newFileName = fileNameTrimmer(fileName, "gray", "bmp");

    // // newFileName.pop_back();
    // // newFileName.pop_back();
    // // newFileName.pop_back();
    // // newFileName.pop_back();

    // // newFileName += "_gray.bmp";

    // newFileName = grayDir + newFileName;
    // const char* finalDir = newFileName.c_str();
    // cout << finalDir << endl;
    // toGrayScale(image, finalDir);
    vector<string> files = {"../resizedImage/bitmask32_bubble_1_gray.bmp",
                            "../resizedImage/bitmask32_bubble_2_gray.bmp",
                            "../resizedImage/bitmask32_bubble_3_gray.bmp",
                            "../resizedImage/bitmask32_bubble_4_gray.bmp",
                            "../resizedImage/bitmask32_bubble_5_gray.bmp",
                            "../resizedImage/bitmask32_insertion_1_gray.bmp",
                            "../resizedImage/bitmask32_insertion_2_gray.bmp",
                            "../resizedImage/bitmask32_insertion_3_gray.bmp",
                            "../resizedImage/bitmask32_insertion_4_gray.bmp",
                            "../resizedImage/bitmask32_insertion_5_gray.bmp"};
    // jaccardCalculate(files);
    pHashCalculator(files);
    // BMP image("../inputImages/bubble_1.bmp");

    // toGrayScale(image, "../outputGray/bubble_1_gray_2.bmp");
}