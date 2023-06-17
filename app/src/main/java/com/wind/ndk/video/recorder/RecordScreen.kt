package com.wind.ndk.video.recorder


import android.app.Activity
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.PlayArrow
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.constraintlayout.compose.ConstraintLayout
import androidx.lifecycle.HasDefaultViewModelProviderFactory
import androidx.lifecycle.viewmodel.CreationExtras
import androidx.lifecycle.viewmodel.MutableCreationExtras
import androidx.lifecycle.viewmodel.compose.LocalViewModelStoreOwner
import androidx.lifecycle.viewmodel.compose.viewModel
import com.wind.ndk.audio.recorder.AudioRecorder
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
    var song by rememberSaveable { mutableStateOf("") }
    val context = LocalContext.current
    val previewScheduler = remember(context) {
        CameraPreviewScheduler(VideoCamera(context as Activity))
    }
    val audioRecorder = remember(context) {
        AudioRecorder
    }
    val owner = LocalViewModelStoreOwner.current
    val defaultExtras =
        (owner as? HasDefaultViewModelProviderFactory)?.defaultViewModelCreationExtras
            ?: CreationExtras.Empty

    val extras = MutableCreationExtras(defaultExtras).apply {
        set(VideoRecordViewModel.CAMERA_PREVIEW_KEY, previewScheduler)
        set(VideoRecordViewModel.AUDIO_RECORDER_KEY, audioRecorder)

    }

    Scaffold { paddingValues ->
        RecordScreen(
            viewModel= viewModel(factory = VideoRecordViewModel.Factory, extras = extras),
            curSong = song,
            modifier = Modifier.padding(paddingValues),
            onSongChange = { song = it },
            cameraPreviewScheduler=previewScheduler,
            onSelectSongClick = onSelectSongClick,
        )
    }
}

@Composable
fun RecordScreen(
    viewModel: VideoRecordViewModel,
    curSong: String,
    onSongChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    cameraPreviewScheduler: CameraPreviewScheduler,
    onSelectSongClick: (NavResultCallback<String>) -> Unit
) {
    val viewState by viewModel.state.collectAsState()
    RecordScreen(
        state=viewState,
        curSong = curSong,
        onSongChange = onSongChange,
        modifier = modifier,
        onClickRecord = viewModel::onClickRecord,
        cameraPreviewScheduler=cameraPreviewScheduler,
        onSelectSongClick = onSelectSongClick,
    )
}

@Composable
fun RecordScreen(
    state:RecordViewState,
    curSong: String,
    onSongChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    onClickRecord: () -> Unit,
    cameraPreviewScheduler: CameraPreviewScheduler,
    onSelectSongClick: (NavResultCallback<String>) -> Unit,
) {



    ConstraintLayout(modifier.fillMaxSize()) {
        val (songBtn, switchBtn, surfaceView, recordBtn, musicPanel,recordingText) = createRefs()

        CameraView(cameraPreviewScheduler = cameraPreviewScheduler,
            modifier = Modifier
                .fillMaxSize()
                .constrainAs(surfaceView) {
                    start.linkTo(parent.start)
                    end.linkTo(parent.end)
                })

        Button(onClick = {
            onSelectSongClick { song ->
                onSongChange(song)
            }
        }, modifier = Modifier.constrainAs(songBtn) {
            top.linkTo(parent.top, margin = 16.dp)
            start.linkTo(parent.start, margin = 16.dp)
        }) {
            Text(text = "选歌")
        }
        println("curSong:$curSong")
        if (curSong != "") {
            MusicPlayerPanel(curSong, modifier = Modifier.constrainAs(musicPanel) {
                top.linkTo(songBtn.bottom, margin = 16.dp)
                start.linkTo(parent.start)
                end.linkTo(parent.end)
            })
        }
        Button(onClick = { /*TODO*/ }, modifier = Modifier.constrainAs(switchBtn) {
            top.linkTo(parent.top, margin = 16.dp)
            end.linkTo(parent.end, margin = 16.dp)
        }) {
            Text(text = "切换")
        }
        Button(onClick = {onClickRecord()}, modifier = Modifier.constrainAs(recordBtn) {
            bottom.linkTo(parent.bottom, margin = 16.dp)
            start.linkTo(parent.start)
            end.linkTo(parent.end)
        }) {
            Text(text = "录制")
        }
        if (state.formattedTime != ""){
            Text(color=Color.Red,text = state.formattedTime,modifier= Modifier.constrainAs(recordingText){
                start.linkTo(parent.start)
                end.linkTo(parent.end)
                top.linkTo(parent.top, margin = 16.dp)
            })
        }


    }

}

@Composable
fun MusicPlayerPanel(song: String, modifier: Modifier = Modifier) {
    Surface(
        color = Color.Cyan,
        modifier = modifier
            .height(60.dp)
            .fillMaxWidth(0.8f),
        shape = MaterialTheme.shapes.small
    ) {
        Column(
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(text = song)

            Row {
                IconButton(onClick = { /*TODO*/ }) {
                    Icon(imageVector = Icons.Outlined.PlayArrow, contentDescription = null)
                }
            }
        }
    }

}