package com.wind.ndk.audiorecoder

/**
 * Copyright (C), 2015-2022, 杭州迈优文化创意有限公司
 * FileName: AudioEncoder
 * Author: wind
 * Date: 2023/4/24 09:41
 * Description: 描述该类的作用
 * Path: 路径
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class AudioEncoder {

    external fun encode(pcmPath:String,audioChannels:Int,bitRate:Int,sampleRate:Int,aacPath:String)


    companion object{
        init {
            System.loadLibrary("audiorecoder")
        }
    }
}