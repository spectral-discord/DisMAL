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
#include "DissonanceModel.h"
#include "OvertoneDistribution.h"
#include "Preprocessor.h"

/** A modular class for calculating dissonance.
 
    With this class, you can modularly use dissonance models and preprocessing algorithms to calculate the dissonance of timbres, intervals, and chords. Dissonance values can be calculated for a single chord or many, as well as for ranges of frequencies that result in dissonance curves and surfaces (collectively refered to in DisMAL as dissonance maps).
 
    Dissonance maps are limited to two- and three-dimensions (one and two frequency dimensions, respectively, plus a dissonance dimension). This is because visual representations of dense, 4+ dimensional data ultimately limit the clarity and usefulness of dissonance maps. More than three overtone distributions can be used to create dissonance maps, but only one or two distributions can have variable frequencies that step over the frequency range. The rest must have fixed fundamental frequencies.
*/
class DissonanceCalc
{
public:
    /** Creates a DissonanceCalc object. */
    DissonanceCalc();
    
    /** Creates a copy of another DissonanceCalc object. */
    DissonanceCalc (const DissonanceCalc& otherCalc);
    
    /** Destructor. */
    virtual ~DissonanceCalc();
    
    //==============================================================================
    /** Sets the dissonance model to use in dissonance calculations. */
    void setModel (DissonanceModel* newModel);
    
    /** Returns the name of the model being used in dissonance calculations. */
    String getModelName() const;
    
    /** Adds a Preprocessor object to the end of the preprocessors array.
     
        If your preprocessors must be arranged in a specific order, you should call setPreprocessorIndex to arrange the preprocessors in the correct order.
     
        @see setPreprocessorIndex
    */
    void addPreprocessor (Preprocessor& newPreprocessor);
    
    /** Moves a Preprocessor object within the preprocessors array from one index to another.
     
        This enables rearranging of the order in which preprocessors are applied to sets of overtone distributions.
    */
    void setPreprocessorIndex (int currentIndex, int newIndex);
    
    /** Returns the name of the Preprocessor object at an index within the preprocessors array. */
    String getPreprocessorNameAtIndex (int index) const;
    
    /** Removes a Preprocessor object from the preprocessors array. */
    void removePreprocessor (int index);
    
    /** Clears the preprocessors array. */
    void clearPreprocessors();
    
    //==============================================================================
    /** Adds an overtone distribution to be included in dissonance calculations.
     
        The array can hold multiple copies of the same OvertoneDistribution object, which should be the case when calculating the dissonance of intervals or chords comprised of notes with the same overtone distribution.
    */
    void addOvertoneDistribution (OvertoneDistribution* newDistribution);
    
    /** Removes an overtone distribution from the array of distributions to be included in dissonance calculations.
     
        If removing from the middle of the distributions array, all following distributions will have their index shifted. Whenever this is called, be sure refresh your GUI or update anything that could attempt to access an incorrect or non-existant distribution.
    */
    void removeOvertoneDistribution (int distributionNum);
    
    /** Clears the array of overtone distributions to be included in dissonance calculations. */
    void clearOvertoneDistributions();
    
    /** Returns the number of overtone distributions in the distributions array.
     
        The result could differ from the number of distributions being used in dissonance calculations, due to the possiblity of muted distributions and partials.
    */
    int numOvertoneDistributions() const noexcept;
    
    /** Returns a const reference to an overtone distribution in the distributions array. */
    OvertoneDistribution* getDistributionReference (int index) noexcept;
    
    //==============================================================================
    /** Enables the calculation of partial dissonance values.
     
        Setting this to true will cause dissonances valued for individual partials to be summed and stored in the partial's dissonance member - Partial::dissonance
     
        The resulting values represent the amount of dissonance that a partial contributes to the overall dissonance of a sound or chord.
     
        Dissonance map and multi-chord calculations will not sum partial dissonances, as the dissonance values would be overwritten with each dissonance calculation. This is to keep processing and memory requirements low, rather than calculating and storing multidimensional arrays of partial dissonance values for each calculation (of which there can be many). Methods that need to retrieve partial dissonance values in these contexts should call calculateDissonance with a specific chord/step to get the desired partial dissonances.
    */
    void setSumPartialDissonances (bool calculate) noexcept;
    
    /** Returns true if summing the dissonance of each partial.
     
        @see setSumPartialDissonances
    */
    bool summingPartialDissonances() const noexcept;
    
    //==============================================================================
    /** Calculates the dissonance of a singal overtone distribution or a set of distributions forming a chord.
    
        This simply calls DissonanceModel::calculateDissonance and returns the output.
    */
    float calculateDissonance() const;
    
    //==============================================================================
    //                  Calculations of multiple intervals or chords
    //==============================================================================
    
    /** @name Multi-chord calculation functions
     
        These members are used when performing multiple dissonance calculations on sets of predetermined chords.
    */
    ///@{
    
