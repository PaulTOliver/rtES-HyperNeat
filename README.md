rtES-HyperNeat (Real Time Evolving Substrate Hypercube based Neuro-Evolution of
Augmenting Topologies) is a real time neuro-evolution algorithm based on the
papers written by Kenneth Stanley and other researchers at the EPLEX group
([http://eplex.cs.ucf.edu/](http://eplex.cs.ucf.edu/)). More specifically,
rtES-HyperNeat tries to blend the existing ES-HyperNeat
([http://eplex.cs.ucf.edu/ESHyperNEAT/](http://eplex.cs.ucf.edu/ESHyperNEAT/))
algorithm with rtNeat
([http://nn.cs.utexas.edu/?rtNEAT](http://nn.cs.utexas.edu/?rtNEAT)), a previous
version which allowed real time evolution of agents on a virtual field.

The main problem with trying to make ES-HyperNeat real-timed was that the CPPNs
(genotypes) had to be converted into functional Neural Networks (phenotypes) on
a single time-step. This was proven to be impossible, so, as a solution, the
library handles these transformations on separate execution threads. This way,
the algorithm flows seamlessly.

Although several implementations existed for HyperNeat, I've decided to code
mine from scratch, based only on the original papers by EPLEX. My goal is to
construct a simple, easy to use, well documented and tutorialized C++ library
that will allow any user to evolve Neural Networks for any imaginable task. I
also plan to apply this library to my Guppies experiment once its finished (see
my channel for more info on Guppies).
