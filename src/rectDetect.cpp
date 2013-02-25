#include "rectDetect.hpp"
#include "BingApi.hpp"
#include <cmath>

using namespace cv;
using namespace std;

static int thresh = 4000; //1000
static int N = 32; //16
static int NUM_SHARPEN = 5;
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void findSquares(const std::string& downloadPath, int x, int y, int levelOfDetail, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat image, pyr, timg, gray0, gray;

    std::string quad = Bing::TileSystem::tileXYToQuadKey(x, y, levelOfDetail);
    std::string filename = quad + ".jpeg";

    image = imread(downloadPath + '/' + filename);
    if(image.empty()) return;

    gray0 = Mat(image.size(), CV_8U);
    // down-scale and upscale the image to filter out the noise
    //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    //pyrUp(pyr, timg, image.size());
    //Sharpen image, because they are really blurry already
    GaussianBlur(image, pyr, cv::Size(0, 0), 3);
    addWeighted(image, 1.25, pyr, -0.25, 0, timg);
    for(int m=0; m<NUM_SHARPEN; ++m)
        addWeighted(timg, 1.25, pyr, -0.25, 0, timg);
    //timg = image.clone();
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 1; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
#if USE_CANNY
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
#endif
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
#if USE_CANNY
            }
#endif
            // find contours and store them all as a list
            findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                int area = std::abs(contourArea(Mat(approx)));
                int count = approx.size();
                if( count >= 4 && !(count & 1) &&
                    area >= 64  && area <= 6400 ) //&& isContourConvex(Mat(approx))
                {
                    double maxCosine = 0;

                    /*
                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
                    */
                    for(int j = 0; j < count; ++j){
                        int k = j-1;
                        int m = j+1;
                        if(j == 0) k = count-1;
                        else if(j == (count-1)) m = 0;
                        double cosine = std::abs(angle(approx[k], approx[m], approx[j]));
                        maxCosine = std::max(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}
