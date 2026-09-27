工程说明：

	工程用FreeRTOS进行任务管理, 共有如下任务
	- App_Input_task 用于处理输入事件(按键)   20ms
	- App_Balance_task 用于处理平衡控制       100ms
	- App_OLED_task 用于处理OLED显示          动态 3000ms
	- App_Protocol_task 用于处理当前位置的发送任务  
	- App_Protocol_pid_task 用于处理PID参数的发送任务
	- App_Protocol_recv_task 用于处理接收到的数据

	共有如下消息通讯机制
	- xOLED_Refresh_Semaphore: 处理OLED刷新显示的信号量
		如果需要刷新OLED, 通过xSemaphoreGive或xSemaphoreGiveFromISR发送信号量, 
		在OLED任务里处理真正的刷新任务, 避免在中断里直接操作OLED, 控制OLED的刷新频率

	- xProtocolSendQueue: 用于发送数据的队列, 通过串口发送
		如果需要发送数据, 通过xQueueSend将数据放到队列, 发送数据

	- xProtocolRecvQueue: 用于接收并处理数据的队列
		由于接收数据的速度可能比较快, 为了避免数据丢失, 并且避免阻塞串口中断
		在中断里用xQueueSendFromISR将串口收到的数据放到队列中, 在单独的任务里解析消息

	
引脚接线：
	
	激光测距模块(I2C0):

		SDA: PB7
		SCL: PB6
		GND
		3V3

	舵机:

		信号线: PD15
		5V
		GND



各文件目录说明：
	User：系统文件，包括main.c、systick.c、gd32f4xx_it.c
	Project：工程生成的文件，包括工程启动项、hex文件、编译文件
	Hardware：自己添加的各类底层驱动文件
	Firmware：官方标准库
	Doc：工程说明文档