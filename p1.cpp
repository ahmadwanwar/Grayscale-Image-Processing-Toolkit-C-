#include <iostream>
#include<fstream>
#include<vector>
#include<cstring>
#include<cmath>

using namespace std;

int sobelMaskX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
int sobelMaskY[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

struct Image {
    char ImageFileName[100];
    vector<vector<int>> ImageData;
    int cols, rows, maxGray;
    vector<char> comment;
    bool imageLoaded;
    bool imageModified;

    int loadImage(char ImageName[]) {

        ifstream FCIN(ImageName);

        if (!FCIN.is_open())
            return -1;

        char MagicNumber[5];
        char Comment[100];

        FCIN.getline(MagicNumber, 4);
        FCIN.getline(Comment, 100);
        FCIN >> cols >> rows >> maxGray;

        ImageData.clear();
        ImageData.resize(rows, vector<int>(cols, 0));

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                FCIN >> ImageData[r][c];

        if (FCIN.fail())
            return -2;

        FCIN.close();
        imageLoaded = true;
        imageModified = false;
        strcpy_s(ImageFileName, ImageName);
        return 0;
    }

    int saveImage(char ImageName[]) {
        ofstream FCOUT(ImageName);
        if (!FCOUT.is_open())
            return -1;

        FCOUT << "P2\n# This is a comment\n"
            << cols << " " << rows << endl << maxGray << endl;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++)
                FCOUT << ImageData[r][c] << " ";
            FCOUT << endl;
        }
        FCOUT.close();
        imageModified = false;
        return 0;
    }

    void verticalFlipImage() {
        for (int r = 0; r < rows / 2; r++)
            for (int c = 0; c < cols; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[rows - 1 - r][c];
                ImageData[rows - 1 - r][c] = T;
            }
        return;
    }

    void horizontalFlipImage() {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols / 2; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[r][cols - 1 - c];
                ImageData[r][cols - 1 - c] = T;
            }
        return;
    }

    void changeBrightness(double factor) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] *= factor;
                if (ImageData[r][c] > maxGray)
                    ImageData[r][c] = maxGray;
            }
    }

    void rotateClockwise90() {
        vector<vector<int>> rotatedImage(cols, vector<int>(rows, 0));
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                rotatedImage[c][rows - 1 - r] = ImageData[r][c];
        ImageData = rotatedImage;
        swap(rows, cols);
    }

    void rotateAClockwise90() {
        vector<vector<int>> rotatedImage(cols, vector<int>(rows, 0));
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                rotatedImage[cols - 1 - c][r] = ImageData[r][c];

        ImageData = rotatedImage;
        swap(rows, cols);
    }

    void rotateByAngle(double angle, int n) {
        double angleRad;
        if (n == 1) {
            angleRad = -angle * (22.0 / 7.0) / 180.0;
        }
        else if (n == 2) {
            angleRad = angle * (22.0 / 7.0) / 180.0;
        }
        double centerX = cols / 2.0;
        double centerY = rows / 2.0;
        vector<vector<int>> rotatedImage(rows, vector<int>(cols, 0));
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                double x = c - centerX;
                double y = r - centerY;

                double newX = (x * cos(angleRad)) - (y * sin(angleRad));
                double newY = (x * sin(angleRad)) + (y * cos(angleRad));

                newX += centerX;
                newY += centerY;

                if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
                    rotatedImage[r][c] = ImageData[newY][newX];
                }
            }
        }
        ImageData = rotatedImage;
    }

    void contrastStretch() {
        int minPixel = ImageData[0][0];
        int maxPixel = ImageData[0][0];
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (ImageData[r][c] < minPixel) {
                    minPixel = ImageData[r][c];
                }
                if (ImageData[r][c] > maxPixel) {
                    maxPixel = ImageData[r][c];
                }
            }
        }
        double scale = 255.0 / (maxPixel - minPixel);
        double shift = -minPixel * scale;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] = static_cast<int>((scale * ImageData[r][c] + scale));
            }
        }
    }

    void convertToBinary(int threshold) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (ImageData[r][c] > threshold) {
                    ImageData[r][c] = 255;
                }
                else {
                    ImageData[r][c] = 0;
                }
            }

        }
    }

    void translation(int x, int y) {
        vector<vector<int>> translatedImage(rows, vector<int>(cols, 0));
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int newX = c + x;
                int newY = r + y;
                if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
                    translatedImage[newY][newX] = ImageData[r][c];
                }
            }
        }
        ImageData = translatedImage;
    }

    void MeanFilter() {
        vector<vector<int>> newImageData(rows, vector<int>(cols, 0));
        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                int sum = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        sum += ImageData[r + i][c + j];
                    }
                }
                newImageData[r][c] = sum / 9;
            }
        }

        ImageData = newImageData;
    }

    void applyMedianFilter() {
        vector<vector<int>> newData(rows, vector<int>(cols, 0));
        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                int neighborhood[9];
                int index = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        neighborhood[index++] = ImageData[r + i][c + j];
                    }
                }
                int temp;
                for (int i = 0; i < 8; i++) {
                    for (int j = i + 1; j < 9; j++) {
                        if (neighborhood[i] > neighborhood[j]) {
                            temp = neighborhood[i];
                            neighborhood[i] = neighborhood[j];
                            neighborhood[j] = temp;
                        }
                    }
                }
                newData[r][c] = neighborhood[4];
            }
        }
        ImageData = newData;
    }

    void sharpening(double factor) {
        vector<vector<int>> newImageData = ImageData;
        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                int sharpValue = static_cast<int>(factor * (5 * ImageData[r][c] - ImageData[r - 1][c] - ImageData[r + 1][c] - ImageData[r][c - 1] - ImageData[r][c + 1]));
                newImageData[r][c] = sharpValue;
            }
        }
        ImageData = newImageData;
    }

    void scaleImage(double scaleFactor) {
        int nRows = rows * scaleFactor;
        int nCols = cols * scaleFactor;
        vector<vector<int>> newImageData(nRows, vector<int>(nCols, 0));

        for (int r = 0; r < nRows; r++) {
            for (int c = 0; c < nCols; c++) {
                int oR = r / scaleFactor;
                int oC = c / scaleFactor;
                newImageData[r][c] = ImageData[oR][oC];
            }
        }

        ImageData = newImageData;
        rows = nRows;
        cols = nCols;
    }

    void computeDerivatives() {
        vector<vector<int>> gradX(rows, vector<int>(cols, 0));
        vector<vector<int>> gradY(rows, vector<int>(cols, 0));

        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        gradX[r][c] += sobelMaskX[i][j] * ImageData[r + i - 1][c + j - 1];
                        gradY[r][c] += sobelMaskY[i][j] * ImageData[r + i - 1][c + j - 1];
                    }
                }
            }
        }
        vector<vector<int>> gradientMagnitude(rows, vector<int>(cols, 0));
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                gradientMagnitude[r][c] = sqrt(gradX[r][c] * gradX[r][c] + gradY[r][c] * gradY[r][c]);
            }
        }
        ImageData = gradientMagnitude;
    }

    void FindEdges(int threshold) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (ImageData[r][c] > threshold) {
                    ImageData[r][c] = 255;
                }
                else {
                    ImageData[r][c] = 0;
                }
            }
        }
    }

    void cropImage(int sX, int sY, int nRows, int nCols) {
        vector<vector<int>> croppedImageData(nRows, vector<int>(nCols, 0));

        for (int r = 0; r < nRows; r++) {
            for (int c = 0; c < nCols; c++) {
                croppedImageData[r][c] = ImageData[sY + r][sX + c];
            }
        }
        ImageData = croppedImageData;
        rows = nRows;
        cols = nCols;
    }

    void combineImagesSBS(const Image& otherImage) {
        vector<vector<int>> combinedData(rows, vector<int>(cols + otherImage.cols, 0));
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                combinedData[r][c] = ImageData[r][c];
                combinedData[r][c + cols] = otherImage.ImageData[r][c];
            }
        }
        ImageData = combinedData;
        cols += otherImage.cols;
    }
    void combineImagesTtB(const Image& otherImage) {
        vector<vector<int>>combinedData(rows + otherImage.rows, vector<int>(cols, 0));
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                combinedData[r][c] = ImageData[r][c];
            }
        }
        for (int r = 0; r < otherImage.rows; r++) {
            for (int c = 0; c < cols; c++) {
                combinedData[r + rows][c] = otherImage.ImageData[r][c];
            }
        }
        ImageData = combinedData;
        rows += otherImage.rows;
    }
    void applyLinearFilter(const char* filterFileName) {
        ifstream filterFile(filterFileName);
        vector<vector<double>> filter(3, vector<double>(3, 0.0));
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                filterFile >> filter[i][j];
            }
        }
        filterFile.close();
        vector<vector<int>> filteredImageData(rows, vector<int>(cols, 0));
        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                double sum = 0.0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; i <= 1; i++) {
                        sum += ImageData[r + i][c + j] * filter[i + 1][j + 1];
                    }
                }
                filteredImageData[r][c] = sum;
            }
        }
        ImageData = filteredImageData;
    }

};
struct Menu {
    vector<string> menuItems;

