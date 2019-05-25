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

//==============================================================================
/** Container for data describing a sinusoidal partial. This can store:
 
    - Real frequency and amplitude values
    - Frequency and amplitude ratios to the fundamental partial's frequency and amplitude
    - A value representing the partial's contribution to the dissonance of a sound, interval, or chord
    - The partial's mute status
*/
class Partial
{
public:
    float freq;
    float amp;
    bool muted;
    float dissonance;
    
    /** Enables sorting using Array.sort() */
    friend bool operator< (const Partial& first, const Partial& second) noexcept
    {
        return first.freq < second.freq;
    }
    
    /** Copies the frequency and amplitude another partial. */
    Partial& operator= (const Partial& otherPartial) noexcept
    {
        freq = otherPartial.freq;
        amp = otherPartial.amp;
        muted = otherPartial.muted;
        dissonance = 0;
        
        return *this;
    }
    
    /** Creates a Partial object and initializes it with frequency and amplitude values. */
    Partial (float newFreq, float newAmp) noexcept
    {
        if (newFreq > 0)
            freq = newFreq;
        else
            freq = 0;
        
        if (newAmp > 0)
            amp = newAmp;
        else
            amp = 0;
        
        muted = false;
        dissonance = 0;
    }
    
    /** Creates a Partial object and initializes it with the frequency and amplitude values of another Partial. */
    Partial (const Partial& otherPartial)
    {
        freq = otherPartial.freq;
        amp = otherPartial.amp;
        muted = otherPartial.muted;
        dissonance = 0;
    }
    
    /** Creates an empty Partial object. */
    Partial()
    {
        freq = 0;
        amp = 0;
        dissonance = 0;
        muted = false;
    }
    
    ~Partial(){}
};

/** Container for data describing the overtones of a sound.

    This object contains:

    - A list of Partial objects representing the overtones of a sound, ordered by ascending frequency ratio to the fundamental partial.
    - A Partial object representing the fundamental partial, initialized with real frequency (in Hz) and amplitude (or loudness) values.
    - The mute status of both the entire overtone distribution and inidividual partials.
*/
class OvertoneDistribution
{
public:
    //==============================================================================
    /** Creates an empty OvertoneDistribution object. */
    OvertoneDistribution();
    
    /** Creates a copy of an existing OvertoneDistribution object. */
    OvertoneDistribution (const OvertoneDistribution& otherDistribution);
    
    /** Destructor. */
    ~OvertoneDistribution();
    
    /** Copy assignment operator. */
    OvertoneDistribution& operator= (const OvertoneDistribution& otherDistribution);
    
    //==============================================================================
    /** Returns the number of partials in the distribution including the fundamental. */
    int numPartials() const noexcept;
    
    /** Sets a name for the overtone distribution. */
    void setDistributionName (String newName) noexcept;
    
    /** Returns the overtone distribution's name. */
    String getName() const noexcept;
    
    //==============================================================================
    /** Adds an empty partial.
     
        The partial's frequency and amplitude ratios must be set by calling setFreqRatio and setAmpRatio before dissonance calculations can use the distribution.
    */
    void addPartial();
    
    /** Adds a new partial and sorts partials by ascending frequency.
     
        Frequencies and amplitudes of overtone partials should have values greater than zero that represent a ratio to the fundamental's (or the most salient partial's) frequency and amplitude, as in
     
        \f$$\frac{f_n}{f_0}$\f$
     
        and
     
        \f$$\frac{a_n}{a_0}$\f$
    */
    void addPartial (float FreqRatio, float AmpRatio);
    
    /** Sets a partial's frequency relative to the fundamental frequency.
     
        @param partialNum The index of the partial whose frequency is being set.
        @param newFreqRatio The new frequency ratio between this partial and the fundamental.
     
        @see addPartial
    */
    void setFreqRatio (int partialNum, float newFreqRatio);
    
    /** Sets a partial's amplitude relative to the amplitude of the fundamental.
     
        @see addPartial
    */
    void setAmpRatio (int partialNum, float newAmpRatio);
    
    /** Returns a partial's frequency relative to the fundamental frequency. */
    float getFreqRatio (int partialNum) const;
    
    /** Returns a partial's amplitude relative to the fundamental's amplitude. */
    float getAmpRatio (int partialNum) const;
    
    /** Returns a partial's real frequency in Hz.
     
        Multiplies the partial's frequency ratio by the distribution's fundamental frequency.
    */
    float getRealFreq (int partialNum) const;
    
    /** Returns a partial's real amplitude.
     
        Multiplies the partial's amplitude ratio by the amplitude of the distribution's fundamental.
    */
    float getRealAmp (int partialNum) const;
    
    /** Removes a partial.
     
        If removing from the middle of the partials array, all following partials will have their index shifted. Whenever this is called, be sure refresh your GUI or update anything that could attempt to access an incorrect or non-existant partial.
    */
    void removePartial (int partialNum);
    
    /** Removes all partials. */
    void clearPartials();
    
