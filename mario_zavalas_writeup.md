# Mario Zavalas
# Write-up for CS410 Project 

## What Was Built?
I created a complex oscillator using FM synthesis for a group module pack for the open source program VCV Rack. The oscillator itself is opaque by design but I'll explain its functionallity here:

The module consists of two oscillators, dubbed Queen and Drone. Drone is an oscillator capable of creating square and sawtooth waves, with interpolation between the two using the lower red knob. The pulse width of the square wave can be changed by the lower left blue knob, and the degree to which the signal is modulated is controlled by the lower green knob. The Drone oscillator functions as the carrier. The Queen oscillator is the modulator. It can create square, sawtooth, triangle, and sine waves, with varying interpolations between these waves as well. This is controlled by the upper red knob. The upper blue knob will change the pulse width of the square wave (so this doesn't always have an affect depending on the type of wave being output), and the upper green knob will control the degree to which the Queen is self-oscillating. The white knob in the upper right corner will output a triangle, sawtooth, or sine wave (or an interpolation between). This is fed back into the Queen to create a feedback loop for extra bzzzzz. The 1 Volt per octave input is fed into both Queen and Drone so that the tones produced are relatively consonant (relatively). The result is an unpredictable voice that varies from high pitched whines to throbbing basslines.

## How It Worked Out
The actual design process and coding worked well. It took some trial and error to find consistently interesting combinations of modulations, but I'm mostly happy with the results. I do wish I had included a looping envelope for interesting drones, or individual filters for each oscillator, but these only occured to me towards the end of the project.

## What Doesn't Work
The original plan was to create a 5 or 6 oscillator based module, with a bank of synthesis algorithms that could be selected between with a knob, with the potential for multiple or single voices, depending. I abandoned this idea not because of difficulty in implementation (I have some old code for "Pentoscillator" sitting around still), but difficulty in actually laying out the module. As Nicholas has already stated, getting inputs and outputs lined up reasonably is a pain in the ass. I actually abandoned CV input for the knobs because it would have been too much hassle with no learning-reward.

## Lessons Learned
VCV Rack is a wonderful program to use but as far as designing modules goes, I think I'd rather venture into physical hardware. I hope to never use Inkscape again in my life.

