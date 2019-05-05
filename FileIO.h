/*
  ==============================================================================

    This file is part of DisMAL (Dissonance Modeling and Analysis Library)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    DisMAL is provided under the terms of the MIT License
    https://opensource.org/licenses/MIT
 
    The code in this file makes calls to JUCE library code licensed under GPL v3.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "OvertoneDistribution.h"
#include "TuningSystem.h"
#include "../IDs.h"

/** A class for reading from and writing to '.dismal' files.
 
    This class uses a juce::ValueTree, juce::File, and juce::MemoryBlock to serialize and deserialize OvertoneDistribution and TuningSystem objects into raw binary data. The files produced using this class should have an file extension of '.dismal', and can contain both overtone distributions and tuning systems. This is to enable the transfer of overtone distributions and tunings between users of DisMAL or apps derived from DisMAL.
*/
class FileIO
{
public:
    /** Default constructor. */
    FileIO(){}
    
    /** Constructs a FileIO object and sets its filepath using a String. */
    FileIO (String pathName) {setPath(pathName);}
    
    /** Constructs a FileIO object and sets its filepath using a File object. */
    FileIO (File filePath) {setPath(filePath);}
    
    /** Destructor. */
    ~FileIO(){}
    
    /** Sets the path of a file to read or write, using a String input. */
    void setPath (const String& path);
    
    /** Sets the path of a file to read or write from a File object. */
    void setPath (const File& newFile);

    /** Returns a String object that identifies the data in a file as that of either an OvertoneDistribution or a TuningSystem. */
    const String dataTypeOfFile();

    /** Saves an overtone distribution to a file.
     
        @param distribution A reference to the OvertoneDistribution object to be saved to file.
        @param overwrite Set to true to overwrite an existing file. The default value is set to false to prevent accidental overwrites.
    */
    void saveToFile (const OvertoneDistribution& distribution,
                     const bool overwrite = false);
    
    /** Saves a tuning system to a file.
     
        @param tuning A reference to the TuningSystem object to be saved to file.
        @param overwrite Set to true to overwrite an existing file. The default value is set to false to prevent accidental overwrites.
    */
    void saveToFile (const TuningSystem& tuning,
                     const bool overwrite = false);
    
    /** Saves a ValueTree containing overtone distribution or tuning system data to a file.
     
        @param treeToSave A reference to the ValueTree object to be saved to file. The tree must be typed using an "OvertoneDistribution" or "Tuning" identifier.
        @param overwrite Set to true to overwrite an existing file. The default value is set to false to prevent accidental overwrites.
    */
    void saveTreeToFile (const ValueTree& treeToSave,
                         const bool overwrite = false);
    
    /** Returns an OvertoneDistribution object contained in a file. */
    OvertoneDistribution loadOvertonesFromFile();
    
    /** Returns a TuningSystem object contained in a file. */
    TuningSystem loadTuningFromFile();
    
    /** Returns a juce::ValueTree of the OvertoneDistribution or TuningSystem data contained in a file. */
    ValueTree& loadTreeFromFile();
    
private:
    String fileName;
    File file;
    ValueTree tree;
    MemoryBlock memory;
};
