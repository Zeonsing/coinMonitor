#include <stdio.h>
#include "price_monitor.h"

int main() {
    // 创建一个窗口大小为 5，突变阈值为 1% (0.01) 的监控器
    PriceMonitor* pm = price_monitor_create(5, 0.01);

    // 模拟高频 ETH 价格流
    double prices[] = {3000.0, 3000.5, 3001.0, 2999.5, 3000.0, 3040.0, 3041.0, 2950.0};
    int n = sizeof(prices) / sizeof(prices[0]);

    for (int i = 0; i < n; i++) {
        bool is_anomalous = false;
        price_monitor_update(pm, prices[i], &is_anomalous);

        printf("收到价格: %.2f -> %s\n",
               prices[i],
               is_anomalous ? "🚨 【价格突变报警！】" : "正常波动");
    }

    price_monitor_free(pm);
    return 0;
}