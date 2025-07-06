#include "w25n01gv.h"
#include "main.h"


int W25N_Reset()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = RESET;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

uint16_t W25N_Get_ID()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = JEDEC_ID;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    command.DummyCycles = 8;
    command.NbData = 3;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t data[3] = {0};

    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;
    
    HAL_QSPI_Receive(&hqspi, &data, 100);
    
    uint16_t id = (data[1] << 8) | data[2];

    return id;
}

int W25N_Write_Disable()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = WRITE_DISABLE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

int W25N_Write_Enable()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = WRITE_ENABLE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

int W25N_Write_Status_Reg(uint8_t reg, uint8_t set)
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = WRITE_STATUS_REG;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = reg;
    command.AddressSize= QSPI_ADDRESS_8_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.NbData = 1;
    command.DataMode = QSPI_DATA_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Transmit(&hqspi, &set, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}


uint8_t W25N_Read_Status_Reg(uint8_t reg)
{
    uint8_t data;
    QSPI_CommandTypeDef command = {0};
    command.Instruction = READ_STATUS_REG;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = reg;
    command.AddressSize= QSPI_ADDRESS_8_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.NbData = 1;
    command.DataMode = QSPI_DATA_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Receive(&hqspi, &data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return data;
}

int W25N_Init()
{
    W25N_Write_Status_Reg(PROT_REG, 0);
    return HAL_OK;
}

int W25N_Write_Buffer(uint16_t column_address, const uint8_t *data, uint32_t data_len)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = QUAD_PROGRAM_DATA_LOAD;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = column_address;
    command.AddressSize = QSPI_ADDRESS_16_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.NbData = data_len;
    command.DataMode = QSPI_DATA_4_LINES;

    if(HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -100;
    
    if(HAL_QSPI_Transmit(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -200;

    return HAL_OK;
}


int W25N_Read_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
{
    W25N_Write_Enable();

    QSPI_CommandTypeDef command = {0};
    command.Instruction = FAST_READ_QUAD_IO;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = column_address;
    command.AddressSize = QSPI_ADDRESS_16_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.DummyCycles = 8;
    command.NbData = data_len;
    command.DataMode = QSPI_DATA_4_LINES;

    if(HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -300;
    
    if(HAL_QSPI_Receive(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -400;
    
    return HAL_OK;
}

int W25N_Load_Page(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PAGE_DATA_READ;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    if(HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -500;

    return HAL_OK;
}

int W25N_Program_Execute(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PROGRAM_EXECUTE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    if(HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -600;

    if(W25N_Read_Status_Reg(STAT_REG) & 0x08)
        return -700;
    
    return HAL_OK;
}


int W25N_Block_Erase(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = BLOCK_ERASE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    if(HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return -800;

    if(W25N_Read_Status_Reg(STAT_REG) & 0x04)
        return -900;

    return HAL_OK;
}

int W25N_Read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint16_t page_address = block * 64 + off / 2048;
    uint16_t col_address = 0;
    int err;
    err = W25N_Load_Page(page_address);
    if(err != HAL_OK)
        return err;

    err = W25N_Read_Buffer(0, buffer, size);
    if(err != HAL_OK)
        return err;

    return 0;
}

int W25N_Write(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint16_t page_address = block * 64 + off / 2048;
    uint16_t col_address = 0;
    int err;
    err = W25N_Write_Buffer(col_address, buffer, size);
    if(err != HAL_OK)
        return err;

    err = W25N_Program_Execute(page_address);
    if(err != HAL_OK)
        return err;

    return LFS_ERR_OK;
}

int W25N_Erase(const struct lfs_config *c, lfs_block_t block)
{
    uint16_t page_address = block * 64;

    if(W25N_Block_Erase(page_address) != HAL_OK)
        return LFS_ERR_IO;

    return LFS_ERR_OK;
}

int W25N_Sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}




