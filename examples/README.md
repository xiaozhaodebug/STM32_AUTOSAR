# 示例代码

本目录包含 STM32F407 AUTOSAR 项目的示例代码。

## 示例列表

### 1. CAN 发送示例 (`example_can_tx.c`)

演示如何发送 CAN 消息：
- 发送标准帧
- 发送扩展帧
- 周期性发送

### 2. CAN 接收示例 (`example_can_rx.c`)

演示如何接收 CAN 消息：
- 轮询方式接收
- 消息回显
- 按 ID 处理消息

### 3. UDS 诊断示例 (`example_uds.c`)

演示 UDS 诊断服务：
- 会话控制 (0x10)
- ECU 复位 (0x11)
- 读取 DID (0x22)
- 写入 DID (0x2E)
- 安全访问 (0x27)
- Tester Present (0x3E)

## 使用方法

将示例代码复制到您的项目中：

```bash
cp examples/example_can_tx.c src/
```

然后在主程序中包含并调用：

```c
#include "example_can_tx.h"

int main(void) {
    /* 初始化 */
    CanDriver_Init();
    DebugLog_Init();
    
    /* 发送 CAN 消息 */
    Example_CanSend();
    
    /* 主循环 */
    while (1) {
        Example_CanPeriodicSend();
    }
}
```

## 更多示例

欢迎提交您的示例代码！
