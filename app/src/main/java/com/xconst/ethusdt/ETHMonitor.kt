package com.xconst.ethusdt

/**
 * ETH 价格突变监控器组件 (JNI 包装类)
 *
 * @param windowSizeMs 时间窗口跨度（毫秒，例如 5000L 代表 5 秒）
 * @param threshold 突变阈值比例（例如 0.005 代表 0.5%）
 * @param maxCapacity 底层环形队列的最大物理容量（例如 2000，防止极端高频行情暴库）
 */
class ETHMonitor(windowSizeMs: Long, threshold: Double, maxCapacity: Int) : AutoCloseable {

    // 保存 C 端的结构体指针 (对应 C 层的 PriceMonitor*)
    private val nativeContext: Long = 1L
//
//    init {
//        nativeContext = createNative(windowSizeMs, threshold, maxCapacity)
//        if (nativeContext == 0L) {
//            throw RuntimeException("Failed to allocate memory for C PriceMonitor component.")
//        }
//    }

    /**
     * 基于时间戳流式更新价格并判断是否突变
     * @param price 当前最新的实时价格
     * @param timestamp 当前价格对应的毫秒级系统时间戳
     * @return true 代表发生突变，false 代表正常波动
     */
    fun updateByTime(price: Double, timestamp: Long): Boolean {
        return updateNativeWithTime(nativeContext, price, timestamp)
    }

    /**
     * 重置缓冲区，清除所有历史数据和极值状态
     */
    fun reset() {
//        resetNative(nativeContext)
    }

    /**
     * 释放底层 C 语言组件占用的堆外内存
     */
    fun destroy() {
        if (nativeContext != 0L) {
            freeNative(nativeContext)
        }
    }

    // 实现 AutoCloseable 接口，支持 Kotlin 的 .use { } 语法糖
    override fun close() {
        destroy()
    }

    // --- Kotlin 的 Native (JNI) 方法声明 ---

    // 对应 PriceMonitor* price_monitor_create(unsigned long long, double, int)
    private external fun createNative(windowMs: Long, threshold: Double, maxCapacity: Int): Long

    // 对应 bool price_monitor_update_by_time(PriceMonitor*, double, unsigned long long, bool*)
    private external fun updateNativeWithTime(pmPtr: Long, price: Double, timestamp: Long): Boolean

    // 对应 void price_monitor_reset(PriceMonitor*)
    private external fun resetNative(pmPtr: Long)

    // 对应 void price_monitor_free(PriceMonitor*)
    private external fun freeNative(pmPtr: Long)

    companion object {
        init {
            // 名字必须和 CMakeLists.txt 里的 add_library 名字完全一致
//            System.loadLibrary("pricemonitor")
        }
    }
}