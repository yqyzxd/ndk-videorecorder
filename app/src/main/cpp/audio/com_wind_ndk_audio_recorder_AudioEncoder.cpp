#include "com_wind_ndk_audio_recorder_AudioEncoder.h"
#include "audio_encoder.h"
//
// Created by wind on 2023/4/24.
//

void encode(const char* pcmPath,const char* aacPath,int channels,int bitRate,int  sampleRate){
   AudioEncoder* encoder=new AudioEncoder;

   int bitsPerSample=16;
   char * codecName = "libfdk_aac";
   encoder->init(bitRate,channels,sampleRate,bitsPerSample,aacPath,codecName);
   int bufferSize=1024*256;
   byte* buffer=new byte[bufferSize];
   FILE* pcmFile= fopen(pcmPath,"rb+");
   int readBufferSize=0;
   while ((readBufferSize= fread(buffer,1,bufferSize,pcmFile))>0){
      encoder->encode(buffer,readBufferSize);
   }
   delete[] buffer;
   fclose(pcmFile);
   encoder->destroy();
   delete encoder;

}

JNIEXPORT void JNICALL Java_com_wind_ndk_audiorecoder_AudioEncoder_encode
(JNIEnv *env, jobject jobj, jstring jpcmPath, jint channels, jint bitRate, jint jsampleRate, jstring jaccPath){

   const char* pcmPath =env->GetStringUTFChars(jpcmPath,0);
   const char* aacPath =env->GetStringUTFChars(jaccPath,0);

   encode(pcmPath,aacPath,channels,bitRate,jsampleRate);

   env->ReleaseStringUTFChars(jpcmPath,pcmPath);
   env->ReleaseStringUTFChars(jaccPath,aacPath);


}
