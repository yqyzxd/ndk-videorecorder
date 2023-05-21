package com.wind.ndk.camera

import android.hardware.Camera
import android.view.Surface

class CameraPreviewScheduler(private val mCamera:VideoCamera) : CameraPreviewView.Callback,
    VideoCamera.Callback {
    private var mCameraFacingId=Camera.CameraInfo.CAMERA_FACING_FRONT
    init {

        mCamera.setCallback(this)
    }

    override fun onSurfaceCreated(surface: Surface) {
        nativeOnSurfaceCreated(surface,mCameraFacingId)
    }
    override fun onSurfaceChanged(width: Int, height: Int) {
        nativeOnSurfaceChanged(width,height)
    }
    override fun onSurfaceDestroyed() {
        nativeOnSurfaceDestroyed()
    }
    fun switchCamera(){
       val cameraId= if ( mCameraFacingId == Camera.CameraInfo.CAMERA_FACING_FRONT){
           Camera.CameraInfo.CAMERA_FACING_BACK
       }else{
           Camera.CameraInfo.CAMERA_FACING_FRONT
       }
        nativeSwitchCamera(cameraId)
    }



    fun release() {
        nativeRelease()
    }



    fun configCameraFromNative(cameraFacingId:Int):CameraInfo{
        mCameraFacingId=cameraFacingId
        return mCamera.configure(cameraFacingId)
    }

    fun startPreviewFromNative(textureId:Int){
        mCamera.setPreviewTexture(textureId)
        mCamera.startPreview()
    }
    fun updateTexImageFromNative():FloatArray{
        return mCamera.updateTexImage()
    }

    fun releaseCameraFromNative(){
        return mCamera.releaseCamera()
    }

    override fun onFrameAvailable() {
        nativeOnFrameAvailable()
    }



    private external fun nativeOnSurfaceCreated(surface: Surface,cameraFacingId: Int)
    private external fun nativeOnSurfaceChanged(width: Int, height: Int)
    private external fun nativeOnSurfaceDestroyed()
    private external fun nativeOnFrameAvailable()

    private external fun nativeRelease()
    private external fun nativeSwitchCamera(cameraFacingId: Int)
    external fun startEncode(h264File: String, width: Int, height: Int, videoBitrate: Int, frameRate: Int)
    external  fun stopEncode()

    companion object{
        init {
            System.loadLibrary("video_recorder")
        }
    }

}