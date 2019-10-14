/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "DissonanceCalc.h"

DissonanceCalc::DissonanceCalc()   : model (nullptr)
{
    logSteps = false;
    sumPartialDissonances = true;
    varDist = 0;
    xDist = 0;
    yDist = 0;
    dimensionality = Dimensionality::twoDimensional;
}

DissonanceCalc::DissonanceCalc (const DissonanceCalc& otherCalc)   : model (otherCalc.model->cloneModel())
{
    //preprocessors.addCopiesOf (otherCalc.preprocessors);
    distributions.addCopiesOf (otherCalc.distributions);
    sumPartialDissonances = otherCalc.sumPartialDissonances;
    
    dimensionality = otherCalc.dimensionality;
    varDist = otherCalc.varDist;
    xDist = otherCalc.xDist;
    yDist = otherCalc.yDist;
    logSteps = otherCalc.logSteps;

    chords = otherCalc.chords;
}

DissonanceCalc::~DissonanceCalc()
{
}

//==============================================================================


void DissonanceCalc::setModel (DissonanceModel* newModel)
{
    model = newModel->cloneModel();
}

String DissonanceCalc::getModelName() const
{
    return model->getName();
}

void DissonanceCalc::addPreprocessor (Preprocessor& newPreprocessor)
{
    preprocessors.add (&newPreprocessor);
}

void DissonanceCalc::setPreprocessorIndex (int currentIndex, int newIndex)
{
    preprocessors.move (currentIndex, newIndex);
}

String DissonanceCalc::getPreprocessorNameAtIndex (int index) const
{
    return preprocessors[index]->getName();
}

void DissonanceCalc::removePreprocessor (int index)
{
    preprocessors.remove (index);
}

void DissonanceCalc::clearPreprocessors()
{
    preprocessors.clear();
}


//==============================================================================

void DissonanceCalc::addOvertoneDistribution (OvertoneDistribution* newDistribution)
{
    distributions.add (newDistribution);
}

void DissonanceCalc::removeOvertoneDistribution (int distributionNum)
{
    distributions.remove (distributionNum);
}

void DissonanceCalc::clearOvertoneDistributions()
{
    distributions.clear();
}

int DissonanceCalc::numOvertoneDistributions() const noexcept
{
    return distributions.size();
}

OvertoneDistribution* DissonanceCalc::getDistributionReference (int index) noexcept
{
    return distributions[index];
}

//==============================================================================


void DissonanceCalc::setSumPartialDissonances (bool sum) noexcept
{
    sumPartialDissonances = sum;
}

bool DissonanceCalc::summingPartialDissonances() const noexcept
{
    return sumPartialDissonances;
}

//==============================================================================
//                  Dissonance calculation
//==============================================================================

float DissonanceCalc::calculateDissonance() const
{
    float dissonance;
    
    if (summingPartialDissonances())
    {
        for (auto* dist : distributions)
        {
            dist->clearPartialDissonances();
        }
    }
    
    OwnedArray<OvertoneDistribution> tempDistributions;

    tempDistributions.clear();
    tempDistributions.addCopiesOf (distributions);
    
    for (auto pre : preprocessors)
    {
        pre->process (tempDistributions);
    }
    
    dissonance = model->calculateDissonance (tempDistributions, sumPartialDissonances);
    
    if (summingPartialDissonances())
    {
        for (int i = 0; i < distributions.size(); ++i)
        {
            distributions[i]->addDissonanceToFundamental (tempDistributions[i]->getDissonanceOfFundamental());
            
            for (int j = 0; j < distributions[i]->numPartials(); ++j)
            {
                distributions[i]->addPartialDissonance (j, tempDistributions[i]->getPartialDissonance (j));
            }
        }
    }
    
    return dissonance;
}

//==============================================================================
//                  Calculations of multiple specific intervals
//==============================================================================

void DissonanceCalc::addChord()
{
    chords.resize (chords.size() + 1);
}

