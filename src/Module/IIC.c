#include <stm32f10x.h>

#define IIC_SCL_Pin GPIO_Pin_8
#define IIC_SDA_Pin GPIO_Pin_9
#define IIC_Port GPIOB

/// @brief 写入IIC时钟线SCL
/// @param bitValue 
void IIC_W_SCL(uint8_t bitValue)
{
    GPIO_WriteBit(IIC_Port, IIC_SCL_Pin, (BitAction)bitValue);
}

/// @brief 写入IIC数据线SDA
/// @param bitValue 
void IIC_W_SDA(uint8_t bitValue)
{
    GPIO_WriteBit(IIC_Port, IIC_SDA_Pin, (BitAction)bitValue);
}

// 读取IIC数据线SDA
uint8_t IIC_R_SDA(void)
{
    uint8_t bitValue = GPIO_ReadInputDataBit(IIC_Port, IIC_SDA_Pin);
    return bitValue;
}

// 初始化
void IIC_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_Pin | IIC_SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, IIC_SCL_Pin | IIC_SDA_Pin);  // SCL和SDA线拉高，释放总线
}

// 通信开始
void IIC_Start(void)
{
    // 先全都释放，为避免scl为高时候sda被读取，所以先释放sda
    IIC_W_SDA(1);
    IIC_W_SCL(1);
    
    // 产生起始信号
    IIC_W_SDA(0);  // SDA线拉低
    IIC_W_SCL(0);  // SCL线拉低
}

// 通信结束
void IIC_Stop(void)
{
    // 产生停止信号
    IIC_W_SDA(0);  
    IIC_W_SCL(1);  
    IIC_W_SDA(1);  
}

// 发送一个字节
void IIC_SendByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        IIC_W_SDA(byte & (0x80 >> i));  // 按位发送数据
        IIC_W_SCL(1);                     // SCL线拉高
        IIC_W_SCL(0);                     // SCL线拉低
    }  
}

// 读取一个字节
uint8_t IIC_ReceiveByte(void)
{
    uint8_t byte = 0x00;
    IIC_W_SDA(1); // 主机释放SDA线，权限交给从机
    for (uint8_t i = 0; i < 8; i++) 
    {
        IIC_W_SCL(1); // 主机拉高SCL线读取数据
        if (IIC_R_SDA() == 1) 
        {
            byte |= 0x80 >> i;  // 按位读取数据
        }
        IIC_W_SCL(0); // 主机拉低SCL线，从机放数据
    }
    return byte;
}

// 主机发送应答
void IIC_SendAck(uint8_t AckBit)
{
    IIC_W_SDA(AckBit); 
    IIC_W_SCL(1);  // 主机拉高SCL线，从机读取
    IIC_W_SCL(0);  // 主机拉低SCL线，从机放数据
}

// 主机接收应答
uint8_t IIC_ReceiveAck(void)
{
    IIC_W_SDA(1); // 主机释放SDA线，权限交给主机
    IIC_W_SCL(1); // 主机拉高SCL线读取数据
    uint8_t ackBit = IIC_R_SDA(); // 读取应答位
    IIC_W_SCL(0); // 主机拉低SCL线，从机放数据
    return ackBit;
}
