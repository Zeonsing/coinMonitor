#ifndef PRICE_MONITOR_H
#define PRICE_MONITOR_H

#include <stdbool.h>

// 定义数据点结构体
typedef struct {
    unsigned long long id; // 存储真实的毫秒级时间戳
    double price;          // 价格
} PriceNode;

// 单调队列/环形缓冲区结构体
typedef struct {
    PriceNode* buffer;     // 数组指针
    int capacity;          // 物理内存最大容量
    int head;              // 队头指针
    int tail;              // 队尾指针
    int size;              // 当前有效元素数量
} MonotonicQueue;

// 价格监控器核心部件结构体
typedef struct {
    unsigned long long window_ms; // 时间窗口大小（毫秒，如 5000 代表 5 秒）
    double threshold;             // 突变阈值比例（如 0.005 表示 0.5%）
    int capacity;                 // 底层所有环形数组的真实物理内存分配容量

    MonotonicQueue max_q;  // 递减队列（维护最大值）
    MonotonicQueue min_q;  // 递增队列（维护最小值）

    PriceNode* history;    // 存储原始数据的环形缓冲区
    int hist_head;
    int hist_tail;
    int hist_size;
} PriceMonitor;

// API 接口定义 - 完全适配时间戳模式
PriceMonitor* price_monitor_create(unsigned long long window_ms, double threshold, int max_capacity);
void price_monitor_free(PriceMonitor* pm);
bool price_monitor_update_by_time(PriceMonitor* pm, double price, unsigned long long ms_timestamp, bool* is_anomalous);
void price_monitor_reset(PriceMonitor* pm);

#endif // PRICE_MONITOR_H