void DissonanceCalc::setFreqInChord (int chordIndex, int distributionIndex, float newFreq)
{
    chords[chordIndex].getReference (distributionIndex).freq = newFreq;
}

void DissonanceCalc::setAmpInChord (int chordIndex, int distributionIndex, float newAmp)
{
    chords[chordIndex].getReference (distributionIndex).amp = newAmp;

}

float DissonanceCalc::getFreqInChord (int chordIndex, int distributionIndex) const
{
    return chords[chordIndex][distributionIndex].freq;
}

float DissonanceCalc::getAmpInChord (int chordIndex, int distributionIndex) const
{
    return chords[chordIndex][distributionIndex].amp;
}

void DissonanceCalc::removeChord (int chordNum)
{
    chords.remove (chordNum);
}

void DissonanceCalc::clearChords()
{
    chords.clear();
}

int DissonanceCalc::numChords() const noexcept
{
    return chords.size();
}

void DissonanceCalc::calculateDissonances()
{
    OwnedArray<OvertoneDistribution> tempDistributions;

    for (int i = 0; i < chords.size(); ++i)
    {
        tempDistributions.clear();
        tempDistributions.addCopiesOf (distributions);
        
        for (int j = 0; j < distributions.size(); ++j)
        {
            tempDistributions[j]->setFundamental (chords[i][j].freq, chords[i][j].amp);
        }
        
        for (int i = 0; i < preprocessors.size(); i++)
        {
            preprocessors[i]->process (tempDistributions);
        }
        
        dissonanceValues.set (i, model->calculateDissonance (tempDistributions, false));
    }
}

float DissonanceCalc::getChordDissonance (int chordNum) const
{
    return dissonanceValues[chordNum];
}

//==============================================================================
//                  Range-based calculations / Dissonance maps
//==============================================================================

void DissonanceCalc::setNumDimensions (Dimensionality newDimensionality) noexcept
{
    dimensionality = newDimensionality;
    
    resizeMap();
}

DissonanceCalc::Dimensionality DissonanceCalc::getNumDimensions() const noexcept
{
    return dimensionality;
}

//==============================================================================


void DissonanceCalc::setRange (float startFreq, float endFreq) noexcept
{
    
    // You might want to add a method here to let users know the following, when bad inputs are used.
    jassert (startFreq > 0);                // Frequencies must be positive.
    jassert (endFreq > startFreq);          // endFreq must be greater than startFreq.
    
    if (startFreq > 0 && endFreq > startFreq)
    {
        frequencyRange.setStart (startFreq);
        frequencyRange.setEnd (endFreq);
    }
    
    if (numSteps > 1)
        setStepSize();
}

Range<float> DissonanceCalc::getRange() const noexcept
{
    return frequencyRange;
}

void DissonanceCalc::setNumSteps (int newNumSteps) noexcept
{
    if (newNumSteps > 0)
    {
        numSteps = newNumSteps;
    
        resizeMap();
    
        if (! frequencyRange.isEmpty())
            setStepSize();
    }
    
    // numSteps must be positive
    // It should really be more than a handful. Otherwise, use calculateDissonances() to target specific intervals.
    jassert (newNumSteps > 0);
}

int DissonanceCalc::getNumSteps() const noexcept
{
    return numSteps;
}

void DissonanceCalc::useLogarithmicSteps (bool useLogSteps) noexcept
{
    logSteps = useLogSteps;
    
    setStepSize();
}

bool DissonanceCalc::usingLogarithmicSteps() const noexcept
{
    return logSteps;
}

float DissonanceCalc::getStepSize() const noexcept
{
    return stepSize;
}

//==============================================================================


void DissonanceCalc::set2dVariableDistribution (int distributionIndex) noexcept
{
    varDist = distributionIndex;
}

int DissonanceCalc::get2dVariableDistributionIndex() const noexcept
{
    return varDist;
}