    //==============================================================================
    /** Sets values for the fundamental's frequency and amplitude.
     
        For most dissonance models, the fundamental partial will need to be initialized with a real frequency value, in Hz.
     
        In some cases, amplitudes may need to have a value denoting a sound pressure level (dB, dB(SPL)) or loudness estimation (sones, LFKS). For some, it may be acceptable to use frequency and amplitude ratios.
    */
    void setFundamental (float fundamentalFreq, float fundamentalAmp) noexcept;
    
    /** Sets a real value for the fundamental frequency.
    
        @param newFundamentalFreq The frequency (very likely in Hz) of the fundamental frequency.
    */
    void setFundamentalFreq (float newFundamentalFreq) noexcept;
    
    /** Sets a real value for the fundamental's amplitude.
     
        @param newFundamentalAmp The amplitude of the fundamental. Different models may require different units. Check the documentation of the models you will be using.
    */
    void setFundamentalAmp (float newFundamentalAmp) noexcept;
    
    /** Returns the fundamental frequency.
    
        @see setFundamental
    */
    float getFundamentalFreq() const noexcept;
    
    /** Returns the fundamental's amplitude.
     
        @see setFundamental
    */
    float getFundamentalAmp() const noexcept;
    
    //==============================================================================
    /** Sets the mute status of the overtone distribution.
     
        For this library, muting is used to exlude overtone distributions from dissonance calculations. It could also be used to mute distributions in synthesis, or to isolate timbral elements if incorporated into a larger Timbre class.
    */
    void mute (bool mute) noexcept;
    
    /** Sets whether to mute a partial, excluding it from dissonance calculations.
    
        @param partialNum The index of the partial to mute or unmute.
        @param mute Set to true to mute, false to unmute.
    */
    void mutePartial (int partialNum, bool mute);
    
    /** Sets the mute status of the fundamental.
     
        @see mute
    */
    void muteFundamental (bool mute) noexcept;
    
    /** Returns true if the overtone distribution should be muted or excluded from dissonance calculations. */
    bool isMuted() const noexcept;
    
    /** Returns true if the partial should be muted and excluded from dissonance calculations.
     
        @param partialNum The index of the partial whose mute status will be checked.
    */
    bool partialIsMuted (int partialNum) const;
    
    /** Returns true if the fundamental is muted.
     
        @see isMuted, partialIsMuted
    */
    bool fundamentalIsMuted() const noexcept;
    
    //==============================================================================
    /** Adds dissonance to partial's dissonance value.
     
        This should be called anytime that a partial contributes to the dissonance of a chord while DissonanceCalc::sumPartialDissonances is set to true.
     
        @param dissonanceToAdd This amount is added to the partial's total dissonance.
    */
    void addPartialDissonance (int partialNum, float dissonanceToAdd);
    
    /** Adds dissonance to the total dissonance of the fundamental partial.
     
        This should be called anytime that the fundamental contributes to the dissonance of a chord while DissonanceCalc::sumPartialDissonances is set to true.
     
        @param dissonanceToAdd This amount is added to the fundamental's total dissonance.
    */
    void addDissonanceToFundamental (float dissonanceToAdd) noexcept;
    
    
    /** Returns the dissonance of a partial.
     
        This usually reflects the amount of dissonance that a partial has contributed to the overall dissonance of a distribution or chord.
    */
    float getPartialDissonance (int partialNum) const;
    
    /** Returns the dissonance of the fundamental partial.
     
        This usually reflects the amount of dissonance that the fundamental partial contributes to the overall dissonance of a distribution or chord.
    */
    float getDissonanceOfFundamental() const noexcept;
    
    /** Returns the sum of all partial dissonances.
    
        This usually reflects the amount of dissonance that an overtone distribution contributes to the overall dissonance of a chord.
    */
    float getTotalDissonance() const noexcept;
    
    /** Sets all partial dissonance values to zero.
     
        This should be called at the beginning of a dissonance calculation for a new chord, unless you are intending to use data from previous frames in your dissonance calculations.
    */
    void clearPartialDissonances();
    
    //==============================================================================
    /** Sets the minimum frequency interval between any two partials.
     
        This value represents the frequency ratio between any two partials and should be greater than or equal to 1. A value of 1 means that there is no minimum. Although an overtone distribution may not have a minimum partial interval, distributions cannot have two partials with the same frequency.
    */
    void setMinInterval (float newMinInteval) noexcept;
    
    /** Returns the minimum frequency interval between any two partials.
     
        @see setMinimumInterval
    */
    float getMinInterval() const noexcept;
    
protected:
    //==============================================================================
    String name;
    Array<Partial> partials;
    Partial fundamental;
    float minInterval;
    bool muted;
    
    //==============================================================================
    /** Returns true if a partial in the distribution already has the input frequency. */
    bool alreadyContains (double freqToCheck) const;
    
    /** Returns true if the input partial is closer in frequency to another partial than the minimum interval permits.
     
        @see setMinimumInterval
    */
    bool tooCloseToOther (double freqToCheck) const;
    
};














