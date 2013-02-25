#ifndef RECTDETECT_HPP
#define RECTDETECT_HPP
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

void findSquares(const std::string& downloadPath, int x, int y, int levelOfDetail, std::vector<std::vector<cv::Point> >& squares );

#endif // RECTDETECT_HPP
