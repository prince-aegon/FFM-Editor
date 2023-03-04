#!/bin/zsh
g++ encode_video.cpp -o encode_video -lavcodec -lavutil -lswscale -lavformat -ljpeg;
./encode_video test1.mpg mpeg1video;
g++ decode_video_2.cpp -o decode_video_2 -lavcodec -lavutil -lswscale -lavformat -ljpeg;
./decode_video_2 test1.mpg 1;
mv *.jpg storage;
g++ decode_jpg.cpp -o decode_jpg -lavformat -lavcodec -lavutil -lswscale;
./decode_jpg input.jpg > encode.txt;
g++ compare_yuv.cpp -o compare_yuv;
./compare_yuv > transcode.txt