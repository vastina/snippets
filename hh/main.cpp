#include "color.hpp"
#include "filter.hpp"

int main()
{
  const Mat lena = cv::imread( "./lena.jpg" );
  const auto printOut{[](const Mat& src){
    std::cout << '\n';
    for ( int i = 0; i < src.rows; i++ ) {
      for ( int j = 0; j < src.cols; j++ ) {
        auto& vec { src.at<Vec3b>( i, j ) };
        print( " ", {}, Color( vec[0], vec[1], vec[2] ) );
      }
      std::cout << '\n';
    }
  }};

  printOut(sample( lena, 2, 2 ));
  printOut(sample( lena, 8, 8 ));

  return 0;
}