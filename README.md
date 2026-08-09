<!--
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-07 19:49:18
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-09 21:41:39
 * @FilePath: \f4\f4_cubemx配置.md
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
-->

# STM32F405RGT6

## 一、时钟配置
SYS 时钟源用TIM1  
RCC HSE用外部晶振  
时钟配置，拉高频率跑满芯片性能  
外频用HSE，先降频到1-2MHz，之后再升频到168MHz，先降频是为了进PLL锁相环，之后再升频是为了让芯片跑满性能。  
AHB（高速总线）：配置为 /1。内存、DMA 和 CPU 核心一样硬核，直接享受 168MHz 满速待遇。  
APB1（低速外设总线）：物理极限是 42MHz。所以必须配置为 /4（168 / 4 = 42MHz），一点没浪费。
APB2（高速外设总线）：物理极限是 84MHz。所以配置为 /2（168 / 2 = 84MHz），同样顶格拉满。  
给定时器开了“后门”，让最高级的定时器（如 TIM1/TIM8，挂在 APB2 上）依然能以 168MHz 的满血速度运行  
Project Manager 调堆栈，勾选Generate peripheral initialization as a pair of '.c/.h' files，生成.c和.h文件，方便后续使用。  

![alt text](image-4.png)


## 二、GPIO
### 1. GPIO输出
#### cubemx配置  
看原理图  
output level:MX_GPIO_Init()执行完成后，引脚的默认输出电平高低  
mode:输出模式 推挽和开漏  
推挽：内部信号直接驱动输出端口，输出高电平时，内部信号给高，NMOS截至，PMOS导通，对外输出高电平；输出低电平时，内部信号给低，NMOS导通，PMOS截至，对外输出低电平。  
既具有高电平驱动能力，又具有低电平驱动能力。MOS管的导通电阻很小，输出端口的驱动能力很强，MOS“推”上去的，会比较快。  
![alt text](image-1.png)  
开漏：上方没有PMOS，输出低电平时和推挽类似，NMOS导通，引脚被下拉到低电平；输出高电平时NMOS截至，但是上方也没有PMOS，所以引脚现在的状态是浮空的，外部需要上拉电阻将引脚拉高。 

![alt text](image-2.png)  
具有低电平驱动能力，不直接具有高电平驱动能力，只能接地，拉完了，但是有一些场景需要开漏输出，“线与”特性与多机并联I2C总线：SDA数据线，SCL时钟线，Master 想要发数据 1：它内部的 PMOS 强力导通，把蓝色的 SDA 整根线强行拉到 Vcc，Slave 1 此时刚好想要回一个 0：它内部的 NMOS 强力导通，把同一根蓝色的 SDA 线强行拉到 GND（0V），结果：Master的VCC和Slave 1的GND相连，短路了，通信瘫痪、引脚烧毁。I2C用开漏输出是怎么解决这个问题的：（自己查）“线与特性”；输出5V。 
![alt text](image-3.png)  
PULL UP/PULL DOWN：芯片内部上拉/下拉电阻，给引脚一个默认电平，避免引脚悬空，与前面的推挽输出/开漏输出属于“并联”关系，相当于在引脚上除了输出电路再挂载一个默认的电阻，在输出电路初始化前或者是输出电路浮空时给引脚一个默认的电平，引脚浮空：很容易受外部电磁扰动，可能会产生不好的影响要尽量避免浮空状态存在，在输入部分还会提，影响会更大一些。  
output speed:输出速度，影响输出端口的上升沿和下降沿的速度，速度越快，电流越大，功耗越大，对于灯、蜂鸣器这种设备，速度不需要太快，选择低速即可；对于通信接口，速度如果太慢可能会导致波形边缘失真，严重点会导致通信乱码，盲目的给高速也可能会因为电流过大，产生高频电磁干扰，也会产生一些不好的影响。  
User Label：取名字，给这个引脚取个别名，取好名字后会在main.h中生成对应的宏定义，方便后续使用。
``` C
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
```h723zet6


