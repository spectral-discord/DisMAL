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

OvertoneDistribution::OvertoneDistribution()
{
    minInterval = 1;
    muted = false;
    name = "untitled";
}

OvertoneDistribution::OvertoneDistribution (const OvertoneDistribution& otherDistribution)
{
    partials = otherDistribution.partials;
    fundamental = otherDistribution.fundamental;
    minInterval = otherDistribution.minInterval;
    name = otherDistribution.name;
    muted = otherDistribution.muted;
}

OvertoneDistribution::~OvertoneDistribution()
{
}

OvertoneDistribution& OvertoneDistribution::operator= (const OvertoneDistribution& otherDistribution)
{
    partials = otherDistribution.partials;
    fundamental = otherDistribution.fundamental;
    minInterval = otherDistribution.minInterval;
    name = otherDistribution.name;
    muted = otherDistribution.muted;
    
    return *this;
}

//==============================================================================


int OvertoneDistribution::numPartials() const noexcept
{
    return partials.size();
}

void OvertoneDistribution::setDistributionName (String newName) noexcept
{
    name = newName;
}

String OvertoneDistribution::getName() const noexcept
{
    return name;
}

//==============================================================================


void OvertoneDistribution::addPartial()
{
    partials.add (Partial());
    partials.sort();
}

void OvertoneDistribution::addPartial (float FreqRatio, float AmpRatio)
{
    jassert (FreqRatio > 0);                     // Frequencies must be positive
    jassert (AmpRatio > 0);                      // Amplitudes must be positive
    
    // You probably want to add error handling code here or in your to let
    // users know that their input violates these criteria, if you
    // don't have another means of ensuring that inputs are valid.
    jassert (! alreadyContains (FreqRatio));      // OvertoneDistribution already contains a partial with this frequency
    jassert (! tooCloseToOther (FreqRatio));      // Frequency is closer to another partial than minInterval permits
    
    if (! alreadyContains (FreqRatio) && ! tooCloseToOther (FreqRatio))
    {
        partials.add (Partial (FreqRatio, AmpRatio));
        partials.sort();
    }
}

void OvertoneDistribution::setFreqRatio (int partialNum, float newFreqRatio)
{
    jassert (newFreqRatio > 0);                  // Frequencies must be positive
    
    // You probably want to add error handling code here or in your app to let
    // users know that their input violates these criteria, if you
    // don't have another means of ensuring that inputs are valid.
    jassert (! alreadyContains (newFreqRatio));   // OvertoneDistribution already contains a partial with this frequency
    jassert (! tooCloseToOther (newFreqRatio));   // Frequency is closer to another partial than minInterval permits
    
    if (! alreadyContains (newFreqRatio) && ! tooCloseToOther (newFreqRatio))
    {
        partials.getReference(partialNum).freq = newFreqRatio;
        partials.sort();
    }
}

float OvertoneDistribution::getFreqRatio (int partialNum) const
{
    return partials[partialNum].freq;
}

float OvertoneDistribution::getRealFreq (int partialNum) const
{
    return partials[partialNum].freq * fundamental.freq;
}

void OvertoneDistribution::setAmpRatio (int partialNum, float newAmpRatio)
{
    jassert (newAmpRatio > 0);                   // Amplitudes must be positive
    
    partials.getReference(partialNum).amp = newAmpRatio;
}

float OvertoneDistribution::getAmpRatio (int partialNum) const
{
    return partials[partialNum].amp;
}

float OvertoneDistribution::getRealAmp (int partialNum) const
{
    return partials[partialNum].amp * fundamental.amp;
}

void OvertoneDistribution::removePartial (int partialNum)
{
    partials.remove (partialNum);
}

void OvertoneDistribution::clearPartials()
{
    partials.clear();
}

//==============================================================================


void OvertoneDistribution::setFundamental (float fundamentalFreq, float fundamentalAmp) noexcept
{
    jassert (fundamentalFreq > 0);                  // Frequencies must be positive
    jassert (fundamentalAmp > 0);                   // Amplitudes must be positive
    
    fundamental.freq = fundamentalFreq;
    fundamental.amp = fundamentalAmp;
}

void OvertoneDistribution::setFundamentalFreq (float newFundamentalFreq) noexcept
{
    jassert (newFundamentalFreq > 0);               // Frequencies must be positive
    
    fundamental.freq = newFundamentalFreq;
}

float OvertoneDistribution::getFundamentalFreq() const noexcept
{
    return fundamental.freq;
}

void OvertoneDistribution::setFundamentalAmp (float newFundamentalAmp) noexcept
{
    jassert (newFundamentalAmp > 0);                // Amplitudes must be positive
        
    fundamental.amp = newFundamentalAmp;
}

float OvertoneDistribution::getFundamentalAmp() const noexcept
{
    return fundamental.amp;
}

void OvertoneDistribution::muteFundamental (bool mute) noexcept
{
    fundamental.muted = mute;
}

bool OvertoneDistribution::fundamentalIsMuted() const noexcept
{
    return fundamental.muted;
}

//==============================================================================


void OvertoneDistribution::mute (bool mute) noexcept
{
    muted = mute;
}

bool OvertoneDistribution::isMuted() const noexcept
{
    return muted;
}

void OvertoneDistribution::mutePartial (int partialNum, bool mute)
{
    partials.getReference(partialNum).muted = mute;
}

bool OvertoneDistribution::partialIsMuted (int partialNum) const
{
    return partials[partialNum].muted;
}

//==============================================================================


void OvertoneDistribution::addPartialDissonance (int partialNum, float dissonanceToAdd)
{
    partials.getReference(partialNum).dissonance += dissonanceToAdd;
}

void OvertoneDistribution::addDissonanceToFundamental (float dissonanceToAdd) noexcept
{
    fundamental.dissonance += dissonanceToAdd;
}

float OvertoneDistribution::getPartialDissonance (int partialNum) const
{
    return partials[partialNum].dissonance;
}

float OvertoneDistribution::getDissonanceOfFundamental() const noexcept
{
    return fundamental.dissonance;
}

float OvertoneDistribution::getTotalDissonance() const noexcept
{
    float dissonance = fundamental.dissonance;
    
    for (int i = 0; i < partials.size(); ++i)
        dissonance += partials[i].dissonance;
    
    return dissonance;
}

void OvertoneDistribution::clearPartialDissonances()
{
    fundamental.dissonance = 0;
    
    for (int i = 0; i < numPartials(); ++i)
    {
        partials.getReference(i).dissonance = 0;
    }
}

//==============================================================================


void OvertoneDistribution::setMinInterval (float newMinInterval) noexcept
{
    jassert (newMinInterval >= 1);                  // Minimum interval must be >= 1.  See setMinInterval() documentation.
    
    minInterval = newMinInterval;
}

float OvertoneDistribution::getMinInterval() const noexcept
{
    return minInterval;
}

//==============================================================================


bool OvertoneDistribution::alreadyContains (double freqToCheck) const
{
    if (freqToCheck == 1)
        return true;
    
    for (int i = 0; i < partials.size(); i++)
    {
        if (freqToCheck == partials[i].freq)
            return true;
    }
    
    return false;
}

bool OvertoneDistribution::tooCloseToOther (double freqToCheck) const 
{
    if (minInterval > 1)
    {
        Range<double> tooClose;
        tooClose.setStart (1 / minInterval);
        tooClose.setEnd (minInterval);
        
        if (tooClose.contains (freqToCheck))
            return true;
        
        for (int i = 0; i < partials.size(); i++)
        {
            if (tooClose.contains (freqToCheck / partials[i].freq))
                return true;
        }
    }
    
    return false;
}
