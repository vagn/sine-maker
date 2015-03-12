# sine-maker
Testing out ideas for a tonewheel organ.
So far have:
  - phase generator (model rotation of tonewheel)
  - adsr, mainly to get rid of the clicks
  - sine conversion (math.h and some hackery, not CORDIC)
  - full polyphony

Doing it with an eye to an FPGA implementation. If it looks
a little funny, think "This will all be hardware running
in parallel."  The reason for doing it in software first is
that it is easier to compute and try out the various tables
of amplitudes (like the adsr functions).  The same tables
can then be expressed in the FPGA (or computed on the fly.)
