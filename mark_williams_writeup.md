# Mark Williams 
# Write-up for CS410 Project ( my experience )

## What Was Built?
Together we created a module pack for VCV rack, an online open-source virtual modular synth. They also have desktop applications for Windows, Linux and MAC. Individually we each created a module to publish together under a plugin package named toot-toot modules. In my part I created a square wave generator that changes pitch based on a dial, along with a light that blinks and is related to the pitch. When the pitch is past half way on the dial it gets really frantic until it's on all the time. This module builds on the example module provided by the VCV rack team under the BSD license.

## How It Worked Out / Successes
Generating a square wave worked out well, I was able to also generate a saw tooth wave but did not end up using it.

## What Doesn't Work / Challenges
Originally I wanted to trigger a pulse from the square wave off of the input, and make a rudimentary hi-hat machine. This turned out to be more difficult than I had bargained for. I also wanted to create a sweeping saw tooth that oscillated as a function a sine wave, to increase the frequency, then wrap it around. I fiddled with this, but the output didn't quite match what I wanted. Lastly working with inkscape was a bit of a challenge. I added a trumpet to the bottom of my module, imported from a png. I followed a number of guides on how to turn a simple png into a path, but none of them loaded when I started VCV rack. I finally found an svg trumpet with a creative commons license from Twemoji.

## Lessons Learned
Using C++ with well defined APIs is a lot cleaner than the C and C++ I have gotten used to. I have really enjoyed other languages like rust, but I was surprised by how clean new C++ projects are. I need to check out C++ 11, 14 and 17 and see what I've been missing. Overall making a module was a but tedious but they have a good system. I like how VCV has created their plugins and has good tutorials about how to get started. I spent a lot of hours just messing around making cool sounds.

