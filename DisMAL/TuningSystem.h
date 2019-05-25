/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    DisMAL is provided under the terms of the MIT License
    https://opensource.org/licenses/MIT
 
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/** Container for data describing a tuning system.
 
    This class can store and manipulate a set of intervals representing a scale, along with data that can be used to turn scales into tuning systems that spans the entire hearing range.
*/
class TuningSystem
{
public:
    //==============================================================================
    /** Creates an empty tuning system. */
    TuningSystem();
    
    /** Creates a copy of another tuning system. */
    TuningSystem (const TuningSystem& otherTuningSystem);
    
    /** Destructor. */
    ~TuningSystem();
    
    //==============================================================================
    /** Returns the number of notes in the tuning system. */
    int numNotes() const noexcept;
    
    /** Sets the tuning system's name. */
    void setName (String newName) noexcept;
    
    /** Returns the tuning system's name. */
    String getName() const noexcept;
    
    //==============================================================================
    /** Adds a new interval to the tuning system.
     
        Intervals should have positive values that represent ratios of the intervals' frequencies to the frequency of the tonic as in
     
        \f$$\frac{f_n}{f_0}$\f$
     
        As intervals are represented as ratios to the tonic, it is unnecessary to set intervals[0] as the tonic (doing so would just set it to 1 for every tuning system). Thus, the intervals array should be used to store *intervals to the tonic*. Use the repeat ratio and reference frequency to define the tonic in a more meaningful and useful way.
    */
    void addInterval (float newFrequencyRatio);
    
    /** Sets an interval's frequency ratio to the tonic.
     
        @see addInterval
    */
    void setFreqRatio (int intervalNum, float newFreqRatio);
    
    /** Returns an interval's frequency ratio to the tonic. */
    float getFreqRatio (int intervalNum) const;
    
    /** Removes a frequency ratio. */
    void removeInterval (int intervalNum);
    
    /** Removes all frequency ratio. */
    void clearIntervals();
    
    //==============================================================================
    /** Sets the real frequency of a tonic (first scale degree).
     
        Using the reference frequency, other intervals' real frequency values can be calculated.
    */
    void setReferenceFrequency (float newReferenceFreq) noexcept;
    
    /** Returns the real frequency value of a tonic (first scale degree). */
    float getReferenceFrequency() const noexcept;
    
    //==============================================================================
    /** Sets the repeat ratio.
     
        Also known as a pseudo-octave, this represents the frequency ratio at which the tuning system will repeat, beginning with the repeat ratio as the tonic.
    */
    void setRepeatRatio (float newRepeatRatio) noexcept;
    
    /** Returns the repeat ratio. */
    float getRepeatRatio() const noexcept;
    
    //==============================================================================
    /** Sets the minimum interval.
     
        This value represents the minimum frequency ratio between any two notes and should be greater than or equal to 1. A value of 1 means that there is no minimum, although a tuning system cannot have two notes with the same interval.
    */
    void setMinInterval (float newMinInterval) noexcept;
    
    /** Returns the minimum interval between two notes. */
    float getMinInterval() const noexcept;
    
protected:
    //==============================================================================
    Array<float> intervals;
    String name;
    float referenceFrequency, repeatRatio, minInterval;
    
    //==============================================================================
    /** Returns true if an interval in the intervals array already has the input frequency ratio. */
    bool alreadyContains (double freqToCheck) const;
    
    /** Returns true if the frequency ratio is closer to another interval than the minimum frequency ratio permits.
     
        @see setMinimumInterval
    */
    bool tooCloseToOther (double freqToCheck) const;
};
