# gem
Gesture Modulation with Win32 MIDI

## CoolSoft VirtualMidiSynth (https://https://coolsoft.altervista.org/en/virtualmidisynth)
midiOutGetDevCaps():
Number of MIDI output devices = 2
MIDI OUT Device 0 MIDIOUTCAPS:
wMid = 65535
wPid = 65535
vDriverVersion = 525
szPname = VirtualMIDISynth #1
wTechnology = 7
wVoices = 500
wNotes = 500
wChannelMask (hex) = ffff
dwSupport = 1

## Microsoft GS Wavetable Synth Info (Built in to Windows 10)
midiOutGetDevCaps():
- Number of MIDI output devices = 1
- MIDI OUT Device 0 MIDIOUTCAPS:
- wMid = 1
- wPid = 27
- vDriverVersion = 256
- szPname = Microsoft GS Wavetable Synth
- wTechnology = 7
- wVoices = 32
- wNotes = 32
- wChannelMask (hex) = ffff
- dwSupport = 1

## Microsoft GS Wavetable Synth Issues
1. Running Status doesn't seem to work (works with VirtualkMidiSynth). Turn off USE_RUNNING_STATUS in MidiOut.cpp.

## General Issues
1. All Control Change messages (for example pan) are subject to instrument implementation in soundfont

## TODO
- Try various sound fonts (https://coolsoft.altervista.org/en/virtualmidisynth#soundfonts)
- Try hardware synth (Casio, MT32)
- Sweeten output with DAW. Possibly record each channel separately and mix/pan in Audacity.
- Create a UWP using C# MIDI
- Try NAudio C# MIDI library
