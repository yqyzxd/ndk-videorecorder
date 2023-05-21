package com.wind.ndk.video.recorder

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

/**
 * FileName: SongsScreen
 * Author: wind
 * Date: 2023/5/21 19:58
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SongsScreen(onClickBack:()->Unit,onItemSelected:(String)->Unit) {
    Scaffold(topBar = { TopAppBar(
        title = { Text(text = "歌曲") },
        navigationIcon={ IconButton(onClick = onClickBack) {
            Icon(imageVector = Icons.Default.ArrowBack, contentDescription = null)
        }}
    )
    }) { paddingValues ->
        val songs = listOf("踢出个未来", "总以为来日方长", "孤勇者", "天地龙鳞", "酒干倘卖无")
        LazyColumn(modifier = Modifier.padding(paddingValues)) {
            itemsIndexed(songs) { index, item ->
                Item(index, item,onItemSelected)
            }
        }
    }
}

@Composable
fun Item(index: Int, content: String,onItemSelected:(String)->Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.Start,
        modifier = Modifier
            .height(30.dp)
            .fillMaxWidth().clickable { onItemSelected(content) }
    ) {
        Text(text = index.toString(), Modifier.padding(start = 16.dp))
        Text(text = content, modifier = Modifier.padding(start = 16.dp))
    }
}

@Preview
@Composable
fun ItemPreview() {
    Item(index = 1, content = "踢出个未来", onItemSelected = {})
}