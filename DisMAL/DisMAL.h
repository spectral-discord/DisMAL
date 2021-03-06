/*
  ==============================================================================

    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0

  ==============================================================================
*/

#pragma once

#include "DissonanceCalc.h"
#include "DissonanceModel.h"
#include "OvertoneDistribution.h"
#include "TuningSystem.h"
#include "Preprocessor.h"
#include "FileIO.h"

namespace DisMAL {
    const OwnedArray<Preprocessor> Preprocessors (std::initializer_list<Preprocessor*> {new HearingRangePreprocessor()});
    const OwnedArray<DissonanceModel> DissonanceModels (std::initializer_list<DissonanceModel*> {new SetharesModel(), new VassilakisModel()});
}
