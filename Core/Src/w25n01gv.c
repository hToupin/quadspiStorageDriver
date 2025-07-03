#include "w25n01gv.h"
#include "main.h"


HAL_StatusTypeDef W25N_Reset()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = RESET;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Init()
{
    W25N_Write_Status_Reg(PROT_REG, 0);
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

HAL_StatusTypeDef W25N_Write_Status_Reg(uint8_t reg, uint8_t set)
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

HAL_StatusTypeDef W25N_Write_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
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

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Transmit(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Read_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
HAL_StatusTypeDef W25N_Read_Buffer(uint16_t column_address, uint8_t *data, uint32_t data_len)
{
    if(data_len >= SECTOR_SIZE * 4)
        return HAL_ERROR;

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

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Receive(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    
    return HAL_OK;
}

HAL_StatusTypeDef W25N_Load_Page(uint16_t page_address)
HAL_StatusTypeDef W25N_Load_Page(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PAGE_DATA_READ;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Program_Execute(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = PROGRAM_EXECUTE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    
    return HAL_OK;
}


HAL_StatusTypeDef W25N_Block_Erase(uint16_t page_address)
{
    W25N_Write_Enable();
    QSPI_CommandTypeDef command = {0};
    command.Instruction = BLOCK_ERASE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = page_address;
    command.AddressSize = QSPI_ADDRESS_24_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}

// int hal_lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
// {
//     if(block > c->block_count || off + size > c->block_size)
//         return LFS_ERR_INVAL;
    
//     if(W25N_Read(block, off, buffer, size) != HAL_OK)
//         return LFS_ERR_IO;

//     return LFS_ERR_OK;
// }

// int hal_lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
// {
//     if(block > c->block_count || off + size > c->block_size)
//         return LFS_ERR_INVAL;
    
//     if(W25N_Read(block, off, buffer, size) != HAL_OK)
//         return LFS_ERR_IO;

//     return LFS_ERR_OK;
// }

// int hal_lfs_erase(const struct lfs_config *c, lfs_block_t block)
// {
//     W25N_Page_Erase(block);
//     return 0;
// }

// int hal_lfs_sync(const struct lfs_config *c)
// {
//     UNUSED(c);
//     return 0;
// }
