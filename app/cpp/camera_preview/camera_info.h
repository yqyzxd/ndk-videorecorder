//
// Created by 史浩 on 2023/5/7.
//

#ifndef NDK_CAMERARECORDER_CAMERA_INFO_H
#define NDK_CAMERARECORDER_CAMERA_INFO_H


class CameraInfo {
public:
    CameraInfo(int previewWidth,int previewHeight,int cameraFacingId):previewWidth(previewWidth),previewHeight(previewHeight),cameraFacingId(cameraFacingId){};
    ~CameraInfo();

    int previewWidth;
    int previewHeight;
    int cameraFacingId;

};


#endif //NDK_CAMERARECORDER_CAMERA_INFO_H
