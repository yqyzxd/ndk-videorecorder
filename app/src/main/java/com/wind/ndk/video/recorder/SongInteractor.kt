package com.wind.ndk.video.recorder

import com.wind.ndk.SubjectInteractor
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

class SongInteractor : SubjectInteractor<String, String>() {
    override fun createObservable(params: String): Flow<String> {
        return flow{
            emit(params)
        }
    }

}