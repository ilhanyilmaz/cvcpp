cmake_minimum_required(VERSION 2.8)
project( SampleHist )
find_package( OpenCV REQUIRED )
add_executable( SampleHist MTSampleHist.cpp )
target_link_libraries( SampleHist ${OpenCV_LIBS} )
