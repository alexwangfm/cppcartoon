#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

void colorReduce(cv::Mat& image, int div=10)
{
    int nl = image.rows;                    // number of lines
    int nc = image.cols * image.channels(); // number of elements per line

    for (int j = 0; j < nl; j++)
    {
        // get the address of row j
        uchar* data = image.ptr<uchar>(j);

        for (int i = 0; i < nc; i++)
        {
            // process each pixel
            data[i] = data[i] / div * div + div / 2;
        }
    }
}

void detectEdges(cv::Mat& image, cv::Mat& imgTotalC3)
{
    Mat image_gray;
    Mat grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    GaussianBlur(image, image, Size(7,7), 0, 0, BORDER_DEFAULT);

    /// 转换为灰度图
    cvtColor(image, image_gray, CV_RGB2GRAY);

    //细线
    Mat imgC;
    Canny(image_gray, imgC, 30, 90);

    //粗线，越大越粗，但是会有大量噪点
    Mat imgL;
    //第三个参数ddepth表示目标图像的深度，ddepth=-1时，与原图像一致
    Laplacian(image_gray, imgL, -1, 3, 1);

    /// 创建 grad_x 和 grad_y 矩阵
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    /// 求 X方向梯度
    //Scharr(image_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
    Sobel(image_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs( grad_x, abs_grad_x );

    /// 求Y方向梯度
    //Scharr(image_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT);
    Sobel(image_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y );

    /// 合并梯度(近似)
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    grad += imgC;
    grad += imgL;

    normalize(grad, grad, 255, 0, CV_MINMAX);
    GaussianBlur(grad, grad, Size(3,3), 3);
    threshold(grad, grad, 100, 255, THRESH_BINARY_INV);

    cvtColor(grad, imgTotalC3, CV_GRAY2BGR);
}

void mergeImage(cv::Mat &dst, cv::Mat &src1, cv::Mat &src2)
{
    int rows = src1.rows > src2.rows ? src1.rows : src2.rows;//合成图像的行数

    int cols = src1.cols + 20 + src2.cols;  //合成图像的列数

    CV_Assert(src1.type() == src2.type());

    cv::Mat zeroMat = cv::Mat::zeros(rows, cols, src1.type());

    zeroMat.copyTo(dst);

    src1.copyTo(dst(cv::Rect(0, 0, src1.cols, src1.rows)));

    src2.copyTo(dst(cv::Rect(src1.cols + 20,0, src2.cols, src2.rows))); //两张图像之间相隔20个像素
}

int main(int argc, char *argv[])
{
    int count = 0;
    Mat image, imagenew, image_lab, image_l, I1, image_merge, imgTotalC3;
    vector<Mat> channels(3);

    image = imread(argv[1], 1);

    Size size = image.size();
    Size newsize = Size(600, size.height*600/size.width);
    resize(image, image, newsize, 0, 0, INTER_AREA);

    cvtColor(image, image_lab, CV_BGR2Lab);

    split(image_lab, channels);
    image_l = channels[0];
    image_l *= 100.0/255.0; // normalize

    while (count < 16) {
        Mat tmp;

        bilateralFilter(image_l, tmp, 5, 3, 2);
        image_l = tmp;
        count++;
    }

    image_l.copyTo(I1);
    colorReduce(I1, 10);

    channels[0] = I1 * (255.0/100.0);
    merge(channels, image_merge);

    cvtColor(image_merge, imagenew, CV_Lab2BGR);

    detectEdges(image, imgTotalC3);
    bitwise_and(imagenew, imgTotalC3, imagenew);

    // imshow("CARTOON", imagenew);
    // waitKey(0);

    Mat final;
    mergeImage(final, image, imagenew);

    imshow("CARTOON", final);
    waitKey(0);

    return 0;
}
