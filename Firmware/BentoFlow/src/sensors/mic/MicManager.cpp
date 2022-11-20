
#include "MicManager.h"

const String MicEvent::eventNames[MicEvent::TYPES_MAX]{"enveloppe"};
MicManager *MicManager::instance = NULL;

MicManager::MicManager() : Component("mic"),
                           isEnabled(false),
                           shouldStopRead(false),
                           enveloppe(0)
{
    instance = this;
}

void MicManager::init()
{
#if USE_MIC
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
   // M5.Lcd.println("mic test");

    i2sInit();
    xTaskCreate(mic_record_task, "mic_record_task", 2048, NULL, 1, NULL);
#endif
}

void MicManager::update()
{
#if USE_MIC

    // vTaskDelay(1000 / portTICK_RATE_MS); // otherwise the main task wastes half
    //  of the cpu cycles
#endif
}

void MicManager::shutdown()
{
    setEnabled(false);
    shouldStopRead = true;
}

void MicManager::i2sInit() // Init I2S.  初始化I2S
{
#if USE_MIC
   i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX |
                             I2S_MODE_PDM), // Set the I2S operating mode.
                                            // 设置I2S工作模式
        .sample_rate = 44100,               // Set the I2S sampling rate.  设置I2S采样率
        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_16BIT, // Fixed 12-bit stereo MSB.
                                       // 固定为12位立体声MSB
        .channel_format =
            I2S_CHANNEL_FMT_ALL_RIGHT, // Set the channel format.  设置频道格式
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S, // Set the format of the communication.
#else                                  // 设置通讯格式
        .communication_format = I2S_COMM_FORMAT_I2S,
#endif
        .intr_alloc_flags =
            ESP_INTR_FLAG_LEVEL1, // Set the interrupt flag.  设置中断的标志
        .dma_buf_count = 2,       // DMA buffer count.  DMA缓冲区计数
        .dma_buf_len = 128,       // DMA buffer length.  DMA缓冲区长度
    };

    i2s_pin_config_t pin_config;

#if (ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    pin_config.bck_io_num = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num = PIN_DATA;

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
#endif
}

void MicManager::showSignal()
{
#if USE_MIC

    int eMin = INT32_MAX;
    int eMax = INT32_MIN;
    int mapY;
    int offsetX = (M5.Lcd.width() - 160) / 2;
    for (int n = 0; n < 160; n++)
    {
        int y = adcBuffer[n] * GAIN_FACTOR;
        mapY = map(y, INT16_MIN, INT16_MAX, 10, M5.Lcd.height() - 20);

        eMin = min(eMin, y);
        eMax = max(eMax, y);

        M5.Lcd.drawPixel(offsetX + n, oldy[n], BLACK);
        M5.Lcd.drawPixel(offsetX + n, mapY, WHITE);
        oldy[n] = mapY;
    }

    int diff = eMax - eMin;
    float env = diff * 1.0 / (INT16_MAX - INT16_MIN);
    env /= 3;
    envWindow[envIndex] = env;
    envIndex = (envIndex + 1) % RMS_WINDOW;

    float e = 0;
    for (int i = 0; i < RMS_WINDOW; i++) e = max(e, envWindow[i]);
    enveloppe = e;
#endif

    // NDBG("Enveloppe " + String(enveloppe)+" ( " + String(eMin)+", "+String(eMax)+")");
}

void MicManager::mic_record_task(void *arg)
{
#if USE_MIC
    size_t bytesread;

    while (!instance->shouldStopRead)
    {
        if (!instance->isEnabled)
        {
            delay(100);
            continue;
        }

        i2s_read(I2S_NUM_0, (char *)instance->BUFFER, READ_LEN, &bytesread,
                 (100 / portTICK_RATE_MS));
        instance->adcBuffer = (int16_t *)instance->BUFFER;
        instance->showSignal();
        delay(5);
    }
#endif
}

void MicManager::setEnabled(bool value)
{
    if (isEnabled == value)
        return;

    isEnabled = value;
}

bool MicManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "enabled", numData, 1))
    {
        setEnabled(data[0].intValue());
        return true;
    }
    return false;
}