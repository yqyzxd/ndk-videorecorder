package com.wind.ndk.video.recorder

import com.wind.ndk.SubjectInteractor
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

/**
 * FileName: RecordingTimeInteractor
 * Author: wind
 * Date: 2023/6/13 15:59
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class RecordingTimeInteractor : SubjectInteractor<Int, String>() {

    override fun createObservable(params: Int): Flow<String> {
        return flow{
            emit(format(params))

        }
    }

    private fun format(secs: Int): String {
        val hours = secs / 3600
        val remainSec = secs % 3600
        val mins = remainSec / 60
        val secs = remainSec % 60

        return String.format("%02d:%02d:%02d", hours, mins, secs)
    }
}


