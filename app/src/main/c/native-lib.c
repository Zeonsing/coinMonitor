#include <jni.h>
#include "price_monitor.h"

// 1. 创建实例的胶水
JNIEXPORT jlong JNICALL
Java_com_xconst_ethusdt_ETHMonitor_createNative(JNIEnv *env, jobject thiz, jlong window_ms, jdouble threshold, jint max_capacity) {
    PriceMonitor* pm = price_monitor_create((unsigned long long)window_ms, (double)threshold, (int)max_capacity);
    return (jlong)pm;
}

// 2. 时间更新的胶水
JNIEXPORT jboolean JNICALL
Java_com_xconst_ethusdt_ETHMonitor_updateNativeWithTime(JNIEnv *env, jobject thiz, jlong pm_ptr, jdouble price, jlong timestamp) {
    PriceMonitor* pm = (PriceMonitor*)pm_ptr;
    bool is_anomalous = false;
    price_monitor_update_by_time(pm, (double)price, (unsigned long long)timestamp, &is_anomalous);
    return is_anomalous ? JNI_TRUE : JNI_FALSE;
}

// 3. 重置缓冲区的胶水
JNIEXPORT void JNICALL
Java_com_xconst_ethusdt_ETHMonitor_resetNative(JNIEnv *env, jobject thiz, jlong pm_ptr) {
    PriceMonitor* pm = (PriceMonitor*)pm_ptr;
    price_monitor_reset(pm);
}

// 4. 释放内存的胶水
JNIEXPORT void JNICALL
Java_com_xconst_ethusdt_ETHMonitor_freeNative(JNIEnv *env, jobject thiz, jlong pm_ptr) {
    PriceMonitor* pm = (PriceMonitor*)pm_ptr;
    price_monitor_free(pm);
}