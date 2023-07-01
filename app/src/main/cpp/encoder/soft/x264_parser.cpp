//
// Created by wind on 2023/5/22.
//

#include "x264_parser.h"
#include "../../utils/log.h"
#define LOG_TAG "X264Parser"
X264Parser::X264Parser() {

}

X264Parser::~X264Parser() {

}

int X264Parser::parse(uint8_t *data, int size,std::vector<NALU*>* nalus) {
    //判断数据是否00 00 00 01 开头，如果不是则非法
    bool legal= false;
    if (data[0] == 0 &&
        data[1] == 0 &&
        data[2] == 0 &&
        data[3] == 1) {
        legal= true;
    }
    if (!legal){
        return -1;
    }
    int startIndex=0;
    int startIndexSize=4;
    int outBodySize;
    int outStartIndexSize;
    startIndex = findStartCode(startIndex,startIndexSize, data, size, &outBodySize,&outStartIndexSize);

    int naluType = data[4] & (0x1f);
    //LOGE("naluType：%d",naluType);
    NALU *nalu = new NALU(naluType,data+4);
    nalu->startCodeLen=4;
    while (startIndex>0){

        if (nalu!= nullptr){

            nalu->size=outBodySize;
            nalus->push_back(nalu);
        }
        if (startIndex+outStartIndexSize>=size){
            break;
        }
        //获取nalu type
        naluType = data[startIndex+outStartIndexSize] & (0x1f);
        //LOGE("naluType：%d",naluType);
        nalu=new NALU(naluType,data+startIndex+outStartIndexSize);
        nalu->startCodeLen=outStartIndexSize;

        startIndexSize = outStartIndexSize;
        startIndex = findStartCode(startIndex,startIndexSize, data, size, &outBodySize,&outStartIndexSize);
    }
    if (nalu!= nullptr){
        nalu->size=outBodySize;
        nalus->push_back(nalu);
    }

    return 0;



}

int X264Parser::findStartCode(int startIndex,int startIndexSize, uint8_t *data, int dataSize, int *outBodySize,
                              int *outStartIndexSize) {

    int index=startIndex+startIndexSize;
    while (index < dataSize && (data[index] != 0 || data[index + 1] != 0 ||
           data[index + 2] != 1)) {
        index++;
    }
    if (index == 0 || index>=dataSize) {
        *outBodySize = index -(startIndex+startIndexSize);
        return -1;
    }
    //判断index-1是否为0
    //int newIndex = startIndex + 3;
    *outStartIndexSize=data[index-1]==0?4:3;
    int newIndex=data[index-1]==0? index-1:index;
    *outBodySize = newIndex - startIndex-startIndexSize;
    return newIndex;
}