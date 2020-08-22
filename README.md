# Simple_Daisy_Oscilloscope
Very rudimentary "oscilloscope" example for the Electro-smith Daisy platform 

This is example is intended to be used with POD.

# DaisyExamples/pod/simple_oscilloscope/
cpp and make file included in this directory<br/>
compile with "make" and use "make program-dfu" to load onto your Daisy POD<br/>
Example includes two identical sine wave oscillators.<br/>
Samples are taken from oscViz and plotted on the OLED screen as a rudimentary oscilloscope<br/>
oscAud is used in the AudioCallBack so that you can "hear" what you see on the OLED<br/>

I found that if I use just one Oscillator in both UpdateOled() and AudioCallback() the timing discrepancy between the while loop in main() and the AudioCallback() causes the audio to be really choppy. I'm using two oscillators here as a simple workaround.

Knob 1 modulates the Oscillator frequency (pitch)<br/>
Knob 2 modulates the Oscillator amplitude (volume)<br/>

# DaisyExamples/libdaisy/src/
Custom cpp and header files included so that POD can use the OLED drivers for PATCH<br/>
I've simply taken the OLED related code out of the PATCH source files and included them in this file<br/>

If you include these updated files in your libdaisy folder you will need to rebuild the libraries. You can do that by navigating to your working directory (I’m just using /Desktop/DaisyExamples) and running “./rebuild_libs.sh”.

Note: In my home "rig" I have several additional knobs connected to my POD via breadboard. These files also include changes I've made to allow up to 6 knobs to be used by POD. details concerning the ADC pins are included in the files.
