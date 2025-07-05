#include "w25n01gv.h"
#include "main.h"
#include "spiffs_config.h"



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

HAL_StatusTypeDef W25N_Init()
{
    W25N_Write_Status_Reg(PROT_REG, 0);
    return HAL_OK;
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

    HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Transmit(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return HAL_OK;
}


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

int W25N_Read(uint32_t addr, uint32_t size, uint8_t *dst)
{
    uint16_t page_address = addr / 2048;
    uint16_t col_address = addr - page_address * 2048;

    W25N_Load_Page(page_address);
    W25N_Read_Buffer(col_address, dst, size);

    return 0;
}

int W25N_Write(uint32_t addr, uint32_t size, uint8_t *src)
{
    uint16_t page_address = addr / 2048;
    uint16_t col_address = addr - page_address * 2048;

    W25N_Write_Buffer(col_address, src, size);
    W25N_Program_Execute(page_address);

    return 0;
}

int W25N_Erase(uint32_t addr, uint32_t size)
{
    uint16_t page_address = addr / 2048;
    W25N_Block_Erase(page_address);
    return 0;
}