    /** Adds a chord to the list of chords to include in dissonance calculations.
     
        This does not set frequency or amplitude values for any distribution objects for the new chord. setFreqInChord and setAmpInChord must be called for all distributions in the new chord before calling calculateDissonances.
    */
    void addChord();
    
    /** Sets a distribution's frequency for a particular chord.
     
        @param chordIndex The index of the chord in which a distribution's frequency is being set.
        @param distributionIndex The index of the distribution whose frequency is being set for a chord.
        @param newFreq The frequency value to set for the distribution.
        @see addChord
    */
    void setFreqInChord (int chordIndex, int distributionIndex, float newFreq);
    
    /** Sets a distribution's amplitude for a particular chord.
     
        @param chordIndex The index of the chord in which a distribution's amplitude is being set.
        @param distributionIndex The index of the distribution whose amplitude is being set for a chord.
        @param newAmp The amplitude value to set for the distribution.
        @see addChord
    */
    void setAmpInChord (int chordIndex, int distributionIndex, float newAmp);
    
    /** Returns a distribution's frequency for a particular chord. */
    float getFreqInChord (int chordIndex, int distributionIndex) const;
    
    /** Returns a distribution's amplitude for a particular chord. */
    float getAmpInChord (int chordIndex, int distributionIndex) const;
    
    /** Removes a chord from the list of chords to include in dissonance calculations. */
    void removeChord (int chordNum);
    
    /** Removes all chords from the list of chords to be included in multi-chord dissonance calculations. */
    void clearChords();
    
    /** Returns the number of chords to be included in multi-chord dissonance calculations. */
    int numChords() const noexcept;
    
    /** Calculates dissonance values for a list of chords.
     
        This function calculates dissonance values for a set of overtone distributions with a list of predefined chord structures (ie, sets of frequencies and amplitudes for each overtone distribution), yielding a dissonance value for each interval or chord.
    */
    void calculateDissonances();
    
    /** Returns the dissonance of a chord that was included in the previous multi-chord dissonance calculations. */
    float getChordDissonance (int chordNum) const;
    
    ///@}
    
    //==============================================================================
    //                  Range-based calculations / Dissonance maps
    //==============================================================================
    
    /** @name Dissonance map functions
     
        These members are used when performing range-based calculations to create 2D and 3D dissonance maps.
    */
    ///@{
    
    /** Flags to indicate the dimensionality of the dissonance map.
     
        As dissonance maps are limited to 2D and 3D, the only acceptable values are twoDimensional and threeDimensional.
    */
    enum Dimensionality
    {
        twoDimensional = 2,
        threeDimensional = 3
    };
    
    /** Sets the dimensionality of the dissonance map.
     
        As dissonance maps are currently limited to 2D and 3D, the only acceptable values are twoDimensional and threeDimensional.
     
        Setting the dimensionality with Dimensionality::twoDimensional means that only one distribution will have a variable frequency that steps across the frequency range, creating a vector of dissonance values.
     
        Setting the dimensionality with Dimensionality::threeDimensional means that two distributions will step across the frequency range, creating a 2D matrix of dissonance values.(Although it is a 2D matrix, it represents a 3D dissonance map because there are two frequency dimensions and one dissonance dimension.)
    */
    void setNumDimensions (Dimensionality newDimensionality) noexcept;
    
    /** Returns the dimensionality of the dissonance map. */
    Dimensionality getNumDimensions() const noexcept;
    
    //==============================================================================
    /** Sets the range of frequencies to use when calculating dissonance maps.
     
        @param startFreq The frequency (in Hz) from which variable-frequency overtone distributions will begin to increase in frequency.
        @param endFreq The final frequency at which a dissonance calculation will occur. endFreq should be higher than startFreq.
    */
    virtual void setRange (float startFreq, float endFreq) noexcept;
    
    /** Returns the range of frequencies to use when calculating dissonance maps. */
    Range<float> getRange() const noexcept;
    
    /** Sets the number of data points to calculate in a dissonance map.
     
        For creating smooth plots of dissonance maps, this could be set to the number of pixels wide and/or tall of the plot.
    */
    virtual void setNumSteps (int newNumSteps) noexcept;
    
    /** Returns the number of data points to calculate in a dissonance map. */
    int getNumSteps() const noexcept;
    
    /** Sets the type of step used.
     
        Setting this to true will cause dissonance calculations to use logarithmic step sizes that increase in size as frequencies increase. This ensures that the resolution of the dissonance map scales (approximately) with a number of pitch perception observations, including the perception of frequency distance and just noticeable difference.
     
        Setting this to false will cause dissonance calculations to use linear (static) step sizes. It could be useful to provide a greater dissonance resolution for higher frequencies. One particular use is to have greater resolution at areas of interest when choosing a repitition ratio or when tempering.
    */
    void useLogarithmicSteps (bool useLogSteps) noexcept;
    
    /** Returns true if using logarithmic step sizes. */
    bool usingLogarithmicSteps() const noexcept;
    
