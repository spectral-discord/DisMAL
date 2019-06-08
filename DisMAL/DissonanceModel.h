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
#include "OvertoneDistribution.h"

/** Base class for implementing dissonance models. */
class DissonanceModel
{
public:
    //==============================================================================
    /** Creates a dissonance model object. */
    DissonanceModel();
    
    /** Desctructor. */
    virtual ~DissonanceModel();
    
    //==============================================================================
    /** Returns the name of the dissonance model. */
    virtual String getName() = 0;
    
    //==============================================================================
    /** Calculates the dissonance of a set of overtone distributions with corresponding frequency and amplitude values. */
    virtual float calculateDissonance (const OwnedArray<OvertoneDistribution>& distributions,
                                       bool sumPartialDissonances) = 0;
    
    /** Enables dynamic allocation of child objects via std::unique_ptr. */
    virtual std::unique_ptr<DissonanceModel> cloneModel() const = 0;
    
protected:
    //==============================================================================

};

//==================================================================================

/** Base class for implementing dissonance models based on summed interference between spectral partials.
 
    These dissonance models sum the roughness between all partials according to the models proposed by Plomp & Levelt, Sethares, and others.
*/
class SpectralInterferenceModel :   public DissonanceModel
{
public:
    /** Creates a SpectralInterferenceModel object. */
    SpectralInterferenceModel();
    
    /** Destructor. */
    virtual ~SpectralInterferenceModel();
    
    //==============================================================================
    /** Calculates the dissonance of a set of overtone distributions with corresponding frequency and amplitude values.
     
        This method makes repeated calls to calculateRoughness for every possible pair of partials (including fundamentals) in a set of overtone distributions. The outputs of the calls to calculateRoughness are summed to satisfy:
    
        \f$$D = \sum_{i=1}^{n}\sum_{j=1}^{n} d(f_i, f_j, a_i, a_j)$\f$
    */
    float calculateDissonance (const OwnedArray<OvertoneDistribution>& distributions,
                               bool sumPartialDissonances) override;
    
    //==============================================================================
    /** Calculates the roughness between two partials.
     
        @see calculateDissonance
    */
    virtual float calculateRoughness (float firstFreq, float firstAmp,
                                      float secondFreq, float secondAmp) = 0;
};

//==================================================================================

/** Implementation of Sethares' model from "Tuning, Timbre, Spectrum, Scale" (2005).
 
    \f$$d(f_1,f_2,a_1,a_2) = min(a_1,a_2)[e^{-b_1s(f_2-f_1)}+e^{-b_2s(f_2-f_1)}]$\f$
 
    where
 
    \f$$s = \frac{x}{s_1f_1 + s_2}$\f$
 
    for
 
    \f$$f_1 < f_2$\f$
*/
class SetharesModel :   public SpectralInterferenceModel
{
public:
    /** Creates a SetharesModel object. */
    SetharesModel();
    
    /** Destructor. */
    ~SetharesModel();
    
    /** Returns the name of the dissonance model. */
    String getName() override;
    
    /** Calculates the roughness between two partials. */
    float calculateRoughness (float firstFreq, float firstAmp,
                              float secondFreq, float secondAmp) override;
    
