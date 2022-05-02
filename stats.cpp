
#include "stats.h"

stats::stats(PNG & im){
    unsigned int width = im.width();
    unsigned int height = im.height();

    sumHueX.resize(width);
    sumHueY.resize(width);
    sumSat.resize(width);
    sumLum.resize(width);
    hist.resize(width);
    for (unsigned int i = 0; i < width; i++) {
        sumHueX[i].resize(height);
        sumHueY[i].resize(height);
        sumSat[i].resize(height);
        sumLum[i].resize(height);
        hist[i].resize(height);
    }

    
    for (unsigned int j = 0; j < height; j++) {
        for (unsigned int i = 0; i < width; i++) {
            hist[i][j].resize(36);
            HSLAPixel * pix = im.getPixel(i,j);
    
            double hRad = pix->h*PI/180;
            int hue = (int) pix->h;
            if (pix->h >= 360) hue = hue - 360;

            double hX = cos(hRad);
            double hY = sin(hRad);
            int pixK = (int) hue/10;

            if (i == 0 && j == 0) {
                sumHueX[i][j] = hX;
                sumHueY[i][j] = hY;

                sumSat[i][j] = pix->s;

                sumLum[i][j] = pix->l;
            } else if (i == 0) {
                sumHueX[i][j] = hX + sumHueX[i][j-1];
                sumHueY[i][j] = hY + sumHueY[i][j-1];

                sumSat[i][j] = pix->s + sumSat[i][j-1];

                sumLum[i][j] = pix->l + sumLum[i][j-1];

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i][j-1][k];
                }
            } else if (j == 0) {
                sumHueX[i][j] = hX + sumHueX[i-1][j];
                sumHueY[i][j] = hY + sumHueY[i-1][j];

                sumSat[i][j] = pix->s + sumSat[i-1][j];

                sumLum[i][j] = pix->l + sumLum[i-1][j];

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i-1][j][k];
                }
            } else {
                sumHueX[i][j] = hX + sumHueX[i][j-1] + sumHueX[i-1][j] - sumHueX[i-1][j-1];
                sumHueY[i][j] = hY + sumHueY[i][j-1] + sumHueY[i-1][j] - sumHueY[i-1][j-1];
                
                sumSat[i][j] = pix->s + sumSat[i][j-1] + sumSat[i-1][j] - sumSat[i-1][j-1];

                sumLum[i][j] = pix->l + sumLum[i][j-1] + sumLum[i-1][j] - sumLum[i-1][j-1];

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i][j-1][k] + hist[i-1][j][k] - hist[i-1][j-1][k];
                }
            }

            hist[i][j][pixK] += 1;
        }
    }

}


long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

/* your code here */
int ulX = ul.first;
int ulY = ul.second;
int lrX = lr.first;
int lrY = lr.second;
long totalNumPix = (abs(lrX - ulX) + 1) * (abs(lrY - ulY) + 1);

return totalNumPix;

}


HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

int ulX = ul.first;
int ulY = ul.second;
int lrX = lr.first;
int lrY = lr.second;
long totalNumPix = rectArea(ul, lr);
double hueAvg;
double satAvg;
double lumAvg;
double hueY;
double hueX;