    /** Returns the step size between each data point in a dissonance map.
     
        This could return a logarithmic step size or linear step size, each having their own operations. It is advised that this is called in tandem with usingLogarithmicSteps, if operations are to be done using this result. Different implementations are likely needed when switching between logarithmic and linear steps.
     
        @see useLogarithmicSteps
    */
    float getStepSize() const noexcept;
    
    //==============================================================================
    /** Sets the variable-frequency overtone distribution in a 2D dissonance map.
     
        The overtone distribution at the supplied index in the distributions array will have a non-fixed frequency that steps over the frequency range throughout the dissonance calculations.
     
        @param distributionIndex The index (in the distributions array) of the overtone distribution that will have a non-fixed frequency.
    */
    void set2dVariableDistribution (int distributionIndex) noexcept;
    
    /** Sets the x-axis non-fixed overtone distribution in a 3D dissonance map.
     
        @param distributionIndex The index (in the distributions array) of the overtone distribution with a non-fixed frequency that will be represented by the x-axis as it traverses the frequency range.
     
        @see set2dVariableDistribution
    */
    void setXVariableDistribution (int distributionIndex) noexcept;
    
    /** Sets the y-axis non-fixed overtone distribution in a 3D dissonance map.
     
        @param distributionIndex The index (in the distributions array) of the overtone distribution with a non-fixed frequency that will be represented by the y-axis as it traverses the frequency range.
     
        @see set2dVariableDistribution
    */
    void setYVariableDistribution (int distributionIndex) noexcept;
    
    /** Returns the overtone distribution in a 2D dissonance map that has a non-fixed frequency. */
    int get2dVariableDistributionIndex() const noexcept;

    /** Returns the x-axis overtone distribution in a 3D dissonance map. */
    int getXVariableDistributionIndex() const noexcept;
    
    /** Returns the index of the y-axis overtone distribution in a 3D dissonance map. */
    int getYVariableDistributionIndex() const noexcept;
    
    //==============================================================================
    /** Checks if any data needs to be set in order to calculate a dissonance map. */
    bool isReadyToProcess();
    
    /** Calculates dissonance values for a set of overtone distributions across a range of frequency intervals. */
    virtual void calculateDissonanceMap();
    
    /** Returns the dissonance value stored at the nth step in a 2D dissonance map. */
    float getDissonanceAtStep (int step) const;
    
    /** Returns the dissonance value stored at the (x, y) step in a 3D dissonance map. */
    float getDissonanceAtStep (int xStep, int yStep) const;
    
    /** Returns a pointer to the start of the array of dissonances. */
    float* get2dRawDissonanceData();
    
    ///@}
    
protected:
    /** Contains OvertoneDistribution objects to be used in dissonance calculations. */
    OwnedArray<OvertoneDistribution> distributions;
    
    /** Pointer to the DissonanceModel object to be used in dissonance calculations. */
    std::unique_ptr<DissonanceModel> model;
    
    /** Contains pointers to Preprocessor objects to be applied to overtone distributions before using a dissonance model for dissonance calculations. */
    OwnedArray<Preprocessor> preprocessors;
    bool sumPartialDissonances;
    
    //==============================================================================
    //                  Calculations of multiple specific intervals
    //==============================================================================
    struct FreqAmpPair
    {
        float freq;
        float amp;
    }; typedef struct FreqAmpPair FreqAmpPair;
    
    //The matrix should be structured as: chords[chordIndex][distributionIndex]
    Array<Array<FreqAmpPair>> chords;
    
    Array<float> dissonanceValues;
    
    //==============================================================================
    //                  Range-based calculations / Dissonance maps
    //==============================================================================
    Array<float> map2D;
    
    Array<Array<float>> map3D;
    Range<float> frequencyRange;
    float stepSize;
    int numSteps, varDist, xDist, yDist;
    Dimensionality dimensionality;
    bool logSteps;
    
    //==============================================================================
    /** Calculates the step size of a dissonance map, given a range of frequencies and number of steps.
     
        For linear step sizes, the step size is found with
     
        \f$$\frac{f_{max}-f_{min}}{n}$\f$
     
        and logarithmic step sizes are found with
     
        \f$$\sqrt[n]{\frac{f_{max}}{f_{min}}}$\f$
     
        where \f$n\f$ is the number of steps.
    */
    virtual void setStepSize() noexcept;
    
    /** Increments the frequency of an overtone distribution by the step size.
     
        This is called after each dissonance calculation when creating a dissonance map, to increment the frequency of an overtone distribution. When using logarithmic steps, the frequency is multiplied by the step size.  When using linear steps, the step size is added to the frequency.
     
        @param frequency The frequency to increment.
        @return The incremented frequency.
    */
    virtual float incrementFrequency (float frequency) noexcept;
    
    /** Resizes the array that will hold dissonance values when using calculateRange. */
    void resizeMap();
};
