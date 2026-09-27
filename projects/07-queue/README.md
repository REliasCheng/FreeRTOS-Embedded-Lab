# 队列 | Queue

[`08_FreeRTOS_Queue`](course/08_FreeRTOS_Queue/) 创建两个队列，分别传递 `uint32_t` 和结构体数据。

## 数据流

```text
producer task → xQueueSend → queue storage → xQueueReceive → consumer task
```

Queue 按元素大小复制数据，让生产者与消费者保持独立。综合参考工程进一步使用队列连接 UART ISR、协议接收和发送任务。

详见 [Task Communication](../../docs/task-communication.md)。