void DissonanceCalc::setXVariableDistribution (int distributionIndex) noexcept
{
    xDist = distributionIndex;
}

int DissonanceCalc::getXVariableDistributionIndex() const noexcept
{
    return xDist;
}

void DissonanceCalc::setYVariableDistribution (int distributionIndex) noexcept
{
    yDist = distributionIndex;
}

int DissonanceCalc::getYVariableDistributionIndex() const noexcept
{
    return yDist;
}

//==============================================================================


bool DissonanceCalc::isReadyToProcess()
{
    if (distributions.size() > 1
        && ! frequencyRange.isEmpty()
        && model != nullptr
        && numSteps > 1)
    {
        for (auto* distribution : distributions)
        {
            if (distribution->getFundamentalFreq() <= 0
                && ! frequencyRange.contains (distribution->getFundamentalFreq()
                                              * frequencyRange.getStart()))
            {
                return false;
            }
            
            for (int p = 0; p < distribution->numPartials(); ++p)
            {
                if (distribution->getFreqRatio (p) <= 0
                    || distribution->getAmpRatio (p) <= 0)
                {
                    return false;
                }
            }
        }
        
        return true;
    }

    return false;
}

void DissonanceCalc::calculateDissonanceMap()
{
    OwnedArray<OvertoneDistribution> tempDistributions;
    
    if (dimensionality == 2)
    {
        float currentFreq = frequencyRange.getStart();
        
        distributions[varDist]->setFundamentalFreq (currentFreq);
        
        for (int i = 0; i < numSteps; ++i)
        {
            tempDistributions.clear();
            tempDistributions.addCopiesOf (distributions);
            
            for (auto* pre : preprocessors)
            {
                pre->process (tempDistributions);
            }
            
            map2D.set (i, model->calculateDissonance (tempDistributions, false));
            
            currentFreq = incrementFrequency (currentFreq);
            distributions[varDist]->setFundamentalFreq (currentFreq);
        }
    }
    else if (dimensionality == 3)
    {
        float currentXFreq = frequencyRange.getStart();
        float currentYFreq = frequencyRange.getStart();

        distributions[xDist]->setFundamentalFreq (currentXFreq);
        distributions[yDist]->setFundamentalFreq (currentYFreq);
        
        for (int xStep = 0; xStep < numSteps; ++xStep)
        {
            for (int yStep = 0; yStep < numSteps; ++yStep)
            {
                tempDistributions.clear();
                tempDistributions.addCopiesOf (distributions);
                
                for (auto* pre : preprocessors)
                {
                    pre->process (tempDistributions);
                }
                
                map3D[xStep].set (yStep, model->calculateDissonance (tempDistributions, false));
                
                currentYFreq = incrementFrequency (currentYFreq);
                distributions[yDist]->setFundamentalFreq (currentYFreq);
            }
            
            currentXFreq = incrementFrequency (currentXFreq);
            distributions[xDist]->setFundamentalFreq (currentXFreq);
            
            // Reset the Y-axis distribution's frequency to the starting frequency
            currentYFreq = frequencyRange.getStart();
            distributions[yDist]->setFundamentalFreq (currentYFreq);
        }
    }
}

//==============================================================================

// For use with NLopt
double opt2D (const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    DissonanceCalc temp = *static_cast<DissonanceCalc*> (data);
    
    temp.getDistributionReference (temp.get2dVariableDistributionIndex())->setFundamentalFreq (x[0]);
    
    return temp.calculateDissonance();
}