    Menu(char menuFile[]) {
        loadMenu(menuFile);
    }

    int loadMenu(char menuFile[]) {
        ifstream IN;
        IN.open(menuFile);
        if (!IN.is_open())
            return -1;
        char menuItem[100], TotalItems[10];

        int Choices;

        IN.getline(TotalItems, 8);
        Choices = atoi(TotalItems);
        for (int i = 1; i <= Choices; i++) {
            IN.getline(menuItem, 99);
            menuItems.push_back(menuItem);
        }
        IN.close();
        return Choices;
    }

    int presentMenu() {
        int userChoice;
        int totalChoices = menuItems.size();


        do {
            int k = 1;
            for (int i = 0; i < totalChoices; i++) {
                if (menuItems[i][0] != '*') {
                    cout << k << "\t" << menuItems[i] << endl;
                    k++;
                }
            }
            cout << " Enter Your Choice (1 - " << k - 1 << " ) ";
            cin >> userChoice;
        } while (userChoice < 1 || userChoice > totalChoices);
        return userChoice;
    }

};

int main() {
    char MenuFile[] = "MainMenu.txt";
    Image images[2];
    int activeImage = 0;
    int errorCode = 0;
    int userChoice;
    int totalChoices;

    Menu menu(MenuFile);
    totalChoices = menu.menuItems.size();
    do {
        userChoice = menu.presentMenu();
        if (1 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }
        }
        else if (2 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }
        else if (3 == userChoice) {
            cout << "enter the multiplicative factor" << endl;
            double fact;
            cin >> fact;
            images[activeImage].changeBrightness(fact);
            cout << "You need to save the changes " << endl;
        }
        else if (4 == userChoice) {
            images[activeImage].contrastStretch();
            cout << "You need to save the changes " << endl;
        }
        else if (5 == userChoice) {
            double fact;
            cout << "enter the factor" << endl;
            cin >> fact;
            images[activeImage].sharpening(fact);
            cout << "You need to save the changes " << endl;

        }
        else if (6 == userChoice) {
            images[activeImage].convertToBinary(128);
            cout << "You need to save the changes " << endl;
        }
        else if (7 == userChoice) {
            double fact;
            cout << "enter the factor; <1 ofr scaling down and >1 for scaling up" << endl;
            cin >> fact;
            images[activeImage].scaleImage(fact);
            cout << "You need to save the changes " << endl;
        }
        else if (8 == userChoice) {
            int num;
            cout << "1 for clockwise and 2 for anticlockwise" << endl;
            cin >> num;
            if (num == 1) {
                images[activeImage].rotateClockwise90();
                cout << "You need to save the changes " << endl;
            }
            else {
                images[activeImage].rotateAClockwise90();
                cout << "You need to save the changes " << endl;

            }


        }
        else if (9 == userChoice) {
            double angle;
            int n;
            cout << "enter the angle" << endl;
            cin >> angle;
            cout << "1 for clockwise and 2 for anticlockwise" << endl;
            cin >> n;
            images[activeImage].rotateByAngle(angle, n);
            cout << "You need to save the changes " << endl;

        }
        else if (10 == userChoice) {
            images[activeImage].horizontalFlipImage();
            cout << "You need to save the changes " << endl;

        }
        else if (11 == userChoice) {
            images[activeImage].verticalFlipImage();
            cout << "You need to save the changes " << endl;

        }
        else if (12 == userChoice) {
            int stX, stY, nRows, nCols;
            cout << "enter starting x-coordinate: ";
            cin >> stX;
            cout << "enter starting y-coordinate: ";
            cin >> stY;
            cout << "enter new number of rows: ";
            cin >> nRows;
            cout << "enter new number of columns: ";
            cin >> nCols;
            images[activeImage].cropImage(stX, stY, nRows, nCols);
            cout << "You need to save the changes " << endl;
        }
        else if (13 == userChoice) {
            char fImage[100], sImage[100];
            cout << "enter the name of the first image";
            cin >> fImage;
            images[activeImage].loadImage(fImage);
            cout << "enter the name of the second image";
            cin >> sImage;
            Image secondImage;
            secondImage.loadImage(sImage);
            int n;
            cout << "press 1 for side by side, 2 for top to bottom " << endl;
            cin >> n;
            if (n == 1) {
                images[activeImage].combineImagesSBS(secondImage);
            }
            else if (n == 2) {
                images[activeImage].combineImagesTtB(secondImage);
            }
            else {
                cout << "invalid input";
            }
            cout << "you need to save the file" << endl;
        }
        else if (14 == userChoice) {
            images[activeImage].MeanFilter();
            cout << "You need to save the changes " << endl;

        }
        else if (15 == userChoice) {
            images[activeImage].applyMedianFilter();
            cout << "You need to save the changes " << endl;
        }
        else if (16 == userChoice) {
            char filter[100];
            cout << "enter filter file name: ";
            cin >> filter;
            images[activeImage].applyLinearFilter(filter);
            cout << "filter applied" << endl;
            cout << "You need to save the changes " << endl;
        }
        else if (17 == userChoice) {
            double fact;
            cout << "enter the factor" << endl;
            cin >> fact;
            images[activeImage].sharpening(fact);
            cout << "You need to save the changes " << endl;
        }
        else if (18 == userChoice) {
            images[activeImage].computeDerivatives();
            cout << "You need to save the changes " << endl;
        }
        else if (19 == userChoice) {
            int threshold;
            cout << "enter the threshold (120-150 best for IK pic)" << endl;
            cin >> threshold;
            images[activeImage].FindEdges(threshold);
            cout << "You need to save the changes " << endl;
        }
        else if (20 == userChoice) {
            int x, y;
            cout << "enter the x and y coordinates" << endl;
            cin >> x >> y;
            images[activeImage].translation(x, y);
            cout << "You need to save the changes " << endl;

        }
        else if (21 == userChoice) {
            double fact;
            cout << "enter the factor; <1 ofr scaling down and >1 for scaling up" << endl;
            cin >> fact;
            images[activeImage].scaleImage(fact);
            cout << "You need to save the changes " << endl;
        }
        else if (22 == userChoice) {
            int n;
            double angle;
            cout << "enter the angle" << endl;
            cin >> angle;
            cout << "1 for clockwise movement, 2 for anticlockwise mvoement" << endl;
            cin >> n;
            images[activeImage].rotateByAngle(angle, n);
            cout << "You need to save the changes " << endl;
        }
        else if (23 == userChoice) {
            int n;
            cout << "1 for Horizontal or 2 for Vertical" << endl;
            cin >> n;
            if (n == 1) {
                images[activeImage].horizontalFlipImage();
                cout << "You need to save the changes " << endl;
            }
            else if (n == 2) {
                images[activeImage].verticalFlipImage();
                cout << "You need to save the changes " << endl;

            }
        }

    } while (userChoice != totalChoices);
    return 0;
}