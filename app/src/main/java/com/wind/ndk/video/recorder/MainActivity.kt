@file:OptIn(ExperimentalMaterial3Api::class)

package com.wind.ndk.video.recorder

import android.Manifest
import android.os.Bundle
import android.view.WindowManager
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.wind.ndk.video.recorder.ui.theme.NdkvideorecorderTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestPermissions(arrayOf(Manifest.permission.CAMERA,Manifest.permission.RECORD_AUDIO)) {
            setContent {
                NdkvideorecorderTheme {
                    if (it.isEmpty()) {
                        App()
                    }
                }
            }
        }
    }

    private fun requestPermissions(permissions: Array<String>, onResult: (List<String>) -> Unit) {
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { result ->
            val failed = result.filter { !it.value }.keys
            onResult(failed.toList())
        }.launch(permissions)
    }

}

@Composable
fun App() {
    VideoNavHost()
}

@Composable
fun VideoNavHost(
    modifier: Modifier = Modifier,
    startDestination: String = "record_screen",
    navController: NavHostController = rememberNavController()
) {
    NavHost(
        modifier = modifier,
        navController = navController,
        startDestination = startDestination
    ) {
        composable("record_screen") {
            RecordScreen(onSelectSongClick = { navResultCallback ->
                navController.navigateForResult(
                    route = "songs_screen",
                    callback = navResultCallback
                )
            })
        }
        composable("songs_screen") {
            SongsScreen(onClickBack = {
                navController.popBackStack()
            }, onItemSelected = {song->
                navController.popBackStack(song)
            })
        }
    }
}