    /** For dynamic allocation via std::unique_ptr in DissonanceCalc. */
    std::unique_ptr<DissonanceModel> cloneModel() const override;
    
protected:
    /** This is the point of maximum dissonance. The value is derived from a model of the Plom Levelt dissonance curves for all frequencies. Denoted by \f$$x$\f$. */
    const float maxDiss;
    /** These values are used to allow a single functional form to interpolate beween the various P&L curves of different frequencies by sliding, stretching/compressing the curve so that its max dissonance occurse at dstar. A least-square-fit was made to determine the values. Denoted by \f$$s_1$\f$. */
    const float plcInterp1;
    /** These values are used to allow a single functional form to interpolate beween the various P&L curves of different frequencies by sliding, stretching/compressing the curve so that its max dissonance occurse at dstar. A least-square-fit was made to determine the values. Denoted by \f$$s_2$\f$. */
    const float plcInterp2;
    /** Theses values determine the rates at which the function rises and falls and are based on a gradient minimisation of the squared error between Plomp and Levelt's averaged data and the curve. Denoted by \f$$b_1$\f$. */
    const float plCurveRate1;
    /** Theses values determine the rates at which the function rises and falls and are based on a gradient minimisation of the squared error between Plomp and Levelt's averaged data and the curve. Denoted by \f$$b_2$\f$. */
    const float plCurveRate2;
    const float plcFit1;        /**< These parameters have values to fit the experimental data of Plomp and Levelt. */
    const float plcFit2;        /**< These parameters have values to fit the experimental data of Plomp and Levelt. */
    float curveInterp;          /**< This stores the result of \f$s = \frac{x}{s_1f_1 + s_2}\f$. */
    float freqDiff;             /**< This stores the difference in frequency between the partials. */
};

//==================================================================================

/** Implementation of Vassilakis' model from "Perceptual and Physical Properties of Amplitude Fluctuation and their Musical Significance" (2001).
 
    \f$$d(f_1,f_2,a_1,a_2) = (a_1a_2)^{0.1}(\frac{2*min(a_1,a_2)}{a_1+a_2})^{3.11}(e^{-b_1s(f_2-f_1)}+e^{-b_2s(f_2-f_1)})$\f$
 
    where
 
    \f$$s = \frac{x}{s_1f_1 + s_2}$\f$
 
    for
 
    \f$$f_1 < f_2$\f$
*/
class VassilakisModel :   public SpectralInterferenceModel
{
public:
    /** Creates a VassilakisModel object. */
    VassilakisModel();
    
    /** Detructor. */
    ~VassilakisModel();
    
    /** Returns the name of the dissonance model. */
    String getName() override;
    
    /** Calculates the roughness between two partials. */
    float calculateRoughness (float firstFreq, float firstAmp,
                              float secondFreq, float secondAmp) override;
    
    /** For dynamic allocation via std::unique_ptr in DissonanceCalc. */
    std::unique_ptr<DissonanceModel> cloneModel() const override;
    
protected:
    /** This is the point of maximum dissonance. The value is derived from a model of the Plom Levelt dissonance curves for all frequencies. Denoted by \f$$x$\f$. */
    const float maxDiss;
    /** These values are used to allow a single functional form to interpolate beween the various P&L curves of different frequencies by sliding, stretching/compressing the curve so that its max dissonance occurse at dstar. A least-square-fit was made to determine the values. Denoted by \f$$s_1$\f$. */
    const float plcInterp1;
    /** These values are used to allow a single functional form to interpolate beween the various P&L curves of different frequencies by sliding, stretching/compressing the curve so that its max dissonance occurse at dstar. A least-square-fit was made to determine the values. Denoted by \f$$s_2$\f$. */
    const float plcInterp2;
    /** Theses values determine the rates at which the function rises and falls and are based on a gradient minimisation of the squared error between Plomp and Levelt's averaged data and the curve. Denoted by \f$$b_1$\f$. */
    const float plCurveRate1;
    /** Theses values determine the rates at which the function rises and falls and are based on a gradient minimisation of the squared error between Plomp and Levelt's averaged data and the curve. Denoted by \f$$b_2$\f$. */
    const float plCurveRate2;
    const float plcFit1;        /**< These parameters have values to fit the experimental data of Plomp and Levelt. */
    const float plcFit2;        /**< These parameters have values to fit the experimental data of Plomp and Levelt. */
    float curveInterp;          /**< This stores the result of \f$s = \frac{x}{s_1f_1 + s_2}\f$. */
    float freqDiff;             /**< This stores the difference in frequency between the partials. */
    float x, y, z;
    
    
};
