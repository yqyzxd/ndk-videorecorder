package com.wind.ndk.video.recorder


import android.app.Activity
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.constraintlayout.compose.ConstraintLayout
import com.wind.ndk.camera.CameraPreviewScheduler
import com.wind.ndk.camera.VideoCamera

/**
 * FileName: RecordScreen
 * Author: wind
 * Date: 2023/5/21 17:36
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun RecordScreen(onSelectSongClick: (NavResultCallback<String>) -> Unit) {

    Scaffold { paddingValues ->
        RecordScreen(
            modifier = Modifier.padding(paddingValues), onSelectSongClick = onSelectSongClick
        )
    }
}

@Composable
fun RecordScreen(
    modifier: Modifier = Modifier, onSelectSongClick: (NavResultCallback<String>) -> Unit
) {

    var curSong = remember { mutableStateOf("") }
    val context = LocalContext.current
    val previewScheduler = remember(context) {
        CameraPreviewScheduler(VideoCamera(context as Activity))
    }
    ConstraintLayout(modifier.fillMaxSize()) {
        val (songBtn, switchBtn, surfaceView, recordBtn,musicPanel) = createRefs()

        CameraView(cameraPreviewScheduler = previewScheduler,
            modifier = Modifier
                .fillMaxSize()
                .constrainAs(surfaceView) {
                    start.linkTo(parent.start)
                    end.linkTo(parent.end)
                })

        Button(onClick = {
            onSelectSongClick { song ->
                //显示music player ui
                println("selected song:$song")
                curSong.value = song
            }
        }, modifier = Modifier.constrainAs(songBtn) {
            top.linkTo(parent.top, margin = 16.dp)
            start.linkTo(parent.start, margin = 16.dp)
        }) {
            Text(text = "选歌")
        }

        if (curSong.value != "") {
            MusicPlayerPanel(curSong.value,modifier=Modifier.constrainAs(musicPanel){
                top.linkTo(songBtn.bottom, margin = 16.dp)
            })
        }
        Button(onClick = { /*TODO*/ }, modifier = Modifier.constrainAs(switchBtn) {
            top.linkTo(parent.top, margin = 16.dp)
            end.linkTo(parent.end, margin = 16.dp)
        }) {
            Text(text = "切换")
        }
        Button(onClick = { /*TODO*/ }, modifier = Modifier.constrainAs(recordBtn) {
            bottom.linkTo(parent.bottom, margin = 16.dp)
            start.linkTo(parent.start)
            end.linkTo(parent.end)
        }) {
            Text(text = "录制")
        }

    }

}

@Composable
fun MusicPlayerPanel(song: String,modifier: Modifier=Modifier) {
    Row(
        modifier = modifier
            .height(30.dp)
            .fillMaxWidth(0.8f)
    ) {
        Text(text = song)
    }
}