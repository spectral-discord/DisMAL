# DisMAL
C++ library for modeling musical dissonance perception, developing new timbres and tuning systems, and creating tools for music production and analysis. Built with [JUCE](https://github.com/WeAreROLI/JUCE).

With its modular api, you can quickly interchange dissonance models, timbres, and preprocessors. This makes it easy to compare differences in the dissonance of timbres and scales as well as the underlying psychoacoustic models themselves.

##Features Coming Soon
*More Dissonance Models!
*Preprocessors - Equal Loudness Contours, Masking Effects, etc.
*Minima/Maxima Optimization
*Scale Tempering Methods for Robust Tunings
*Scale -> Timbre Optimization
*And More...

##Current Limitations
###Harmonicity
Harmonicity/periodicity models are not currently implemented for two reasons:

Current periodicity algorithms (that I've seen - prove me wrong!) are not generalizable to inharmonic timbres. Including such models would affect the modularity of the library and infringe on the library's goal of making inharmonic timbres accessible to musical composition and analysis.

Also, current research seems to indicate that the effect of harmonicity on dissonance perception is difficult to decouple from culturalization effects, potentially introducing cultural bias into current periodicity models (Harrison & Pearce, 2019). As research in this area continues, I'll continue to consider implementing periodicity models. However, they are currently not planned.
