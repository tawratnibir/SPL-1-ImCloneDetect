#include<iostream>
#include<vector>
// #include<algorithm>
#include<cmath>
#include<iomanip>
double gaussian(double x, double y, double sigma) {
    return ((1.0 / (2.0 * M_PI * sigma * sigma)) * std::exp(-(x * x + y * y) / (2.0 * sigma * sigma)));
}
std::vector<std::vector<double>> createKernel(int kernelRadius, double sigma) {
    int kernelSize = 2 * kernelRadius + 1;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0;
    for(int i = -kernelRadius; i <= kernelRadius; ++i) {
        for(int j = -kernelRadius; j <= kernelRadius; ++j) {
            kernel[i + kernelRadius][j + kernelRadius] = gaussian(i, j, sigma);
            sum+=kernel[i + kernelRadius][j + kernelRadius];
        }
    }

    for(int i=0;i<kernelSize;i++) {
        for(int j=0;j<kernelSize;j++) {
            kernel[i][j]/=sum;
        }
    }
    return kernel;
}
int main() {
    int kernelRadius = 2;
    double sigma = 1.0;
    std::cout << std::fixed << std::setprecision(10);
    std::vector<std::vector<double>> kernel = createKernel(kernelRadius, sigma);
    int n = 2 * kernelRadius + 1;
    int m = n;
    for(int i=0;i<n;i++) {
        for(int j=0;j<m;j++) {
            std::cout << kernel[i][j] << " ";
        }
        std::cout << "\n";
    }
}