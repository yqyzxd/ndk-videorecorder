package com.wind.ndk.video.recorder

/**
 * FileName: RecordViewState
 * Author: wind
 * Date: 2023/6/13 15:06
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
data class RecordViewState(
    val song: String = "",
    val formattedTime: String=""
) {

    companion object {
        val Empty = RecordViewState("", "")
    }
}