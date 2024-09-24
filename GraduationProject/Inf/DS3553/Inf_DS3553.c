#include "INF_DS3553.H"

void Inf_DS3553_Init(void)
{
    /* 1. 初始化I2C */
    MX_I2C1_Init();
}

/**
 * @description: 从DS3553读取指定寄存器的值
 * @param {uint8_t} reg 要读取的寄存器的地址
 * @return {*} 读取到值
 */
static uint8_t Inf_DS3553_ReadReg(uint8_t reg)
{
    uint8_t v = 0;
    DS3553_CS_L;
    Common_Delay_Ms(4);

    HAL_I2C_Mem_Read(&hi2c1, DS3553_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT, &v, 1, 2000);

    DS3553_CS_H;
    Common_Delay_Ms(11);

    return v;
}

/**
 * @description: 测试用: 读取芯片的id
 * @return {*} 芯片的id
 */
uint8_t Inf_DS3553_ReadId(void)
{
    return Inf_DS3553_ReadReg(DS3553_CHIP_ID);
}

/**
 * @description: 读取用户配置寄存器的值
 * @return {*} 用户配置的值
 */
uint8_t Inf_DS3553_ReadUserSet(void)
{
    return Inf_DS3553_ReadReg(DS3553_USER_SET);
}

/**
 * @description: 读取步数
 * @return {*} 读取到的步数
 */
uint32_t Inf_DS3553_ReadCNT(void)
{
    uint32_t result = 0;
    result |= Inf_DS3553_ReadReg(DS3553_CNT_L);
    result |= (Inf_DS3553_ReadReg(DS3553_CNT_M) << 8);
    result |= (Inf_DS3553_ReadReg(DS3553_CNT_H) << 16);
    return result;
}
