package com.wind.ndk.camera

import android.content.Context
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class CameraPreviewView constructor(context :Context, attrs: AttributeSet?=null, defStyleAttr:Int=0): SurfaceView(context,attrs,defStyleAttr),
    SurfaceHolder.Callback {
    private var mCallback:Callback?=null

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        mCallback?.onSurfaceCreated(holder.surface)
        println("surfaceCreated")
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        mCallback?.onSurfaceChanged(width,height)
        println("onSurfaceChanged")
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        mCallback?.onSurfaceDestroyed()
        println("surfaceDestroyed")
    }


    fun setCallback(callback: Callback){
        mCallback=callback
    }

    interface Callback {
        fun onSurfaceCreated(surface: Surface)
        fun onSurfaceChanged(width: Int,height: Int)
        fun onSurfaceDestroyed()
    }
}