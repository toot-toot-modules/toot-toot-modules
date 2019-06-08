# Cadence Michael - Project Write-up

## What was built
Rpeggi8r is a generator for various arpeggiation patterns. There are knobs to adjust the pitch of the tones, the speed of the arpeggiation, and the style of arpeggiation.

## How it worked out and other successes
Sine waves for various frequencies are generated based on the values that the
user-controllable knobs are set to. The VCV Rack software provides a current sample time to modules so you can use that to track how much time has elapsed for whatever time-dependant sound processing you want to do. That is used to calculate the phase of sine waves for the tones as well as the speed of arpeggiation between tones.

## What doesn't work and other challenges
The visual design aspect of modules for VCV Rack was unexpectedly difficult. It is a gross combination of manually laying out the functional UI components in raw C++ and placing the accompanying labels in a restricted subset of SVG. VCV Rack doesn't support text and requires paths. That does help ensure a consistent
look to modules regardless of differences in fonts and font rendering between computers but it was a very fiddly and time consuming process to match things up.

## What lessons were learned
If I were to start anew knowing what I do now, I would have deep dived into
the documentation to learn of all the relevant components were available for
my use in the library. I could have saved myself a good amount of time not reimplementing the wheel in spots and used that to work on even more interesting arpeggiation patterns.
