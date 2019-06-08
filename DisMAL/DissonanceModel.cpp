/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    DisMAL is provided under the terms of the MIT License
    https://opensource.org/licenses/MIT
 
  ==============================================================================
*/

#include "DissonanceModel.h"

//==============================================================================
//                              DissonanceModel
//==============================================================================

DissonanceModel::DissonanceModel()
{
}

DissonanceModel::~DissonanceModel()
{
}


//==============================================================================
//                              SpectralInterferenceModel
//==============================================================================

SpectralInterferenceModel::SpectralInterferenceModel()
{
}

SpectralInterferenceModel::~SpectralInterferenceModel()
{
}

//==============================================================================


float SpectralInterferenceModel::calculateDissonance (const OwnedArray<OvertoneDistribution>& distributions,
                                     bool sumPartialDissonances)
{
    float dissonance = 0;
    float tempDiss = 0;
    
    // Calculate the roughness between all fundamental partials
    for (int firstFundamental = 0;
         firstFundamental < distributions.size();
         ++firstFundamental)
    {
        // Prevent muted partials or distributions from inclusion in dissonance calculations
        if (! distributions[firstFundamental]->fundamentalIsMuted()
            && ! distributions[firstFundamental]->isMuted())
        {
            for (int secondFundamental = firstFundamental + 1;
                 secondFundamental < distributions.size();
                 ++secondFundamental)
            {
                if (! distributions[secondFundamental]->fundamentalIsMuted()
                    && ! distributions[secondFundamental]->isMuted())
                {
                    tempDiss = calculateRoughness (distributions[firstFundamental]->getFundamentalFreq(),
                                                   distributions[firstFundamental]->getFundamentalAmp(),
                                                   distributions[secondFundamental]->getFundamentalFreq(),
                                                   distributions[secondFundamental]->getFundamentalAmp());
                    
                    dissonance += tempDiss;
                    
                    if (sumPartialDissonances == TRUE)
                    {
                        distributions[firstFundamental]->addDissonanceToFundamental (tempDiss / 2);
                        distributions[secondFundamental]->addDissonanceToFundamental (tempDiss / 2);
                    }
                }
            }
        }
    }
    
    for (int firstDistribution = 0;
         firstDistribution < distributions.size();
         ++firstDistribution)
    {
        if (! distributions[firstDistribution]->isMuted())
        {
            for (int firstPartial = 0;
                 firstPartial < distributions[firstDistribution]->numPartials();
                 ++firstPartial)
            {
                if (! distributions[firstDistribution]->partialIsMuted (firstPartial))
                {
                    // Calculate the roughness between the current partial and all fundamental partials
                    for (int fundamentalPartial = 0;
                         fundamentalPartial < distributions.size();
                         ++fundamentalPartial)
                    {
                        if (! distributions[fundamentalPartial]->fundamentalIsMuted()
                            && ! distributions[fundamentalPartial]->isMuted())
                        {
                            tempDiss = calculateRoughness (distributions[firstDistribution]->getRealFreq (firstPartial),
                                                           distributions[firstDistribution]->getRealAmp (firstPartial),
                                                           distributions[fundamentalPartial]->getFundamentalFreq(),
                                                           distributions[fundamentalPartial]->getFundamentalAmp());
                            
                            dissonance += tempDiss;
                            
                            if (sumPartialDissonances == true)
                            {
                                distributions[firstDistribution]->addPartialDissonance (firstDistribution, tempDiss / 2);
                                distributions[fundamentalPartial]->addDissonanceToFundamental (tempDiss / 2);
                            }
                        }
                    }
                    
                    // Calculate the roughness between the current partial and all subsequent partials except for fundamentals
                    for (int secondDistribution = firstDistribution;
                         secondDistribution < distributions.size();
                         ++secondDistribution)
                    {
                        int startingPartial = 0;
                        
                        if (firstDistribution == secondDistribution)    // A lone partial creates no roughness
                            startingPartial = firstPartial + 1;
                        
                        for (int secondPartial = startingPartial;
                             secondPartial < distributions[secondDistribution]->numPartials();
                             ++secondPartial)
                        {
                            if (! distributions[secondDistribution]->isMuted()
                                && ! distributions[secondDistribution]->partialIsMuted (secondPartial))
                            {
                                tempDiss = calculateRoughness (distributions[firstDistribution]->getRealFreq (firstPartial),
                                                               distributions[firstDistribution]->getRealAmp (firstPartial),
                                                               distributions[secondDistribution]->getRealFreq (secondPartial),
                                                               distributions[secondDistribution]->getRealAmp (secondPartial));
                                
                                dissonance += tempDiss;
                                
                                if (sumPartialDissonances == true)
                                {
                                    distributions[firstDistribution]->addPartialDissonance (firstPartial, tempDiss / 2);
                                    distributions[secondDistribution]->addPartialDissonance (secondPartial, tempDiss / 2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return dissonance;
}



//==============================================================================
//                              SetharesModel
//==============================================================================

SetharesModel::SetharesModel() :    maxDiss (0.24),
                                    plcInterp1 (0.0207), plcInterp2 (18.96),
                                    plCurveRate1 (-3.51), plCurveRate2 (-5.75),
                                    plcFit1 (5), plcFit2 (-5)
{
}

SetharesModel::~SetharesModel()
{
}

String SetharesModel::getName()
{
    return "Sethares";
}

//==============================================================================


float SetharesModel::calculateRoughness (float firstFreq, float firstAmp,
                                         float secondFreq, float secondAmp)
{
    curveInterp = maxDiss / (plcInterp1 * jmin (firstFreq, secondFreq) + plcInterp2);
    freqDiff = std::abs (firstFreq - secondFreq);
    
    return jmin (firstAmp, secondAmp) * (plcFit1 * std::exp (plCurveRate1 * curveInterp * freqDiff)
                                         + plcFit2 * std::exp (plCurveRate2 * curveInterp * freqDiff));
}

std::unique_ptr<DissonanceModel> SetharesModel::cloneModel() const
{
    return std::make_unique<SetharesModel> (*this);
}


//==============================================================================
//                              VassilakisModel
//==============================================================================

VassilakisModel::VassilakisModel() :   maxDiss (0.24),
                                       plcInterp1 (0.0207), plcInterp2 (18.96),
                                       plCurveRate1 (-3.51), plCurveRate2 (-5.75),
                                       plcFit1 (5), plcFit2 (-5)
{
}

VassilakisModel::~VassilakisModel()
{
}

String VassilakisModel::getName()
{
    return "Vassilakis";
}

//==============================================================================


float VassilakisModel::calculateRoughness (float firstFreq, float firstAmp,
                                           float secondFreq, float secondAmp)
{
    curveInterp = maxDiss / (plcInterp1 * jmin (firstFreq, secondFreq) + plcInterp2);
    freqDiff = std::abs (firstFreq - secondFreq);
    
    x = std::powf (firstAmp * secondAmp, 0.1);
    y = 0.5 * std::powf (2 * jmin (firstAmp, secondAmp) / (firstAmp + secondAmp), 3.11);
    z = plcFit1 * std::exp (plCurveRate1 * curveInterp * freqDiff)
        + plcFit2 * std::exp (plCurveRate2 * curveInterp * freqDiff);                       // Remove plcFit1 & plcFit2???
    
    return x * y * z;
}

std::unique_ptr<DissonanceModel> VassilakisModel::cloneModel() const
{
    return std::make_unique<VassilakisModel> (*this);
}



