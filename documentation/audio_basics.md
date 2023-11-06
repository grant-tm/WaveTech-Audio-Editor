# Audio Basics
This file briefly describes the physical attributes of sound and how to represent them digitally.

## Physical Sound
In the real world, sound is vibrations in air pressure. These vibrations take the form of waves, specifically sine waves. By combining waves of various amplitudes and frequencies, any sound can be reconstructed. 
### Wave Amplitude
A wave's ampltidue is its displacement from its equillibrium point. We experience sound waves with a higher amplitude as being louder than waves with a lower amplitude.
### Wave Frequency
A wave's frequency, how many times per second it oscillates, is measured in Hertz (Hz). We experience sound waves of higher frequency as having a higher pitch than waves of a lower frequency. Humans can hear sounds of approximately 20 to 20,000 Hz.

## Digital Audio
Sound can be represented digitally as a vector of amplitude measurements. These amplitude mesurements are called samples and they're typically represented as float or double values.
### Sampling Rate
The sampling rate of digital audio is the number of times per second that the wave's amplitude is measured. According to the Nyquist-Shannon sampling theorem, to digitally represent a wave of frequency *F* you must sample the wave's amplitude 2*F* times per second. Audio files typically have a sampling rate of 44,100, allowing digital representation of waves up to 22,500 Hz, the upper limit of human hearing. This means that a one second audio file contains a vector of 44,100 samples.
### Bit-Depth
The bit-depth of an audio file determines how precisely a wave's amplitude can be measured. If a physical wave's amplitude can be any value between 0 and 1, then bit-depth is analogous to the number of decimal places you can use to measure that value between 0 and 1.