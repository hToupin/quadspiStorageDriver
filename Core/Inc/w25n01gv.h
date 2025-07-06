#include "main.h"
#include "spiffs_config.h"
#include "lfs.h"
#include "lfs_util.h"


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
#define PAGE_SIZE 2048
#define BLOCK_SIZE 64 * PAGE_SIZE
#define BLOCK_COUNT 1024
#define MEM_SIZE 1024 * BLOCK_SIZE

// Status Registers
#define PROT_REG 0xA0U
#define CONFIG_REG 0xB0U
#define STAT_REG 0xC0U

extern QSPI_HandleTypeDef hqspi;

int W25N_Reset();
uint16_t W25N_Get_ID();
int W25N_Write_Disable();
int W25N_Write_Enable();
int W25_Write_Status_Reg(uint8_t reg, uint8_t set);
uint8_t W25_Read_Status_Reg(uint8_t reg);
int W25N_Init();
int W25N_Write_Buffer(uint16_t column_address, const uint8_t* data, uint32_t data_len);
int W25N_Read_Buffer(uint16_t column_address, uint8_t* data, uint32_t data_len);
int W25N_Program_Execute(uint16_t page_address);
int W25N_Load_Page(uint16_t page_address);
int W25N_Block_Erase(uint16_t page_address);

int W25N_Read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int W25N_Write(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int W25N_Erase(const struct lfs_config *c, lfs_block_t block);
int W25N_Sync(const struct lfs_config *c);






