#include "INF_DS3553.H"

void Inf_DS3553_Init(void)
{
    /* 1. ��ʼ��I2C */
    MX_I2C1_Init();
}

/**
 * @description: ��DS3553��ȡָ���Ĵ�����ֵ
 * @param {uint8_t} reg Ҫ��ȡ�ļĴ����ĵ�ַ
 * @return {*} ��ȡ��ֵ
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
 * @description: ������: ��ȡоƬ��id
 * @return {*} оƬ��id
 */
uint8_t Inf_DS3553_ReadId(void)
{
    return Inf_DS3553_ReadReg(DS3553_CHIP_ID);
}

/**
 * @description: ��ȡ�û����üĴ�����ֵ
 * @return {*} �û����õ�ֵ
 */
uint8_t Inf_DS3553_ReadUserSet(void)
{
    return Inf_DS3553_ReadReg(DS3553_USER_SET);
}

/**
 * @description: ��ȡ����
 * @return {*} ��ȡ���Ĳ���
 */
uint32_t Inf_DS3553_ReadCNT(void)
{
    uint32_t result = 0;
    result |= Inf_DS3553_ReadReg(DS3553_CNT_L);
    result |= (Inf_DS3553_ReadReg(DS3553_CNT_M) << 8);
    result |= (Inf_DS3553_ReadReg(DS3553_CNT_H) << 16);
    return result;
}
