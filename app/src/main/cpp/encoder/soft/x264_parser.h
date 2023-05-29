//
// Created by wind on 2023/5/22.
//

#ifndef NDK_VIDEORECORDER_X264_PARSER_H
#define NDK_VIDEORECORDER_X264_PARSER_H

#include <cstdint>
#include <queue>

/*
 * h264数据 由一系列的NALU组成，前两个是SPS,PPS
 * NALU组成
 * START CODE：用于标识这是一个NALU单元的开始，必须是“00 00 00 01”或“00 00 01”
 * NALU HEADER：紧随START CODE后的一个字节，
 *              第1bit表示禁止位，H264规定此位为0
 *              第2-3个bit表示这个NALU的重要性
 *              第4-8个bit表示NALU类型
 *              NALU类型表
 *                  0：未规定
                    1：非IDR图像中不采用数据划分的片段
                    2：非IDR图像中A类数据划分片段
                    3：非IDR图像中B类数据划分片段
                    4：非IDR图像中C类数据划分片段
                    5：IDR图像的片段
                    6：补充增强信息（SEI）
                    7：序列参数集（SPS）
                    8：图像参数集（PPS）
                    9：分割符
                    10：序列结束符
                    11：流结束符
                    12：填充数据
                    13：序列参数集扩展
                    14：带前缀的NAL单元
                    15：子序列参数集
                    16 – 18：保留
                    19：不采用数据划分的辅助编码图像片段
                    20：编码片段扩展
                    21 – 23：保留
                    24 – 31：未规定
 *
 *RBSP：包含一个或多个slice，一个slice由slice header和片数据组成
 *
 *
 * 如果编码器设置中没有设置AV_CODEC_FLAG_GLOBAL_HEADER，所以在每个关键帧都存有sps/pps的信息 annexb格式
 * 如果编码器设置了AV_CODEC_FLAG_GLOBAL_HEADER，那么只会在全局存放一份 avcc格式或avc1格式
 */
#define NALU_TYPE_NON_IDR 1
#define NALU_TYPE_IDR     5
#define NALU_TYPE_SEI     6
#define NALU_TYPE_SPS     7
#define NALU_TYPE_PPS     8



typedef struct NALU{
    int type;
    uint8_t* body;
    int size;
    int startCodeLen;

    NALU(int type,uint8_t* body){
        this->type=type;
        this->body=body;
    }

    ~NALU(){
        if (body!= nullptr){
            delete body;
        }
    }

} NALU;

class X264Parser {

public:
    X264Parser();
    ~X264Parser();

    int parse(uint8_t *data,int size,std::vector<NALU*>* nalus);
private:



    /**
     * StartCode用于标识这是一个NALU单元的开始，必须是“00 00 00 01”或“00 00 01”
     * @param startIndex  start from where
     * @param startIndexSize
     * @param data        h264data from AVPacket.data
     * @param dataSize
     * @param outBodySize  nalu body size exclude startCode and head
     * @param outStartIndexSize startIndex's size in bytes
     * @return next startIndex
     */
    int findStartCode(int startIndex,int startIndexSize,uint8_t *data,int dataSize,int* outBodySize,int *outStartIndexSize );




};

#endif //NDK_VIDEORECORDER_X264_PARSER_H
