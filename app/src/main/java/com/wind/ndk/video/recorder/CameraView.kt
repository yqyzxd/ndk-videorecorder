package com.wind.ndk.video.recorder

import android.app.Activity
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import com.wind.ndk.camera.CameraPreviewScheduler
import com.wind.ndk.camera.CameraPreviewView
import com.wind.ndk.camera.VideoCamera

/**
 * FileName: CameraPreview
 * Author: wind
 * Date: 2023/5/21 19:22
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */

@Composable
fun CameraView(modifier: Modifier=Modifier,cameraPreviewScheduler:CameraPreviewScheduler){


    AndroidView(
        factory = { context ->
            CameraPreviewView(context).apply {
                setCallback(cameraPreviewScheduler)
            }
        },
        modifier =modifier,

    )

}