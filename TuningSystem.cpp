/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    DisMAL is provided under the terms of the MIT License
    https://opensource.org/licenses/MIT
 
  ==============================================================================
*/

#include "TuningSystem.h"

TuningSystem::TuningSystem()
{
    minInterval = 1;
    repeatRatio = 0;
    referenceFrequency = 0;
    name = "untitled";
}

TuningSystem::TuningSystem (const TuningSystem& otherTuningSystem)
{
    intervals = otherTuningSystem.intervals;
}

TuningSystem::~TuningSystem()
{
}

//==============================================================================


int TuningSystem::numNotes() const noexcept
{
    return intervals.size() + 1;        // Add 1 to account for the tonic
}

void TuningSystem::setName (String newName) noexcept
{
    name = newName;
}

String TuningSystem::getName() const noexcept
{
    return name;
}

//==============================================================================


void TuningSystem::addInterval (float newFrequencyRatio)
{
    if (newFrequencyRatio > 1
        && ! alreadyContains (newFrequencyRatio)
        && ! tooCloseToOther (newFrequencyRatio))
    {
        intervals.add (newFrequencyRatio);
    }
    
    // You may want to add code to let users know that the input frequency is already in the
    // array, closer than minimumInterval permits, or must be > 1, if these are getting triggered.
    jassert (newFrequencyRatio > 1);
    jassert (! alreadyContains (newFrequencyRatio));
    jassert (! tooCloseToOther (newFrequencyRatio));
}

void TuningSystem::setFreqRatio (int intervalNum, float newFrequencyRatio)
{
    if (newFrequencyRatio > 1
        && ! alreadyContains (newFrequencyRatio)
        && ! tooCloseToOther (newFrequencyRatio))
    {
        intervals.getReference (intervalNum) = newFrequencyRatio;
    }
    
    // You may want to add code to let users know that the input frequency is already in the
    // array, closer than minimumInterval permits, or must be > 1, if these are getting triggered.
    jassert (newFrequencyRatio > 1);
    jassert (! alreadyContains (newFrequencyRatio));
    jassert (! tooCloseToOther (newFrequencyRatio));
}

float TuningSystem::getFreqRatio (int intervalNum) const
{
    return intervals[intervalNum];
}

void TuningSystem::removeInterval (int intervalNum)
{
    intervals.remove (intervalNum);
}

void TuningSystem::clearIntervals()
{
    intervals.clear();
}

//==============================================================================


void TuningSystem::setReferenceFrequency (float newReferenceFreq) noexcept
{
    referenceFrequency = newReferenceFreq;
}

float TuningSystem::getReferenceFrequency() const noexcept
{
    return referenceFrequency;
}

//==============================================================================


void TuningSystem::setRepeatRatio (float newRepeatRatio) noexcept
{
    if (newRepeatRatio > intervals.getLast()
        && ! alreadyContains (newRepeatRatio)
        && ! tooCloseToOther (newRepeatRatio))
    {
        repeatRatio = newRepeatRatio;
    }
    
    jassert (newRepeatRatio > intervals.getLast());     // Repeat ratio must be larger than all intervals.
    jassert (! alreadyContains (newRepeatRatio));       // Repeat ratio must be unique
    jassert (! tooCloseToOther (newRepeatRatio));       // Repeat ratio cannot violate minInterval
}

float TuningSystem::getRepeatRatio() const noexcept
{
    return repeatRatio;
}

//==============================================================================


void TuningSystem::setMinInterval (float newMinInterval) noexcept
{
    if (newMinInterval >= 1)
        minInterval = newMinInterval;
    
    jassert (newMinInterval >= 1);      // Minimum interval must be >= 1
}

float TuningSystem::getMinInterval() const noexcept
{
    return minInterval;
}

//==============================================================================


bool TuningSystem::alreadyContains (double freqToCheck) const
{
    for (int i = 0; i < intervals.size(); ++i)
    {
        if (intervals[i] == freqToCheck)
            return true;
    }
    
    return false;
}

bool TuningSystem::tooCloseToOther (double freqToCheck) const
{
    Range<float> tooClose;
    
    tooClose.setStart (1 / minInterval);
    tooClose.setStart (minInterval);
    
    if (tooClose.contains (freqToCheck))
        return true;
    
    for (int i = 0; i < intervals.size(); ++i)
    {
        if (tooClose.contains (freqToCheck / intervals[i]))
            return true;
    }
    
    return false;
}