if (ulX == 0 && ulY == 0) {
    hueX = sumHueX[lrX][lrY];
    hueY = sumHueY[lrX][lrY];
    satAvg = sumSat[lrX][lrY]/totalNumPix;
    lumAvg = sumLum[lrX][lrY]/totalNumPix;
} else if (ulX == 0) {
    hueX = sumHueX[lrX][lrY] - sumHueX[lrX][ulY-1];
    hueY = sumHueY[lrX][lrY] - sumHueY[lrX][ulY-1];
    satAvg = (sumSat[lrX][lrY] - sumSat[lrX][ulY-1])/totalNumPix;
    lumAvg = (sumLum[lrX][lrY] - sumLum[lrX][ulY-1])/totalNumPix;
} else if (ulY == 0) {
    hueX = sumHueX[lrX][lrY] - sumHueX[ulX-1][lrY];
    hueY = sumHueY[lrX][lrY] - sumHueY[ulX-1][lrY];
    satAvg = (sumSat[lrX][lrY] - sumSat[ulX-1][lrY])/totalNumPix;
    lumAvg = (sumLum[lrX][lrY] - sumLum[ulX-1][lrY])/totalNumPix;
} else {
    hueX = sumHueX[lrX][lrY] - sumHueX[ulX-1][lrY] - sumHueX[lrX][ulY-1] + sumHueX[ulX-1][ulY-1];
    hueY = sumHueY[lrX][lrY] - sumHueY[ulX-1][lrY] - sumHueY[lrX][ulY-1] + sumHueY[ulX-1][ulY-1];
    satAvg = (sumSat[lrX][lrY] - sumSat[ulX-1][lrY] - sumSat[lrX][ulY-1] + sumSat[ulX-1][ulY-1])/totalNumPix;
    lumAvg = (sumLum[lrX][lrY] - sumLum[ulX-1][lrY] - sumLum[lrX][ulY-1] + sumLum[ulX-1][ulY-1])/totalNumPix;
}

hueAvg = (atan2(hueY, hueX)*180/PI);

if (hueAvg > 360) hueAvg = hueAvg - 360;
if (hueAvg < 0) hueAvg = 360 + hueAvg;

return HSLAPixel(hueAvg, satAvg, lumAvg, 1.0);
}

// given a rectangle, return the total entropy over that rect.
	/* @param ul is (x,y) of the upper left corner of the rectangle 
	* @param lr is (x,y) of the lower right corner of the rectangle */
    /* entropy is computed from the hist table, as 
     * follows: E = -Sum(p_i log(p_i)), where p_i is the fraction of
     * pixels in bin i, and the sum is taken over all the bins. 
     * bins holding no pixels should not be included in the sum. */
double stats::entropy(pair<int,int> ul, pair<int,int> lr){
    int ulX = ul.first;
    int ulY = ul.second;
    int lrX = lr.first;
    int lrY = lr.second;
    long area = rectArea(ul, lr);

    vector<int> distn;
    distn.resize(36);
    double entropy = 0;

    /* using private member hist, assemble the distribution over the
    *  given rectangle defined by points ul, and lr into variable distn.
    *  You will use distn to compute the entropy over the rectangle.
    *  if any bin in the distn has frequency 0, then do not add that 
    *  term to the entropy total. see .h file for more details.
    */
   if (ulX == 0 && ulY == 0) {
       for (int k = 0; k <= 35; k++) {
           distn[k] = hist[lrX][lrY][k];
           }
   } else if (ulX == 0) {
       for (int k = 0; k <= 35; k++) {
           distn[k] = hist[lrX][lrY][k] - hist[lrX][ulY-1][k];
           }
   } else if (ulY == 0) {
       for (int k = 0; k <= 35; k++) {
           distn[k] = hist[lrX][lrY][k] - hist[ulX-1][lrY][k];
           }
   } else {
       for (int k = 0; k <= 35; k++) {
           distn[k] = hist[lrX][lrY][k] - hist[ulX-1][lrY][k] - hist[lrX][ulY-1][k] + hist[ulX-1][ulY-1][k];
           }
   }

   for (int i = 0; i <= 35; i++) {
       if (distn[i] > 0 ) {
            double p_i = (double) distn[i]/area;
            entropy += (p_i) * log2(p_i);
       }
   }

    /* my code includes the following lines:
        
    */
    if (-1 * entropy == -0.0) return 0;
    else return  -1 * entropy;
}


double stats::getSumHueX(int x, int y) {
    return sumHueX[x][y];
}

double stats::getSumHueY(int x, int y) {
    return sumHueY[x][y];
}

double stats::getSumSat(int x, int y) {
    return sumSat[x][y];
}

double stats::getSumLum(int x, int y) {
    return sumLum[x][y];
}

int stats::getHistAt(int x, int y, int k) {
    return hist[x][y][k];
}