#### HAL库函数
write_pin：写引脚电平  
toggle_pin：翻转引脚电平  
  
Port（端口）和Pin（引脚）：一个 Port 通常包含 16个 物理引脚。这 16 个引脚在单片机内部共享着同一套时钟线、电源基准，以及一组连续的硬件控制寄存器，16个引脚对应寄存器的低16位。通常不同端口、不同引脚号对应的该引脚的功能会有些许不同，A一般功能会比B多一些，也有可能有些功能是B有而A没有的。具体要看芯片的手册上对功能的描述。除了一些专有引脚，大部分引脚做GPIO输入输出都是没问题的。硬件组在画板子的时候一般也会考虑到引脚的功能，选择对应的引脚，参考原理图上对应引脚的设置就可以  

流水灯、蜂鸣器  

``` C
HAL_GPIO_WritePin(GPIOA, GPIO_Pin_1, GPIO_PIN_SET)
HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
HAL_GPIO_TogglePin(GPIOD, GPIO_Pin_4)
```

### 2. GPIO输入
#### cubemx配置
依旧看原理图，U3做输入，没有预留的GPIO输入口，找个串口魔改一下，既有vcc又有gnd，一个串口两个引脚TX RX都可以改成GPIO输入用来外部信号的输入。  
引脚配置成GPIO_EXTI，EXTI是外部中断，这个模式是包含了GPIO_Input的全部功能，但是比输入又多了一个和NVIC（中断控制器）相关的功能，GPIO_Input模式只是单纯的输入功能，GPIO_EXTI模式是输入功能+中断功能。  
中断的理解：自学。  
简化版：条件触发，CPU保护程序运行现场，程序被打断，程序跳转到中断服务函数，执行完毕后返回原来程序运行现场继续执行。  
External Interrupt Mode：外部中断模式，符合条件时会触发中断。  
External Event Mode：外部事件模式，和中断模式类似，但是没有中断功能，只有事件功能，事件：条件触发，直接调用另一个外设，不过cpu，也不打断程序运行。  
条件：上升沿、下降沿、上升+下降  
Pull up/Pull down: 依旧默认下拉还是上拉，浮空状态引脚易受外部电磁扰动，可能会误触发中断。  
  
#### HAL库函数
read_pin：读引脚电平

``` C
pin_state = HAL_GPIO_ReadPin(GPIOE, OpticalSide0_Pin);
```  
轮询读取，单线程：实时性就没法看；多线程：占一个任务，而且效率也不高。  

外部中断：  
中断向量表跳转至对应handler -> HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)  
IRQ(Interrupt Request中断请求)  
HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  

中断加读取检查：上升下降均触发，触发后readpin确定触发状态，中断回调函数内部只做信息处理，不能放任何延时，包括任何等待类似于while(.....)，定义变量时加volatile，禁止使用动态分配内存malloc。

## 三、定时器

最基础的定时器功能：计数，计数到指定值后触发中断，回调函数里做一些事情。  
#### cubemx配置

TIM2时钟源：Internal Clock，内部时钟源，芯片内部的时钟线直接给定时器供时钟。  
PSC（Prescaler，预分频器）：定时器的计数速度 = 定时器时钟 / (PSC + 1)，PSC越大，计数速度越慢。  
计数模式：向上计数，计数器从0开始计数，计数到ARR（Auto Reload Register，自动重装载寄存器）指定的值后触发中断，计数器清零重新开始计数。  
ARR：计数器计数到ARR指定的值后溢出，计数器清零重新开始计数。
内部时钟分频：不用管，一般做滤波电路会用到  
自动重装预加载：代码里改ARR，可以暂存到当前数完，就不会发生溢出错乱。（从1000->500）但是定时器数到了600了已经。  
主从同步模式：不用开，各个定时器直接不用联动  
定时器计数溢出事件计算公式：(PSC+1)*(ARR+1)/fclk  
预分频系数+1是为了避免没设置预分频从0开始的情况，ARR+1是因为计数器从0开始计数，fclk是定时器时钟频率。TIM2时钟源是APB1，APB1分频是4，TIM2时钟源 = 168MHz / 4 *2= 84MHz。
开中断  


