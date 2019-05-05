/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    DisMAL is provided under the terms of the MIT License
    https://opensource.org/licenses/MIT
 
  ==============================================================================
*/

#include "OvertoneDistribution.h"

#pragma once

/** Base class for implementing preprocessor algorithms. */
class Preprocessor
{
public:
    /** Creates a Preprocessor object. */
    Preprocessor(){}
    
    /** Destructor. */
    virtual ~Preprocessor(){}
        
    /** Processes an array of overtone distributions. */
    virtual void process (OwnedArray<OvertoneDistribution>& distributions) = 0;
    
    String getName()
    {
        return name;
    }
    
private:
    String name;
};
