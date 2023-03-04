#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}

using namespace std;

int main()
{
    avformat_network_init();
    // avcodec_register_all();

    AVFormatContext *pFormatCtx = nullptr;
    if (avformat_open_input(&pFormatCtx, "input.jpg", nullptr, nullptr) != 0)
    {
        throw runtime_error("Could not open file.");
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
    {
        throw runtime_error("Could not find stream information.");
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1)
    {
        throw runtime_error("Could not find video stream.");
    }

    AVCodecParameters *pCodecParams = pFormatCtx->streams[video_stream_index]->codecpar;
    const AVCodec *pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    if (!pCodec)
    {
        throw runtime_error("Codec not found.");
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        throw runtime_error("Could not allocate codec context.");
    }

    if (avcodec_parameters_to_context(pCodecCtx, pCodecParams) < 0)
    {
        throw runtime_error("Could not set codec context parameters.");
    }

    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
    {
        throw runtime_error("Could not open codec.");
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        throw runtime_error("Could not allocate frame.");
    }

    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        throw runtime_error("Could not allocate packet.");
    }

    int response = av_read_frame(pFormatCtx, pPacket);
    while (response >= 0)
    {
        if (pPacket->stream_index == video_stream_index)
        {
            response = avcodec_send_packet(pCodecCtx, pPacket);
            if (response < 0)
            {
                throw runtime_error("Error sending packet to codec context.");
            }

            response = avcodec_receive_frame(pCodecCtx, pFrame);
            if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
            {
                av_packet_unref(pPacket);
                response = av_read_frame(pFormatCtx, pPacket);
                continue;
            }
            else if (response < 0)
            {
                throw runtime_error("Error receiving frame from codec context.");
            }

            // Do something with the frame here

            printf("Image format: %d x %d %s\n", pFrame->width, pFrame->height,
                   av_get_pix_fmt_name((AVPixelFormat)pFrame->format));

            // for (int y = 0; y < pFrame->height; y++)
            // {
            //     printf("Row %d: ", y);
            //     for (int x = 0; x < pFrame->width; x++)
            //     {
            //         // Compute RGB value from YUV components
            //         uint8_t Y = pFrame->data[0][y * pFrame->linesize[0] + x];
            //         uint8_t U = pFrame->data[1][y / 2 * pFrame->linesize[1] + x / 2];
            //         uint8_t V = pFrame->data[2][y / 2 * pFrame->linesize[2] + x / 2];
            //         int R = Y + 1.402 * (V - 128);
            //         int G = Y - 0.34414 * (U - 128) - 0.71414 * (V - 128);
            //         int B = Y + 1.772 * (U - 128);

            //         // Clamp RGB values to [0, 255]
            //         R = min(max(R, 0), 255);
            //         G = min(max(G, 0), 255);
            //         B = min(max(B, 0), 255);

            //         // Print RGB value as hex string
            //         printf("%02X %02X %02X ", R, G, B);
            //     }
            //     printf("\n");
            // }
            int counter = 0;
            for (int y = 0; y < pFrame->height; y++)
            {
                printf("Row %d: ", y);
                for (int x = 0; x < pFrame->width; x++)
                {
                    uint8_t Y = pFrame->data[0][y * pFrame->linesize[0] + x];
                    uint8_t U = pFrame->data[1][y / 2 * pFrame->linesize[1] + x / 2];
                    uint8_t V = pFrame->data[2][y / 2 * pFrame->linesize[2] + x / 2];

                    printf("%02X %02X %02X ", Y, U, V);
                    counter++;
                }
                printf("\n");
            }
            printf("%d\n", counter);
            av_packet_unref(pPacket);
            response = av_read_frame(pFormatCtx, pPacket);
        }

        av_packet_free(&pPacket);
        av_frame_free(&pFrame);
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        avformat_network_deinit();

        return 0;
    }
}
