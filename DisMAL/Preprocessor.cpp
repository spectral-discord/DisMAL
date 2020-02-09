/*
  ==============================================================================
 
    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "Preprocessor.h"

HearingRangePreprocessor::HearingRangePreprocessor()
{
    hearingRange.setStart (20);
    hearingRange.setEnd (20000);
    name = "Hearing Range";
    description = "Applies a bandpass filter to remove frequencies that lie outside the human hearing range.";
}

HearingRangePreprocessor::~HearingRangePreprocessor()
{
}

void HearingRangePreprocessor::setHearingRange (float lowerLimit, float upperLimit)
{
    hearingRange.withStartAndLength (lowerLimit, upperLimit - lowerLimit);
}

Range<float> HearingRangePreprocessor::getHearingRange()
{
    return hearingRange;
}

void HearingRangePreprocessor::process (OwnedArray<OvertoneDistribution>& distributions)
{
    for (auto* dist : distributions)
    {
        if (! hearingRange.contains (dist->getFundamentalFreq()))
            dist->muteFundamental (true);
            
        for (int p = 0; p < dist->numPartials(); ++p)
        {
            if (! hearingRange.contains (dist->getRealFreq (p)))
                dist->mutePartial (p, true);
        }
    }
}

std::unique_ptr<Preprocessor> HearingRangePreprocessor::clone() const
{
    return std::make_unique<HearingRangePreprocessor> (*this);
}
