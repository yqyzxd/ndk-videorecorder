package com.wind.ndk.video.recorder

import androidx.navigation.NavController
import androidx.navigation.NavOptions
import androidx.navigation.NavOptionsBuilder
import androidx.navigation.Navigator

/**
 * FileName: NavExt
 * Author: wind
 * Date: 2023/5/21 21:20
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
typealias NavResultCallback<T> =(T)->Unit
private const val navResultCallbackKey="navResultCallbackKey"
fun <T> NavController.setNavResultCallback(callback:NavResultCallback<T>){
    currentBackStackEntry?.savedStateHandle?.set(navResultCallbackKey,callback)
}
fun <T> NavController.getNavResultCallback():NavResultCallback<T>?{
    return previousBackStackEntry?.savedStateHandle?.remove(navResultCallbackKey)
}

fun <T> NavController.popBackStack(result:T){
    getNavResultCallback<T>()?.invoke(result)
    popBackStack()
}

fun <T> NavController.navigateForResult(
    route:String,
    callback:NavResultCallback<T>,
    navOptions: NavOptions?=null,
    navigatorExtras: Navigator.Extras?=null
){
    setNavResultCallback(callback)
    navigate(route,navOptions,navigatorExtras)
}

fun <T> NavController.navigateForResult(
    route:String,
    callback:NavResultCallback<T>,
    builder:NavOptionsBuilder.()->Unit
){
    setNavResultCallback(callback)
    navigate(route,builder)
}
