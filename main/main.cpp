#include "../headers/BMP.h"
#include "../headers/jaccard.h"
#include "../headers/Normalizer.h"
#include "../headers/pHash.h"

#define TITLE "file1, file2, similarity"
#define INPUT_IMAGE_DIR "../inputImages"
#define GRAY_DIR "../outputGray/"
#define GRAY_BLUR_DIR "../outputGrayBlur/"
#define BLUR_DIR "../blurredImages/"
#define CSV_DIR "../outCSV/"
#define RESIZE_DIR "../resizedImage/"
#define JACCARD_CSV "jaccard.csv"
#define P_HASH_CSV "pHash.csv"
#define KERNEL_RADIUS 10
#define SIGMA 1.0
#define HASH_WIDTH 8
#define HASH_HEIGHT 8
#define P_HASH_HEIGHT 32
#define P_HASH_WIDTH 32

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
    if(ptr == 0) ptr = -1;
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
void generateGrayImages(vector<string> fileNames) {
    int n = fileNames.size();

    for(int i=0;i<n;++i) {
        BMP originalImage(fileNames[i].c_str());
        string destination = GRAY_DIR + fileNameTrimmer(fileNames[i], "gray", "bmp");
        toGrayScale(originalImage, destination.c_str());
    }
}
void generateResizedImages(vector<string> fileNames, int outHeight, int outWidth) {
    int n = fileNames.size();
    for(int i=0;i<n;++i) {
        string grayFileName = GRAY_DIR + fileNameTrimmer(fileNames[i], "gray", "bmp");
        BMP originalImage(grayFileName.c_str());
        string destination = RESIZE_DIR + fileNameTrimmer(fileNames[i], "resized", "bmp");
        resizeBilinear(originalImage, outHeight, outWidth, destination.c_str());
    }
}
void generateGsBllurImages(vector<string> fileNames) {
    int n = fileNames.size();

    for(int i=0;i<n;++i) {
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

            BMP imageOne(files[i].c_str());
            BMP imageTwo(files[j].c_str());

            toGrayScale(imageOne, grayPtrOne);
            toGrayScale(imageTwo, grayPtrTwo);

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
vector<string> getImagesFromDirectory(string path) {
    vector<string> files;
    string winPath = path;
    for (auto& c : winPath) {
        if (c == '/') c = '\\';
    }
    
    system(("dir /b " + winPath + "\\*.bmp > temp_filelist.txt").c_str());
    
    ifstream infile("temp_filelist.txt");
    string filename;
    while (getline(infile, filename)) {
        filename.erase(0, filename.find_first_not_of(" \r\n\t"));
        filename.erase(filename.find_last_not_of(" \r\n\t") + 1);
        
        if (!filename.empty()) {
            files.push_back(winPath + "\\" + filename);
        }
    }
    infile.close();
    system("del temp_filelist.txt");
    return files;
}
int main()
{
    freopen("res.txt", "w", stdout);
    vector<string> files = getImagesFromDirectory(INPUT_IMAGE_DIR);
    // vector<string> grayFiles = getImagesFromDirectory(GRAY_DIR);
    // generateGsBllurImages(grayFiles);
    generateGrayImages(files);
    generateResizedImages(files, P_HASH_HEIGHT, P_HASH_WIDTH);
    jaccardCalculate(files);
    pHashCalculator(files);
    cerr << "Similarities generated successfully!\n";
    // cout << fileNameTrimmer(files[0], "", "bmp");
    // for(auto x:files) cout << x << endl;
}