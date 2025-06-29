#include "w25n01gv.h"
#include "main.h"
#include "../../littlefs/lfs.h"
#include "../../littlefs/lfs_util.h"


HAL_StatusTypeDef W25N_Reset()
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

HAL_StatusTypeDef W25N_Write_Disable()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = WRITE_DISABLE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Write_Enable()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = WRITE_ENABLE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef W25_Write_Status_Reg(uint8_t reg, uint8_t set)
{
    return HAL_OK;
}

uint8_t W25_Read_Status_Reg(uint8_t reg)
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

    HAL_QSPI_Command(&hqspi, &command, 50);
    HAL_QSPI_Receive(&hqspi, &data, 100);

    return data;
}

HAL_StatusTypeDef W25N_Write_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
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

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, data, 50);

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Read_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = FAST_READ_QUAD_IO;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = column_address;
    command.AddressSize = QSPI_ADDRESS_16_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.DummyCycles = 6;
    command.NbData = data_len;
    command.DataMode = QSPI_DATA_4_LINES;

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Receive(&hqspi, data, 50);
    
    return HAL_OK;
}

HAL_StatusTypeDef W25N_Load_Page(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PAGE_DATA_READ;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.DummyCycles = 0;
    command.NbData = 3;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t address[3] = {0x00 ,page_address >> 8, page_address & 0xff};

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, address, 50);
    return HAL_OK;
}


HAL_StatusTypeDef W25N_Program_Execute(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PROGRAM_EXECUTE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = 0;
    command.AddressSize = 0;
    command.AddressMode = QSPI_ADDRESS_NONE;
    command.DummyCycles = 0;
    command.NbData = 3;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t address[3] = {0x00 ,page_address >> 8, page_address & 0xff};

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, address, 50);
    return HAL_OK;
}


HAL_StatusTypeDef W25N_Block_Erase(uint16_t addr)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = BLOCK_ERASE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = 0;
    command.AddressSize = QSPI_ADDRESS_NONE;
    command.DummyCycles = 8;
    command.NbData = 2;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t address[2] = {addr >> 8, addr & 0xff};

    HAL_QSPI_Command(&hqspi, &command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Transmit(&hqspi, address, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}

// int spi_read(int addr, int size, char *buffer)
// {
//     if(block > c->block_count || off + size > c->block_size)
//         return LFS_ERR_INVAL;
    
//     if(W25N_Read(block, off, buffer, size) != HAL_OK)
//         return LFS_ERR_IO;

//     return LFS_ERR_OK;
// }

// int spi_read(int addr, int size, char *buffer);
// {
//     if(block > c->block_count || off + size > c->block_size)
//         return LFS_ERR_INVAL;
    
//     if(W25N_Read(block, off, buffer, size) != HAL_OK)
//         return LFS_ERR_IO;

//     return LFS_ERR_OK;
// }

// int spi_erase(int addr, int size)
// {
//     W25N_Block_Erase(addr);
//     return 0;
// }