#### 中断函数
初始化

``` C
	if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	{
        Error_Handler();
    }
```

HAL_TIM_PeriodElapsedCallback，对cubemx的妥协，因为把 TIM1 设为了 HAL 库的系统心跳，CubeMX 会为了保证系统能跑，自动在 main.c里面写死了一个没有 __weak 的强回调函数：

#### PWM

pwm的概念：脉冲宽度调制：单片机的引脚输出只有两个状态，高电平（3.3v）和低电平，pwm就是通过改变高电平和低电平的时间比例来模拟出一个连续变化的模拟量。比如：一秒钟内高电平持续 0.5 秒，低电平持续 0.5 秒，高低电平是不断交替变化的，那么这个 pwm 的占空比就是 50%，输出给一个led灯就会给人一种“半亮”的感觉，如果高电平持续 0.8 秒，低电平持续 0.2 秒，那么这个 pwm 的占空比就是 80%。
占空比：高电平持续时间  
PA6 PA7 TIM3_CH1 TIM3_CH2  
开自动重装预加载  
MODE 1:只要计数值 CNT 小于 你设定的 CCR（比较寄存器） 值，引脚就输出“有效电平”；大于等于 CCR 时，输出“无效电平”。  
2反着来  
Pulse 初始CCR值，初始占空比  
Output compare preload —— 占空比修改防撕裂，跟重装预加载类似，也是为了避免占空比修改时，计数器还没数完就溢出了，导致占空比错乱。  
CH Polarity：引脚极性，给高就是这个引脚输出高电平，给低就是这个引脚输出低电平。  

```C
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500);  
```
呼吸灯  

作业
可用的外设：GPIO输入（一个按键）中断触发以及readpin读电平状态、GPIO输出（LED1-4、beep）、定时器中断、定时器pwm呼吸灯（LED3-4）;  
复现所有内容



## 四、串口（UART）
#### 串行通信
串行通信：数据是排着队，一个 bit 接着一个 bit，在同一根信号线上依次发出去的（USB、RS232、RS485、CAN、I2C、SPI、UART等），速度慢，成本低，线少，传输距离比较短，抗干扰能力不强  
并行通信：几根线并排，同时传送多个bit数据，速度快，但是成本高，线多，也容易干扰，要保证各条线之间的时序和电气特性一致。  
关于串行、并行、差分、单端、全双工、半双工、同步、异步，详细内容自行查阅资料了解  
![alt text](image-5.png)  
  
UART ：只需要两根线（TX和RX）即可完成数据传输，但是实际接线还要有GND VCC，单端通信，每根线单独传递信息，例如：A的RX断掉了，TX还是可以接着发数据，B也一样可以收到正确的数据，只不过A收不到B发的数据。缺点就是很容易干扰  
#### cubemx配置
Asynchronous（异步模式）两线，一根TX，一根RX，通信双方约定好通信速率、数据位、校验位、停止位等参数，通信双方各发各的，即UART。  
Single Wire (Half-Duplex)（单线半双工模式）  
Synchronous（同步模式），比异步模式多一根时钟线，即USART，类似于阉割过的SPI通信，不如直接用SPI，虽然硬件支持但是一般不用这个模式。    

波特率：每秒钟传输的二进制比特（bit）数，115200是对于普通的芯片、线材来讲比较稳妥且快速的一个波特率，太高了可能会因为芯片、线材、干扰等原因导致通信不稳定。为什么是115200：旧时代的遗留物，老式串行通信一般1200 2400，后面为了保持向下兼容就继续采用了1200的倍数  

字长：一个字节的大小、8位  
校验位：无校验  
停止位：1位

