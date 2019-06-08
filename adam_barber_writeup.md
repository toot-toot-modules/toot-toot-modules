## What was built
ABClock is a fairly simple clock, with adjustable bpm between 0 and 240. It produces both an inverse sawtooth wave and a square wave. Mostly it's an oscillator that goes much slower than VCO-1 or VCO-2. While it was intended as a clock, it turns out to sound like an interesting start to a drum machine when hooked up to a mixer, or it can be used as the gate input on an ADSR for another oscillator.

## How it worked
This module uses the engine time to keep track of the phase, then calculates a voltage based on the phase. The square wave function is noncontinous by default, but the saw wave is not. However, it looks like sounds trigger in vcvrack whenever the voltage of the square wave changes, so I had to double up the saw wave to match the BPM of the square wave. Figuring out how to change the equation to do that ended up being the interesting part - I spent a bunch of time trying to make a continuous function that would work for this before realizing that was silly and I ought to just break the equation up based on the phase.

## What doesn't work
I suspect there's a better way to set the limits of the BPM knob, but I can't find documentation on any of the provided components, so will need to read the source to figure out if there are any limits on the output voltages of knobs. It would also be nice to have a way to sync multiple clocks to build more complex polyrhythmic ones, though this might be more easily implemented inside the module.

## What lessons were learned
As a result of this I finally understood what the phase means when dealing with waves. Thinking about waves in terms of the phase rather than time made things much easier to reason about. I didn't put as much time into this as I wanted due to capstone and various personal things, but with this newfound understanding I finally feel like I could go back and do more. The majority of the time I spent on this project went towards trying to understand terminology well enough to figure out what was going on with the sample module.
Inkscape is also not fun to work with, and I'd do well to find another SVG editor in the future, or at least get DPI scaling for it working properly.
