#include "../headers/emd.h"

void normalizeSignatureValues(vector<double> &sign)
{
    int n = sign.size();
    double sum = 0;
    for (int i = 0; i < n; ++i)
        sum += sign[i];
    for (int i = 0; i < n; ++i)
        sign[i] = (sign[i] / sum);
}
double computeEuclid(int x1, int y1, int x2, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
vector<double> emdSignatureGenerator(vector<vector<uint8_t>> pixels, int divider)
{
    int n = pixels.size();
    vector<double> signature;  // No initial size, using push_back
    int itr1 = pixels.size() / divider;
    int itr2 = pixels[0].size() / divider;
    for (int i = 0; i < divider; ++i)
    {
        for (int j = 0; j < divider; ++j)
        {
            int sum = 0;
            for (int k = i * itr1; k < (i + 1) * itr1; ++k)
            {
                for (int l = j * itr2; l < (j + 1) * itr2; ++l)
                {
                    sum += pixels[k][l];
                }
            }
            signature.push_back(sum);
        }
    }
    return signature;
}
double computeEmd(BMP image1, BMP image2) {
    image1 = resizeBilinear(image1, 300, 300, "emd2_1.bmp");
    image2 = resizeBilinear(image2, 300, 300, "emd2_2.bmp");
    image1 = toGrayScale(image1, "emd2_1_gray.bmp");
    image2 = toGrayScale(image2, "emd2_2_gray.bmp");
    image1 = toGsBlur(image1, 3, 1, "emd2_1_gsblur.bmp");
    image2 = toGsBlur(image2, 3, 1, "emd2_2_gsblur.bmp");
    vector<vector<uint8_t>> pixels1, pixels2;
    pixels1 = pixelVectorGenerator(image1, 300, 300);
    pixels2 = pixelVectorGenerator(image2, 300, 300);

    vector<vector<double>> distanceMat(36, vector<double>(36));
    int idx1 = 0;
    for(int i1=0; i1 < 6; ++i1) {
        for(int j1 = 0; j1 < 6; ++j1) {
            int idx2 = 0;
            for(int i2 = 0; i2 < 6; ++i2) {
                for(int j2 = 0; j2 < 6; ++ j2) {
                    distanceMat[idx1][idx2] = computeEuclid(i1 * 50 + 25
                    , j1 * 50 + 25, i2 * 50 + 25, j2 * 50 + 25);
                    idx2++;
                }
            }
            idx1++;
        }
    }

    vector<double> sign1 = emdSignatureGenerator(pixels1, 6);
    vector<double> sign2 = emdSignatureGenerator(pixels2, 6);
    normalizeSignatureValues(sign1);
    normalizeSignatureValues(sign2);

    SimpleMinCostFlow min_cost_flow;
    const int64_t SCALE = 1000000;
    vector<int64_t> supply(36), demand(36);
    int64_t totalSupply = 0, totalDemand = 0;

    for(int i=0; i < 36; ++i) {
        supply[i] = static_cast<int64_t>(sign1[i] * SCALE);
        demand[i] = static_cast<int64_t>(sign2[i] * SCALE);
        totalSupply += supply[i];
        totalDemand += demand[i];
    }
    int64_t imbalance = totalSupply - totalDemand;
    if(imbalance != 0) {
        int max_idx = 0;
        for(int i=1; i < 36; ++i) {
            if(supply[i] > supply[max_idx]) max_idx = i;
        }
        supply[max_idx] -= imbalance;
        totalSupply -= imbalance;
    }
    for(int i = 0; i < 36; ++i) {
        min_cost_flow.SetNodeSupply(i, supply[i]);
        min_cost_flow.SetNodeSupply(36 + i, -demand[i]);
    }
    int arc_count = 0;

    for(int i=0;i<36;++i) {
        for(int j=0;j<36;++j) {
            min_cost_flow.AddArcWithCapacityAndUnitCost(
                i, 36+j, INT64_MAX, 
                static_cast<int64_t>(distanceMat[i][j] * 1000)
            );
            arc_count++;
        }
    }
    SimpleMinCostFlow::Status status = min_cost_flow.Solve();

    if(status == SimpleMinCostFlow::OPTIMAL) {
        double emd = static_cast<double>(min_cost_flow.OptimalCost()) / 1000000000.0;
        double max_distance = sqrt(pow(5 * 50, 2) + pow(5 * 50, 2));
        double emd_max = max_distance;
        double similarity = 1.0 - emd / emd_max;

        return similarity;
    } else{
        cerr << "No optimal solution found!" << endl;
        return 0;
    }
}