#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod pod;
Oscillator oscViz, oscAud;
Parameter p_freq;
Parameter p_amp;

static float freq;
static float amp;

const int SCREEN_WIDTH = 128;

float sigViz, sigAud, sigPost;
float riseTest[2];

uint32_t getTick;
int getTickInt;
uint32_t saveTick;

void InitSynth(float sample_rate);

void Controls();

void UpdateOled() {
	getTick = dsy_system_getnow();
    getTickInt = getTick;
    if(getTickInt == 1000)
    {
    	saveTick = getTick;
    }
    else if (getTickInt - saveTick > 120 /*refreshRate*/)
	{
		
		for (int m = 0; m < 2; m++)
		{
			riseTest[m] = oscViz.Process();
		}
		if(riseTest[0] > 0 && riseTest[1] < 0)
		{
		pod.display.Fill(false);
		for (int k = 0; k < 128; k++) {
    	sigViz = oscViz.Process();
		sigPost = (sigViz + 1) * 30;
    	pod.display.DrawPixel(k, sigPost, true);
    	}
		saveTick = getTick;
		}
	}

    for (int i = 0; i < 128; i++) {
    if (i==0)
    {
      for (int j = 0; j < 64; j++) {
      pod.display.DrawPixel(i, j, true);
    }
    }
    pod.display.DrawPixel(i, 63, true);
    }
    pod.display.Update(); 

  }

static void AudioCallback(float *in, float *out, size_t size)
{

    for (size_t i = 0; i < size; i += 2)
    {

    	sigAud = oscAud.Process();

    	// left out
        out[i] = sigAud;

        // right out
        out[i+1] = sigAud;
    }
}

int main(void) {

  float sample_rate;
  
  pod.Init();
  sample_rate = pod.AudioSampleRate();
  InitSynth(sample_rate);

  oscViz.SetWaveform(oscViz.WAVE_SIN);
  oscViz.SetFreq(440);
  oscViz.SetAmp(1);
  oscAud.SetWaveform(oscAud.WAVE_SIN);
  oscAud.SetFreq(440);
  oscAud.SetAmp(1);

  pod.StartAdc();
  pod.StartAudio(AudioCallback);

  while(1) {
    UpdateOled();
    Controls();
	freq = p_freq.Process();
    amp = p_amp.Process();
    oscViz.SetFreq(freq);
    oscViz.SetAmp(amp);
    oscAud.SetFreq(freq);
    oscAud.SetAmp(amp);
  }
}

void UpdateKnobs(float &k1, float &k2)
{
    k1 = pod.knob1.Process();
    k2 = pod.knob2.Process();
}

void Controls()
{
    float k1, k2;
    pod.UpdateAnalogControls();
    UpdateKnobs(k1, k2);
}

void InitSynth(float sample_rate)
{
  p_freq.Init(pod.knob1, 0, 2000, Parameter::LINEAR);
  p_amp.Init(pod.knob2, 0.00f, 1.00f, Parameter::LINEAR);

  oscViz.Init(sample_rate);
  oscAud.Init(sample_rate);
}