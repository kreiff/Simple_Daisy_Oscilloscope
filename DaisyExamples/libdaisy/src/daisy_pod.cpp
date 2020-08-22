#include "daisy_pod.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE DSY_AUDIO_SAMPLE_RATE
#endif


// # Rev3 and Rev4 with newest pinout.
// Compatible with Seed Rev3 and Rev4
#define SW_1_PIN 28
#define SW_2_PIN 27

#define ENC_A_PIN 26
#define ENC_B_PIN 25
#define ENC_CLICK_PIN 13

#define LED_1_R_PIN 20
#define LED_1_G_PIN 19
#define LED_1_B_PIN 18
#define LED_2_R_PIN 17
#define LED_2_G_PIN 24
#define LED_2_B_PIN 23

#define KNOB_1_PIN 21
#define KNOB_2_PIN 15
#define KNOB_3_PIN 16
#define KNOB_4_PIN 17
#define KNOB_5_PIN 18
#define KNOB_6_PIN 19

#define PIN_OLED_DC 9
#define PIN_OLED_RESET 30

#define PIN_SAI_SCK_A 28
#define PIN_SAI2_FS_A 27
#define PIN_SAI2_SD_A 26
#define PIN_SAI2_SD_B 25
#define PIN_SAI2_MCLK 24

#define PIN_AK4556_RESET 29

/*
// Leaving in place until older hardware is totally deprecated.
#ifndef SEED_REV2

// Rev2 Pinout
// Compatible with Seed rev1 and rev2

#define SW_1_PIN 28
#define SW_2_PIN 29

#define ENC_A_PIN 27
#define ENC_B_PIN 26
#define ENC_CLICK_PIN 14

#define LED_1_R_PIN 21
#define LED_1_G_PIN 20
#define LED_1_B_PIN 19
#define LED_2_R_PIN 0
#define LED_2_G_PIN 25
#define LED_2_B_PIN 24

#else

// Rev1 Pinout
// Compatible with Seed rev1 and rev2

#define SW_1_PIN 29
#define SW_2_PIN 28

#define ENC_A_PIN 27
#define ENC_B_PIN 26
#define ENC_CLICK_PIN 1

#define LED_1_R_PIN 21
#define LED_1_G_PIN 20
#define LED_1_B_PIN 19
#define LED_2_R_PIN 0
#define LED_2_G_PIN 25
#define LED_2_B_PIN 24

#endif
*/


using namespace daisy;

void DaisyPod::Init()
{
    // Set Some numbers up for accessors.
    sample_rate_   = SAMPLE_RATE; // TODO add configurable SR to libdaisy audio.
    block_size_    = 48;
    callback_rate_ = (sample_rate_ / static_cast<float>(block_size_));
    // Initialize the hardware.
    seed.Configure();
    seed.Init();
    dsy_tim_start();
    InitButtons();
    InitEncoder();
    InitLeds();
    InitDisplay();
    InitKnobs();
    SetAudioBlockSize(block_size_);
    dsy_gpio_write(&ak4556_reset_pin_, 0);
    DelayMs(10);
    dsy_gpio_write(&ak4556_reset_pin_, 1);
    // Set Screen update vars
    screen_update_period_ = 17; // roughly 60Hz
    screen_update_last_   = dsy_system_getnow();
}

void DaisyPod::DelayMs(size_t del)
{
    dsy_tim_delay_ms(del);
}

void DaisyPod::StartAudio(dsy_audio_callback cb)
{
    seed.StartAudio(cb);
}

void DaisyPod::StartAudio(dsy_audio_mc_callback cb)
{
    seed.StartAudio(cb);
}

