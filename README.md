# Toot-Toot Modules - A Collection of VCVRack Modules for CS410P - Sound and Music

A collection of VCVRack modules for use in [VCV Rack](https://vcvrack.com/), a modular synthesizer emulator for Linux, MacOS, and Windows.

# Modules In This Pack

## NG Harmonizer
Author: Nicholas Gilbert <gnick@pdx.edu>

Takes in a wave input, calculates the frequency, and provides outputs for sine waves at ratio frequencies that map to half, whole, etc steps above and below the input frequency.    
The four harmonies can be modified by either a control voltage input or directly through the four knobs.

Provides outputs as:
- Standard Sound Outputs, +-5V
- Control Voltage Outputs 0-10V
- Through output (original wave)
- Total, Cumulative Sound Output, scaled to +-5V
- Total, Cumulative Control Voltage Output, scaled to 0-10V

### Testing

See `test-patches` for a series of VCVRack patches that can be loaded to showcase various features of the harmonizer modular.    
The harmonizer patches are labeled `harmonizer_test_#.vcv`

## AB Clock
Author: Adam Barber <embarber@pdx.edu>

A very slow oscillator, useful as a clock or a simple drum machine. Knob adjusts from 0 to 240bpm.

Outputs:
- Saw (inverse), +-5V
- Square, +-5V

## Mark Mod
Author: Mark Williams

Generates a square wave with input frequency that triggers the wave. Frequency is also adjusted with the dial.

Outputs:
- Square,+- 5V

## Rpeggi8r
Author: Cadence Michael

Generates various arpeggiation patterns. Use dials to control the speed, pattern, and pitch for the tones being arpeggiated between.

Outputs:
- Six-tone arpeggiation pattern, +-5V

# Getting Started

1. Build VCVRack from source, following the guide from here: https://vcvrack.com/manual/Building.html#building-rack or download the [Rack-SDK](https://github.com/VCVRack/Rack/issues/258#issuecomment-405119759), and a zipped build of [Rack](https://vcvrack.com/).

2. Follow the instructions for [Building Rack Plugins](https://vcvrack.com/manual/Building.html#building-rack-plugins), replacing the git repository with this repo: https://vcvrack.com/manual/Building.html#building-rack-plugins

Once completed, upon loading Rack, you should see the available plugins from this pack by searching for them using the Rack interface.

# Module ideas

Please see the wiki page for current [module ideas](https://github.com/toot-toot-modules/toot-toot-modules/wiki/Module-Ideas).
Authors have already chosen module ideas to begin working on
and additional ideas are stored here.

# Licensing

See LICENSE.txt and AUTHORS.txt for licensing and authorship information.
