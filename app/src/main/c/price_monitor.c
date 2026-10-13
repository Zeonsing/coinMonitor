#include "price_monitor.h"
#include <stdlib.h>

// --- 内部辅助函数：环形队列底层操作 ---
static MonotonicQueue create_queue(int capacity) {
    MonotonicQueue q;
    q.buffer = (PriceNode*)malloc(sizeof(PriceNode) * capacity);
    q.capacity = capacity;
    q.head = 0;
    q.tail = 0;
    q.size = 0;
    return q;
}

static void free_queue(MonotonicQueue* q) {
    if (q->buffer) {
        free(q->buffer);
        q->buffer = NULL;
    }
}

static inline bool is_empty(MonotonicQueue* q) { return q->size == 0; }
static inline PriceNode get_front(MonotonicQueue* q) { return q->buffer[q->head]; }
static inline PriceNode get_back(MonotonicQueue* q) { return q->buffer[(q->tail - 1 + q->capacity) % q->capacity]; }

static void pop_front(MonotonicQueue* q) {
    if (q->size > 0) {
        q->head = (q->head + 1) % q->capacity;
        q->size--;
    }
}

static void pop_back(MonotonicQueue* q) {
    if (q->size > 0) {
        q->tail = (q->tail - 1 + q->capacity) % q->capacity;
        q->size--;
    }
}

static void push_back(MonotonicQueue* q, PriceNode node) {
    q->buffer[q->tail] = node;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
}

static void reset_queue(MonotonicQueue* q) {
    if (q) {
        q->head = 0;
        q->tail = 0;
        q->size = 0;
    }
}

// --- 核心部件 API 实现 ---

/**
 * 创建监控器实例
 * @param window_ms 时间窗口跨度（毫秒，如 5000 表示 5 秒）
 * @param threshold 突变阈值比例
 * @param max_capacity 底层环形缓冲区开辟的物理槽位数量（空间换时间，防突发高频暴库，例如给 10000）
 */
PriceMonitor* price_monitor_create(unsigned long long window_ms, double threshold, int max_capacity) {
    PriceMonitor* pm = (PriceMonitor*)malloc(sizeof(PriceMonitor));
    if (!pm) return NULL;

    pm->window_ms = window_ms;
    pm->threshold = threshold;
    pm->capacity = max_capacity;

    pm->max_q = create_queue(max_capacity);
    pm->min_q = create_queue(max_capacity);

    pm->history = (PriceNode*)malloc(sizeof(PriceNode) * max_capacity);
    pm->hist_head = 0;
    pm->hist_tail = 0;
    pm->hist_size = 0;

    return pm;
}

void price_monitor_free(PriceMonitor* pm) {
    if (!pm) return;
    free_queue(&pm->max_q);
    free_queue(&pm->min_q);
    if (pm->history) free(pm->history);
    free(pm);
}

void price_monitor_reset(PriceMonitor* pm) {
    if (!pm) return;
    reset_queue(&pm->max_q);
    reset_queue(&pm->min_q);
    pm->hist_head = 0;
    pm->hist_tail = 0;
    pm->hist_size = 0;
}

// 核心算法：基于物理时间戳的流式检测
bool price_monitor_update_by_time(PriceMonitor* pm, double price, unsigned long long ms_timestamp, bool* is_anomalous) {
    if (!pm) return false;

    PriceNode new_node = { ms_timestamp, price };
    *is_anomalous = false;

    // 1. 判定突变：对比当前价格与窗口内现存的全局极值
    if (pm->hist_size > 0) {
        double current_max = get_front(&pm->max_q).price;
        double current_min = get_front(&pm->min_q).price;

        if ((price - current_min) / current_min > pm->threshold ||
            (current_max - price) / current_max > pm->threshold) {
            *is_anomalous = true;
        }
    }

    // 2. 根据物理时间戳，剔除已经滑出时间窗口的过期数据
    unsigned long long expire_timestamp = (ms_timestamp > pm->window_ms) ? (ms_timestamp - pm->window_ms) : 0;

    while (pm->hist_size > 0) {
        PriceNode oldest = pm->history[pm->hist_head];

        if (oldest.id < expire_timestamp) {
            // 从原始历史记录移除，使用真正的物理容量 pm->capacity 取模
            pm->hist_head = (pm->hist_head + 1) % pm->capacity;
            pm->hist_size--;

            // 同步从单调极值队列的头部移除对应过期的元素
            if (!is_empty(&pm->max_q) && get_front(&pm->max_q).id == oldest.id) {
                pop_front(&pm->max_q);
            }
            if (!is_empty(&pm->min_q) && get_front(&pm->min_q).id == oldest.id) {
                pop_front(&pm->min_q);
            }
        } else {
            // 队头未过期，说明后面的更未过期，退出清理
            break;
        }
    }

    // 4. 安全保护：防止高频极端行情下撑爆物理数组容量
    if (pm->hist_size >= pm->capacity) {
        // 如果挤满了，强行把最老的一个数据弹掉，腾出位置
        pm->hist_head = (pm->hist_head + 1) % pm->capacity;
        pm->hist_size--;
    }

    // 3. 【完美补齐】维护 max_q 的单调性（保持严格递减）
    while (!is_empty(&pm->max_q) && get_back(&pm->max_q).price <= price) {
        pop_back(&pm->max_q);
    }
    push_back(&pm->max_q, new_node);

    // 【完美补齐】维护 min_q 的单调性（保持严格递增）
    while (!is_empty(&pm->min_q) && get_back(&pm->min_q).price >= price) {
        pop_back(&pm->min_q);
    }
    push_back(&pm->min_q, new_node);

    // 5. 将新数据存入历史记录
    pm->history[pm->hist_tail] = new_node;
    pm->hist_tail = (pm->hist_tail + 1) % pm->capacity;
    pm->hist_size++;

    return true;
}