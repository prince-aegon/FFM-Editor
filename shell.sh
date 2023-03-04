#!/bin/zsh
g++ main_encode.cpp -o main_encode -lavcodec -lavutil -lswscale -lavformat -ljpeg;
./main_encode test1.mpg mpeg1video;
g++ main_decode2.cpp -o main_decode2 -lavcodec -lavutil -lswscale -lavformat -ljpeg;
./main_decode2 test1.mpg 1;
mv *.jpg storage;
g++ main_jpg.cpp -o main_jpg -lavformat -lavcodec -lavutil -lswscale;
./main_jpg input.jpg > encode.txt;