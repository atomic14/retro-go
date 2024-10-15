// REF: https://wiki.odroid.com/odroid_go/odroid_go

// Target definition
#define RG_TARGET_NAME             "esp32s3-zx-rainbow"

// Storage
#define RG_STORAGE_ROOT             "/sd"
#define RG_STORAGE_SDSPI_HOST       SPI3_HOST
#define RG_STORAGE_SDSPI_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_SDMMC_HOST       SDMMC_HOST_SLOT_1
// #define RG_STORAGE_SDMMC_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_FLASH_PARTITION  "vfs"

// Audio
#define RG_AUDIO_USE_INT_DAC        0   // 0 = Disable, 1 = GPIO25, 2 = GPIO26, 3 = Both
#define RG_AUDIO_USE_EXT_DAC        0   // 0 = Disable, 1 = Enable

#define RG_AUDIO_USE_BUZZER_PIN 3

// Video
#define RG_SCREEN_DRIVER            0   // 0 = ILI9341
#define RG_SCREEN_HOST              SPI2_HOST
#define RG_SCREEN_SPEED             SPI_MASTER_FREQ_40M // SPI_MASTER_FREQ_80M
#define RG_SCREEN_BACKLIGHT         1
#define RG_SCREEN_WIDTH             320
#define RG_SCREEN_HEIGHT            240
#define RG_SCREEN_ROTATE            0
#define RG_SCREEN_MARGIN_TOP        0
#define RG_SCREEN_MARGIN_BOTTOM     0
#define RG_SCREEN_MARGIN_LEFT       0
#define RG_SCREEN_MARGIN_RIGHT      0
#define RG_SCREEN_INIT() \
    ILI9341_CMD(0x11); \
    ILI9341_CMD(0x3a, 0x55); \
    ILI9341_CMD(0x36, 0x00); \
    ILI9341_CMD(0x2a, 0x00, 0x00, 0x00, 0xf0); \
    ILI9341_CMD(0x2b, 0x00, 0x00, 0x00, 0xf0); \
    ILI9341_CMD(0x21); \
    ILI9341_CMD(0x13); \
    ILI9341_CMD(0x29); \
    ILI9341_CMD(0x36, 0x40 | 0x20 | 0x00); 


// Input
/*
Refer to rg_input.h to see all available RG_KEY_* and RG_GAMEPAD_*_MAP types
#define RG_GAMEPAD_ADC_MAP {\
    {RG_KEY_UP,    ADC_UNIT_1, ADC_CHANNEL_5, ADC_ATTEN_DB_11, 3072, 4096},\
    {RG_KEY_RIGHT, ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_11, 1024, 3071},\
    {RG_KEY_DOWN,  ADC_UNIT_1, ADC_CHANNEL_5, ADC_ATTEN_DB_11, 1024, 3071},\
    {RG_KEY_LEFT,  ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_11, 3072, 4096},\
}
#define RG_GAMEPAD_GPIO_MAP {\
    {RG_KEY_SELECT, GPIO_NUM_16, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_START,  GPIO_NUM_17, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_MENU,   GPIO_NUM_18, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_OPTION, GPIO_NUM_8,  GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_A,      GPIO_NUM_15, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_B,      GPIO_NUM_5,  GPIO_PULLUP_ONLY, 0},\
}*/

// Battery
#define RG_BATTERY_DRIVER           0
#define RG_BATTERY_ADC_UNIT         ADC_UNIT_1
#define RG_BATTERY_ADC_CHANNEL      ADC_CHANNEL_3
#define RG_BATTERY_CALC_PERCENT(raw) (((raw) * 2.f - 3500.f) / (4200.f - 3500.f) * 100.f)
#define RG_BATTERY_CALC_VOLTAGE(raw) ((raw) * 2.f * 0.001f)

// Status LED
#define RG_GPIO_LED                 GPIO_NUM_1

// SPI Display (back up working)
#define RG_GPIO_LCD_MISO            GPIO_NUM_NC
#define RG_GPIO_LCD_MOSI            GPIO_NUM_15
#define RG_GPIO_LCD_CLK             GPIO_NUM_18
#define RG_GPIO_LCD_CS              GPIO_NUM_16
#define RG_GPIO_LCD_DC              GPIO_NUM_17
#define RG_GPIO_LCD_BCKL            GPIO_NUM_NC
#define RG_GPIO_LCD_RST             GPIO_NUM_8

#define RG_GPIO_SDSPI_MISO          GPIO_NUM_39
#define RG_GPIO_SDSPI_MOSI          GPIO_NUM_41
#define RG_GPIO_SDSPI_CLK           GPIO_NUM_40
#define RG_GPIO_SDSPI_CS            GPIO_NUM_42

// External I2S DAC
#define RG_GPIO_SND_I2S_BCK         41
#define RG_GPIO_SND_I2S_WS          42
#define RG_GPIO_SND_I2S_DATA        40
// #define RG_GPIO_SND_AMP_ENABLE      18
