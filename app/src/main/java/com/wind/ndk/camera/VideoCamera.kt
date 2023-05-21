package com.wind.ndk.camera

import android.app.Activity
import android.graphics.ImageFormat
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.hardware.Camera.CameraInfo
import android.util.Log
import android.view.Surface
import javax.microedition.khronos.opengles.GL11Ext

@Suppress("DEPRECATION")
class VideoCamera(private val mActivity: Activity) {

    private var mCamera: Camera? = null
    private var mSurfaceTexture: SurfaceTexture? = null

    private var mCallback: Callback? = null
    private var mVideoWidth = VIDEO_WIDTH_1280
    private var mVideoHeight = VIDEO_HEIGHT_720


    fun configure(cameraFacingId: Int): com.wind.ndk.camera.CameraInfo {
        if (mCamera != null) {
            releaseCamera()
        }
        try {
            mCamera = Camera.open(cameraFacingId)
            val parameters = mCamera?.parameters
            parameters?.apply {
                val supportedFormats = supportedPreviewFormats
                if (supportedFormats.contains(ImageFormat.NV21)) {
                    //NV21其实是“YUV420SP的格式，即UV是interleaved（交错UVUVUV）的存放”
                    previewFormat = ImageFormat.NV21
                } else {
                    throw CameraException("preview format error")
                }

                if (isSupportedSize(supportedPreviewSizes, mVideoWidth, mVideoHeight)) {
                    setPreviewSize(mVideoWidth, mVideoHeight)
                } else {
                    mVideoWidth = VIDEO_WIDTH_640
                    mVideoHeight = VIDEO_HEIGHT_480
                    if (isSupportedSize(supportedPreviewSizes, mVideoWidth, mVideoHeight)) {
                        setPreviewSize(mVideoWidth, mVideoHeight)
                    } else {
                        throw CameraException("preview size error")
                    }

                }

                if (supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                    focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO
                }


            }
            mCamera?.parameters = parameters
            mCamera?.setErrorCallback { error, _ ->
                println("ErrorCallback error:$error")
            }
            setCameraDisplayOrientation(cameraFacingId)
            return CameraInfo(mVideoWidth, mVideoHeight, getCameraFacing(cameraFacingId))
        } catch (e: Exception) {
            throw CameraException(e.message ?: "")
        }
    }

    //渲染出的画面是旋转的。
    // 解决方式一：通过Camera.setDisplayOrientation设置正确的旋转角度。设置Camera旋转角度，这样通过surfaceTexture的getTransformMatrix获取到的matrix就是带有旋转角度的
    //解决方式二：计算出旋转角度，设置正确的纹理坐标。
    private fun setCameraDisplayOrientation(cameraFacingId: Int) {
        val info = CameraInfo()
        Camera.getCameraInfo(cameraFacingId, info)
        val rotation: Int = mActivity.windowManager.defaultDisplay.rotation
        var degrees = 0
        when (rotation) {
            Surface.ROTATION_0 -> degrees = 0
            Surface.ROTATION_90 -> degrees = 90
            Surface.ROTATION_180 -> degrees = 180
            Surface.ROTATION_270 -> degrees = 270
        }

        var result: Int
        if (info.facing == CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360
            result = (360 - result) % 360 // compensate the mirror 前置摄像头镜像
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360
        }
        mCamera?.setDisplayOrientation(result)
    }


    private fun isSupportedSize(sizes: List<Camera.Size>, width: Int, height: Int): Boolean {
        var supported = false;
        sizes.forEach {
            if (it.width == width && it.height == height) {
                supported = true
            }
        }
        return supported
    }

    fun releaseCamera() {
        mSurfaceTexture?.setOnFrameAvailableListener(null)
        //需要从Camera移除SurfaceTexture，因为SurfaceTexture release后会进入'abandoned' state，
        // 若不移除日志中会打出如下Error Log： [SurfaceTexture-1-10493-0](id:28fd00000000,api:4,p:1089,c:10493) queueBuffer: BufferQueue has been abandoned
        mCamera?.setPreviewTexture(null)

        // puts the SurfaceTexture into the 'abandoned' state
        mSurfaceTexture?.release()
        mSurfaceTexture = null

        mCamera?.release()
        mCamera = null


    }

    fun setPreviewTexture(textureId: Int) {
        mSurfaceTexture = SurfaceTexture(textureId)
        mCamera?.apply {

            setPreviewTexture(mSurfaceTexture)
            mSurfaceTexture?.setOnFrameAvailableListener {
                mCallback?.onFrameAvailable()
            }
        }

    }

    fun startPreview() {
        mCamera?.startPreview()
    }

    companion object {
        const val VIDEO_WIDTH_640 = 640
        const val VIDEO_HEIGHT_480 = 480

        const val VIDEO_WIDTH_1280 = 1280
        const val VIDEO_HEIGHT_720 = 720

        @JvmStatic
        fun getCameraFacing(cameraId: Int): Int {
            var facing = 0
            val cameraInfo = CameraInfo()
            Camera.getCameraInfo(cameraId, cameraInfo)
            if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                facing = 1
            }
            return facing
        }

    }

    fun setCallback(callback: Callback) {
        this.mCallback = callback
    }

    fun updateTexImage(): FloatArray {
        var arr = floatArrayOf(
            1f, 0f, 0f, 0f,
            0f, 1f, 0f, 0f,
            0f, 0f, 1f, 0f,
            0f, 0f, 0f, 1f,
        )
        mSurfaceTexture?.updateTexImage()
        //获取纹理矩阵
        mSurfaceTexture?.getTransformMatrix(arr)

        return arr
    }

    interface Callback {
        fun onFrameAvailable()
    }
}