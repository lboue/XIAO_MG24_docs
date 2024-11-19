#include "em_device.h"
#include "em_chip.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_letimer.h"
#include "em_prs.h"

#define CLK_SRC_ADC_FREQ        20000000
#define CLK_ADC_FREQ            10000000
#define IADC_INPUT_0_PORT_PIN   iadcPosInputPortCPin9
#define IADC_INPUT_0_BUS        CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC   GPIO_CDBUSALLOC_CDODD1_ADC0
#define ADC_FREQ                16000//16000/8500
#define IADC_LDMA_CH            0
#define PRS_CHANNEL             0
#define NUM_SAMPLES             200
#define LETIMER_FREQ            9000

const int duration = 3;

int voice_level = 0;

LDMA_Descriptor_t descriptor;
uint32_t singleBuffer[NUM_SAMPLES];

void initPRS(void) {
    CMU_ClockEnable(cmuClock_PRS, true);
    PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_LETIMER0, PRS_LETIMER0_CH0);
    PRS_ConnectConsumer(PRS_CHANNEL, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
}

void initIADC(void) {
    IADC_Init_t init = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
    IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
    IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;

    CMU_ClockEnable(cmuClock_IADC0, true);
    CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);
    init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
    init.warmup = iadcWarmupKeepInStandby;
    init.iadcClkSuspend1 = true;
    initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
    initAllConfigs.configs[0].vRef = 1210;
    initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
    initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;
    initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0, CLK_ADC_FREQ, 0, iadcCfgModeNormal, init.srcClkPrescale);
    initSingle.triggerSelect = iadcTriggerSelPrs0PosEdge;
    initSingle.dataValidLevel = iadcFifoCfgDvl2;
    initSingle.fifoDmaWakeup = true;
    initSingle.start = true;
    singleInput.posInput = IADC_INPUT_0_PORT_PIN;
    singleInput.negInput = iadcNegInputGnd;
    IADC_init(IADC0, &init, &initAllConfigs);
    IADC_initSingle(IADC0, &initSingle, &singleInput);
    GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
}

void initLETIMER(void) {
    CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
    LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

    CMU_LFXOInit(&lfxoInit);
    CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LETIMER0, true);
    uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / LETIMER_FREQ;
    letimerInit.comp0Top = true;
    letimerInit.topValue = topValue;
    letimerInit.ufoa0 = letimerUFOAPulse;
    letimerInit.repMode = letimerRepeatFree;
    LETIMER_Init(LETIMER0, &letimerInit);
}

void initLDMA(uint32_t *buffer, uint32_t size) {
    LDMA_Init_t init = LDMA_INIT_DEFAULT;
    LDMA_TransferCfg_t transferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SINGLE);
    descriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 0);
    LDMA_Init(&init);
    LDMA_StartTransfer(IADC_LDMA_CH, &transferCfg, &descriptor);
}

#include <stdio.h>

#define SIZE 200
#define N 5

#include <stdint.h> 



float findMaxAverage(uint32_t arr[], int size, int n) {
    uint32_t maxElements[N];

    for (int i = 0; i < n; i++) {
        maxElements[i] = arr[i]; 
    }

    for (int i = n; i < size; i++) {
        for (int j = 0; j < n; j++) {
            if (arr[i] > maxElements[j]) {
                maxElements[j] = arr[i];
                break; 
            }
        }
    }

    uint32_t sum = 0; 
    for (int i = 0; i < n; i++) {
        sum += maxElements[i];

    }
    return (float)sum / n; 
}

float mapVoiceLevel(int voice_level) {

    int original_min = 960;
    int original_max = 1500;
    int target_min = 0;
    int target_max = 20;

    if (voice_level < original_min) {
        voice_level = original_min;
    } else if (voice_level > original_max) {
        voice_level = original_max;
    }

    float mapped_value = (float)(voice_level - original_min) / (original_max - original_min) * (target_max - target_min) + target_min;

    return mapped_value;
}

int datatimes = 0;
int pwmdata = 0;
int pwmdata2 = 0;

void LDMA_IRQHandler(void) {
    
    voice_level = findMaxAverage(singleBuffer, SIZE, N);
    Serial.println(voice_level);
    voice_level = mapVoiceLevel(voice_level);
    LDMA_IntClear(1 << IADC_LDMA_CH);
}

void setup(void) {
    Serial.begin(115200);
    CHIP_Init();
    initPRS();
    initIADC();
    initLDMA(singleBuffer, NUM_SAMPLES);
    initLETIMER();
    pinMode(LED_BUILTIN, OUTPUT);
}

int high_time = 0;
int pwm_t = 20;
int low_time = 0;

void loop() {
    int j = 0;
    while (1)
    {
        low_time = voice_level;
        high_time = pwm_t - low_time;
        j = 0;
        while (j < 10)
        {
            digitalWrite(LED_BUILTIN, HIGH);  
            delay(high_time);                      
            digitalWrite(LED_BUILTIN, LOW);   
            delay(low_time);  /* code */
            j++;
        }
       
    }
 
}
