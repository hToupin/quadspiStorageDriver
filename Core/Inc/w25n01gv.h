#include "main.h"

// Instruction Set
#define RESET 0xFFU
#define JEDEC_ID 0x9FU
#define READ_STATUS_REG 0x05U
#define WRITE_STATUS_REG 0x1FU
#define WRITE_ENABLE 0x06U
#define WRITE_DISABLE 0x04U
#define BLOCK_ERASE 0xD8U
#define PROGRAM_DATA_LOAD 0x02U
#define RANDOM_PROGRAM_DATA_LOAD 0x84U
#define QUAD_PROGRAM_DATA_LOAD 0x32U
#define PROGRAM_EXECUTE 0x10U
#define PAGE_DATA_READ 0x13U
#define FAST_READ_QUAD_IO 0x6BU

// Chip Properties
#define MAX_COL_ADDRESS 2048
#define MAX_PAGE_ADDRESS 65536
#define SECTOR_SIZE 512

// Status Registers
#define PROT_REG 0xA0U
#define CONFIG_REG 0xB0U
#define STAT_REG 0xC0U

extern QSPI_HandleTypeDef hqspi;

HAL_StatusTypeDef W25N_Reset();
uint16_t W25N_Get_ID();
HAL_StatusTypeDef W25N_Write_Disable();
HAL_StatusTypeDef W25N_Write_Enable();
HAL_StatusTypeDef W25_Write_Status_Reg(uint8_t reg, uint8_t set);
uint8_t W25_Read_Status_Reg(uint8_t reg);
HAL_StatusTypeDef W25N_Write(uint16_t page_address, uint16_t column_address, uint8_t* data, uint32_t data_len);
HAL_StatusTypeDef W25N_Read(uint16_t page_address, uint16_t column_address, uint8_t* data, uint32_t data_len);
HAL_StatusTypeDef W25N_Block_Erase(uint16_t page_address, uint16_t column_address);


