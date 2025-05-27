#include "w25n01gv.h"

HAL_StatusTypeDef W25N_Reset()
{
    QSPI_CommandTypeDef command = {0};
    command.Instruction = RESET;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
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
    
    if(HAL_QSPI_Command(&hqspi, &command, 10) != HAL_OK)
        return HAL_ERROR;

    uint8_t reg_stat = 0;
    reg_stat = W25_Read_Status_Reg(STAT_REG);

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
    command.DummyCycles = 6;
    command.NbData = 1;
    command.DataMode = QSPI_DATA_1_LINE;

    HAL_QSPI_Command(&hqspi, &command, 50);
    HAL_QSPI_Receive(&hqspi, &data, 100);

    return data;
}

HAL_StatusTypeDef W25N_Write(uint16_t page_address, uint16_t column_address, uint8_t *data, uint32_t data_len)
{
    if(data_len >= SECTOR_SIZE * 4 || page_address >= MAX_PAGE_ADDRESS || column_address >= MAX_COL_ADDRESS)
        return HAL_ERROR;

    QSPI_CommandTypeDef command = {0};
    command.Instruction = QUAD_PROGRAM_DATA_LOAD;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = column_address;
    command.AddressSize= QSPI_ADDRESS_16_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.NbData = data_len;
    command.DataMode = QSPI_DATA_4_LINES;

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, data, 50);

    command.Instruction = PROGRAM_EXECUTE;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = 0;
    command.AddressSize = 0;
    command.AddressMode = QSPI_ADDRESS_NONE;
    command.DummyCycles = 8;
    command.NbData = 2;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t address[2] = {0, 0};

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, address, 50);

    return HAL_OK;
}

HAL_StatusTypeDef W25N_Read(uint16_t page_address, uint16_t column_address, uint8_t *data, uint32_t data_len)
{
    if(data_len >= SECTOR_SIZE * 4 || page_address >= MAX_PAGE_ADDRESS || column_address >= MAX_COL_ADDRESS)
        return HAL_ERROR;

    QSPI_CommandTypeDef command = {0};
    command.Instruction = PAGE_DATA_READ;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.DummyCycles = 6;
    command.NbData = 2;
    command.DataMode = QSPI_DATA_1_LINE;

    uint8_t address[2] = {0, 0};

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Transmit(&hqspi, address, 50);

    command.Instruction = FAST_READ_QUAD_IO;
    command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    command.Address = column_address;
    command.AddressSize = QSPI_ADDRESS_16_BITS;
    command.AddressMode = QSPI_ADDRESS_1_LINE;
    command.DummyCycles = 8;
    command.NbData = data_len;
    command.DataMode = QSPI_DATA_4_LINES;

    HAL_QSPI_Command(&hqspi, &command, 10);
    HAL_QSPI_Receive(&hqspi, data, 50);
    
    return HAL_OK;
}


HAL_StatusTypeDef W25N_Block_Erase(uint16_t page_address, uint16_t column_address)
{
    return HAL_OK;
}
