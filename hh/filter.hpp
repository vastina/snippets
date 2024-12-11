#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Vec3b;

static Vec3b sampleOne( const Mat& src, int wbegin, int wend, int hbegin, int hend )
{
  int r = 0;
  int g = 0;
  int b = 0;

  for ( int i = hbegin; i < hend; i++ ) {
    for ( int j = wbegin; j < wend; j++ ) {
      auto& vec { src.at<Vec3b>( i, j ) };
      b += vec[2];
      g += vec[1];
      r += vec[0];
    }
  }

  int area = ( wend - wbegin ) * ( hend - hbegin );
  const uchar ab = b / area;
  const uchar ag = g / area;
  const uchar ar = r / area;
  return Vec3b { ar, ag, ab };
}

// how many col or row one unit has
static Mat sample( const Mat& src, int col, int row )
{
  int w = src.cols / col; // + (src.cols % col == 0);
  int h = src.rows / row; // + (src.rows % row == 0);

  Mat dst( h, w, src.type() );
  for ( int i = 0; i < h; i++ ) {
    for ( int j = 0; j < w; j++ ) {
      dst.at<Vec3b>( i, j ) = sampleOne( src, j * col, ( j + 1 ) * col, i * row, ( i + 1 ) * row );
    }
  }

  return dst;
}