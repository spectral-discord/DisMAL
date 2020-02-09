/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
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
    
    /** Enables dynamic allocation of child objects via std::unique_ptr. */
    virtual std::unique_ptr<Preprocessor> clone() const = 0;

    
    String getName()
    {
        return name;
    }
    
    String getDescription()
    {
        return description;
    }
    
protected:
    String name = "";
    String description = "";
};

//==============================================================================

/** A simple preprocessor that filters out partials that lie outside of the human hearing range.
 
    Basically, this acts as a bandpass filter. By default, this range is set to 20Hz - 20kHz, but this can be adjusted with class members.
*/
class HearingRangePreprocessor   : public Preprocessor
{
public:
    HearingRangePreprocessor();
    ~HearingRangePreprocessor();
    
    /** Sets the hearing range in which partials will not be muted. */
    void setHearingRange (float lowerLimit, float upperLimit);
    
    /** Returns the hearing range in which partials will not be muted. */
    Range<float> getHearingRange();
    
    /** Processes an array of overtone distributions. */
    void process (OwnedArray<OvertoneDistribution>& distributions) override;
    
    /** For dynamic allocation of unique pointers to generic preprocessor arrays in DissonanceCalc. */
    std::unique_ptr<Preprocessor> clone() const override;
    
private:
    Range<float> hearingRange;
};