void DaisyPod::ChangeAudioCallback(dsy_audio_callback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyPod::ChangeAudioCallback(dsy_audio_mc_callback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyPod::SetAudioBlockSize(size_t size)
{
    block_size_    = size;
    callback_rate_ = (sample_rate_ / static_cast<float>(block_size_));
    dsy_audio_set_blocksize(DSY_AUDIO_INTERNAL, block_size_);
}

float DaisyPod::AudioSampleRate()
{
    return sample_rate_;
}

size_t DaisyPod::AudioBlockSize()
{
    return block_size_;
}

float DaisyPod::AudioCallbackRate()
{
    return callback_rate_;
}

void DaisyPod::StartAdc()
{
    seed.adc.Start();
}

void DaisyPod::UpdateAnalogControls()
{
    knob1.Process();
    knob2.Process();
    knob3.Process();
    knob4.Process();
    knob5.Process();
    knob6.Process();
}

float DaisyPod::GetKnobValue(Knob k)
{
    size_t idx;
    idx = k < KNOB_LAST ? k : KNOB_1;
    return knobs[idx]->Value();
}

void DaisyPod::DebounceControls()
{
    encoder.Debounce();
    button1.Debounce();
    button2.Debounce();
}

void DaisyPod::ClearLeds()
{
    // Using Color
    Color c;
    c.Init(Color::PresetColor::OFF);
    led1.SetColor(c);
    led2.SetColor(c);
    // Without
    // led1.Set(0.0f, 0.0f, 0.0f);
    // led2.Set(0.0f, 0.0f, 0.0f);
}

void DaisyPod::UpdateLeds()
{
    led1.Update();
    led2.Update();
}

void DaisyPod::InitButtons()
{
    // button1
    button1.Init(seed.GetPin(SW_1_PIN), callback_rate_);
    // button2
    button2.Init(seed.GetPin(SW_2_PIN), callback_rate_);

    buttons[BUTTON_1] = &button1;
    buttons[BUTTON_2] = &button2;
}

void DaisyPod::InitEncoder()
{
    dsy_gpio_pin a, b, click;
    a     = seed.GetPin(ENC_A_PIN);
    b     = seed.GetPin(ENC_B_PIN);
    click = seed.GetPin(ENC_CLICK_PIN);
    encoder.Init(a, b, click, callback_rate_);
}

void DaisyPod::InitLeds()
{
    // LEDs are just going to be on/off for now.
    // TODO: Add PWM support
    dsy_gpio_pin rpin, gpin, bpin;

    rpin = seed.GetPin(LED_1_R_PIN);
    gpin = seed.GetPin(LED_1_G_PIN);
    bpin = seed.GetPin(LED_1_B_PIN);
    led1.Init(rpin, gpin, bpin, true);

    rpin = seed.GetPin(LED_2_R_PIN);
    gpin = seed.GetPin(LED_2_G_PIN);
    bpin = seed.GetPin(LED_2_B_PIN);
    led2.Init(rpin, gpin, bpin, true);

    ClearLeds();
    UpdateLeds();
}
void DaisyPod::InitKnobs()
{
    // Configure the ADC channels using the desired pin
    AdcChannelConfig knob_init[KNOB_LAST];
    knob_init[KNOB_1].InitSingle(seed.GetPin(KNOB_1_PIN));
    knob_init[KNOB_2].InitSingle(seed.GetPin(KNOB_2_PIN));
    knob_init[KNOB_3].InitSingle(seed.GetPin(KNOB_3_PIN));
    knob_init[KNOB_4].InitSingle(seed.GetPin(KNOB_4_PIN));
    knob_init[KNOB_5].InitSingle(seed.GetPin(KNOB_5_PIN));
    knob_init[KNOB_6].InitSingle(seed.GetPin(KNOB_6_PIN));
    // Initialize with the knob init struct w/ 6 members
    // Set Oversampling to 32x
    seed.adc.Init(knob_init, KNOB_LAST);
    // Make an array of pointers to the knobs.
    knobs[KNOB_1] = &knob1;
    knobs[KNOB_2] = &knob2;
    knobs[KNOB_3] = &knob3;
    knobs[KNOB_4] = &knob4;
    knobs[KNOB_5] = &knob5;
    knobs[KNOB_6] = &knob6;
    for(int i = 0; i < KNOB_LAST; i++)
    {
        knobs[i]->Init(seed.adc.GetPtr(i), callback_rate_);
    }
}

void DaisyPod::InitDisplay()
{
    dsy_gpio_pin pincfg[OledDisplay::NUM_PINS];
    pincfg[OledDisplay::DATA_COMMAND] = seed.GetPin(PIN_OLED_DC);
    pincfg[OledDisplay::RESET]        = seed.GetPin(PIN_OLED_RESET);
    display.Init(pincfg);
}