虽然是1位1位发，但是发送的时候会做一个小小的打包，一个字节后加上一个停止位或者再加上校验位，接收端就可以根据停止位和校验位把每个字节拼好
大数据包：包头包尾

过采样：对于115200波特率，每个bit的时间是8.68us，过采样就是在这个时间内采样多次，取多数值作为最终结果，过采样越高，抗干扰能力越强，但是通信速率会降低。

RX给个上拉，TX不用管  
NVIC开中断。  

#### 收消息  

三种：
1. 阻塞接收 HAL_UART_Receive()；拉完了，死等，整个程序都等着收消息。知道就行。  
2. 定长中断接收 HAL_UART_Receive_IT()；触发RXNE（接受寄存器非空）中断，提前知道要收几个字节的情况下，可以做到收够这几个字节后触发中断，但如果不知道消息多长，就只能一个字节触发一次，自己去处理。  
3. 空闲中断接收 HAL_UARTEx_ReceiveToIdle_IT()：触发IDLE（空闲）中断，空闲中断：接收寄存器空了一段时间后，意味着数据传输结束，触发中断，很方便的硬件机制，可以处理数据消息不定长的情况。  

##### 定长中断接收
``` C
    HAL_UART_Receive_IT(&huart1, rx_buffer, 5);//初始化后启动第一次接收
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//中断回调 

    if (huart->Instance == USART1) 
    {
        // 校验一下包头是不是正确的
        if(rx_buffer[0] == 0xAA) {
            // 解析数据...
        }
        
        // 重启接收
        HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
    }
```
数据包里几个1就响几声蜂鸣  

##### 空闲中断接收
``` C
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));//初始化后启动第一次接收
    
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)//中断回调 

    if (huart->Instance == USART1) 
    {
        // 校验一下包头是不是正确的
        if(rx_buffer[0] == 0xAA) {
            // 解析数据...
            
        }
        
        // 重启接收
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
    }
```

#### 发送消息
``` C
    HAL_UART_Transmit(&huart1, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);//阻塞式发送死等消息发完，最后一位是超时时间，一个字节理论耗时0.086ms，留出3-5倍的余量  

    HAL_UART_Transmit_IT(&huart1, tx_data, len);//中断发送，cpu消耗依旧很大，每发一个字节cpu触发一次中断，处理完后继续发下一个字节，效率不高，比不上dma也没比阻塞强多少  
```



#### DMA
dma在串口接收的应用，数据搬运不过cpu，给cpu减负，同时可以
关于dma的介绍：https://blog.csdn.net/as480133937/article/details/104927922

#### 收消息

##### 定长dma接收
```C
    HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);//初始化后启动第一次接收
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//中断回调 

    if (huart->Instance == USART1) 
    {
        // 校验一下包头是不是正确的
        if(rx_buffer[0] == 0xAA) {
            // 解析数据...
        }
        
        // 重启接收
        HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);
    }
```  

##### 空闲中断dma接收
```C
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));//初始化后启动第一次接收
    
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)//中断回调 

    if (huart->Instance == USART1) 
    {
        // 校验一下包头是不是正确的
        if(rx_buffer[0] == 0xAA) {
            // 解析数据...
            
        }
        
        // 重启接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
    }
```  
从使用上来说，空闲中断dma接收是最方便的，数据包不定长，cpu负担小，效率高。  
基本上可以无脑用dma接收，但是it会更简单一些，dma出问题it也要会。  
进阶使用，空闲中断+dma的循环模式+环形队列（软件层，收不定长消息，鲁棒性强），双缓存区dma（硬件层，0丢包、0错位）

发送消息
``` C
    HAL_UART_Transmit_DMA(&huart1, tx_buffer, sizeof(tx_buffer));//dma发送，cpu消耗很小，数据搬运不过cpu，给cpu减负，但是cpu比dma快，存在一种情况，cpu处理完了，dma还没搬运完，cpu就继续更新下一个数据包，导致数据包错位，解决办法：1.加延时 2.用双缓存区dma 3.用环形队列（软件层）
```

## 五、CAN