void DissonanceCalc::optimize2D (bool minimize, float lowerBound, float upperBound)
{
    nlopt::vfunc o = &opt2D;
    nlopt::opt optim (nlopt::LN_COBYLA, 1);
    
    Array<float>& optimizedValues = minimize ? minima : maxima;
    Array<float> dissValues;
    
    if (minimize)
    {
        minima.clear();
        optim.set_min_objective (o, this);
    }
    else
    {
        maxima.clear();
        optim.set_max_objective (o, this);
    }
    
    optim.set_lower_bounds (lowerBound <= 0 ? frequencyRange.getStart() : lowerBound);
    optim.set_upper_bounds (upperBound <= 0 ? frequencyRange.getEnd() : upperBound);
    optim.set_xtol_abs (0.0001);

    std::vector<double> x (1, 0);
    double dissonanceValue = 0;
    Range<float> tooClose;
    int index;
    
    for (float thisX = frequencyRange.getStart();
         thisX < frequencyRange.getEnd();
         thisX *= 1.0008)
    {
        x[0] = thisX;
        optim.optimize (x, dissonanceValue);
        
        if (! optimizedValues.contains (x[0]))
        {
            tooClose.setStart (x[0] / 1.001);
            tooClose.setEnd (x[0] * 1.001);
            
            optimizedValues.addUsingDefaultSort (x[0]);
            index = optimizedValues.indexOf (x[0]);
            dissValues.insert (index, dissonanceValue);

            for (int i = 0; i < optimizedValues.size(); ++i)
            {
                if (i != index
                    && tooClose.contains (optimizedValues[i]))
                {
                    if (dissValues[index] < dissValues[i])
                    {
                        optimizedValues.remove (i);
                        dissValues.remove (i);
                    }
                    else
                    {
                        optimizedValues.remove (index);
                        dissValues.remove (index);
                    }
                }
            }
        }
    }
}

//==============================================================================

float DissonanceCalc::getDissonanceAtStep (int step) const
{
    return map2D[step];
}

float DissonanceCalc::getDissonanceAtStep (int xStep, int yStep) const
{
    return map3D[xStep][yStep];
}

float DissonanceCalc::getDissonanceAtFreq (float freq) const
{
    distributions[get2dVariableDistributionIndex()]->setFundamentalFreq (freq);
    
    return calculateDissonance();
}

float DissonanceCalc::getDissonanceAtFreq (float xFreq, float yFreq) const
{
    distributions[getXVariableDistributionIndex()]->setFundamentalFreq (xFreq);
    distributions[getYVariableDistributionIndex()]->setFundamentalFreq (yFreq);

    return calculateDissonance();
}

float DissonanceCalc::getFrequencyAtStep (float step)
{
    return logSteps
           ? pow (stepSize, step) * frequencyRange.getStart()
           : stepSize * step + frequencyRange.getStart();
}

float DissonanceCalc::getFreqRatioAtStep (float step)
{
    return getFrequencyAtStep (step) / frequencyRange.getStart();
}

float DissonanceCalc::getStepOfFrequency (float freq)
{
    return logSteps
           ? log (freq / frequencyRange.getStart()) / log (stepSize)
           : (freq - frequencyRange.getStart()) / stepSize;
}

Array<float> DissonanceCalc::getOptimalFreqs (bool getMinima)
{
    return getMinima ? minima : maxima;
}

float* DissonanceCalc::get2dRawDissonanceData()
{
    return map2D.getRawDataPointer();
}

//==============================================================================


void DissonanceCalc::setStepSize() noexcept
{
    if (usingLogarithmicSteps())
    {
        stepSize = pow (frequencyRange.getEnd() / frequencyRange.getStart(), 1.0 / numSteps);
    }
    else
    {
        stepSize = (frequencyRange.getEnd() - frequencyRange.getStart()) / numSteps;
    }
}

float DissonanceCalc::incrementFrequency (float frequency) noexcept
{
    if (usingLogarithmicSteps())
        return frequency * stepSize;
    
    return frequency + stepSize;
}

void DissonanceCalc::resizeMap() 
{
    if (dimensionality == 2)
    {
        map2D.resize (numSteps);
    }
    else if (dimensionality == 3)
    {
        map3D.resize (numSteps);
        
        for (int i = 0; i < numSteps; ++i)
        {
            map3D[i].resize (numSteps);
        }
    }
}


