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

#include "FileIO.h"

void FileIO::setPath (const String& path)
{
    if (File::isAbsolutePath (StringRef (path)))
    {
        File temp (path);
        
        if (temp.getParentDirectory().isDirectory()
            || temp.hasFileExtension (StringRef ("dismal")))
            file = path;
    }
}

void FileIO::setPath (const File& newFile)
{
    if (newFile.getParentDirectory().isDirectory()
        || newFile.hasFileExtension (StringRef ("dismal")))
        file = newFile;
}

String FileIO::getPath()
{
    if (file.isAbsolutePath (file.getFullPathName()))
        return file.getFullPathName();
    
    return "";
}

const File& FileIO::getFileReference()
{
    return file;
}

const String FileIO::dataTypeOfFile()
{
    // Ensure that files are valid.
    jassert (file.existsAsFile() && file.hasFileExtension (StringRef ("dismal")));
    
    if (file.existsAsFile() && file.hasFileExtension (StringRef ("dismal")))
    {
        file.createInputStream()->readIntoMemoryBlock (memory);
        
        ValueTree temp;
        temp.readFromData (&memory, file.getSize());
        
        jassert (temp.isValid()); // For debugging
        
        if (temp.isValid() && ! tree.isEquivalentTo (temp))
            tree = temp.createCopy();
        
        return tree.getType().toString();
    }
    else
        return "N/A";
}

void FileIO::saveToFile (const OvertoneDistribution& distribution,
                         const bool overwrite)
{
    if (file.existsAsFile() && overwrite == false)
    {
        jassertfalse;    // Overwrite set to false, but the file path points to an existing file.
        return;
    }
    else if (distribution.numPartials() < 1)
    {
        jassertfalse;    // Distribution has no overtones
        return;
    }
    
    tree = ValueTree (IDs::OvertoneDistribution);
    
    for (int i = 0; i < distribution.numPartials(); ++i)
    {
        tree.addChild (ValueTree (IDs::Partial), -1, nullptr);
        
        tree.getChild (i).setProperty (IDs::Freq, distribution.getFreqRatio (i), nullptr);
        tree.getChild (i).setProperty (IDs::Amp, distribution.getAmpRatio (i), nullptr);
    }
    
    tree.setProperty (IDs::Name, distribution.getName(), nullptr);
    tree.setProperty (IDs::MinInterval, distribution.getMinInterval(), nullptr);
    
    memory.reset();
    MemoryOutputStream os (memory, false);
    tree.writeToStream (os);
    
    if (overwrite == true)
    {
        file.replaceWithData (&memory, memory.getSize());
    }
    else
    {
        if (file.create())
        {
            if (file.appendData (&memory, memory.getSize()))
                return;

            jassertfalse;    // Failed to write data
        }

        jassertfalse;        // Failed to create file
    }
}

void FileIO::saveToFile (const TuningSystem& tuning,
                         const bool overwrite)
{
    if (file.existsAsFile() && overwrite == false)
    {
        jassertfalse;    // Overwrite set to false, but the file path points to an existing file.
        return;
    }
    else if (tuning.numNotes() <= 1
             || tuning.getReferenceFrequency() != 0
             || tuning.getRepeatRatio() != 0)
    {
        jassertfalse;    // TuningSystem object is incomplete or invalid
        return;
    }
    
    tree = ValueTree (IDs::Tuning);
    
    Array<var> notes;
    
    for (int i = 0; i < tuning.numNotes(); ++i)
    {
        notes.addUsingDefaultSort (tuning.getFreqRatio (i));
    }
    
    tree.setProperty (IDs::Notes, var (notes), nullptr);
    tree.setProperty (IDs::Name, tuning.getName(), nullptr);
    tree.setProperty (IDs::MinInterval, tuning.getMinInterval(), nullptr);
    tree.setProperty (IDs::ReferenceFreq, tuning.getReferenceFrequency(), nullptr);
    tree.setProperty (IDs::RepeatRatio, tuning.getRepeatRatio(), nullptr);
    
    memory.reset();
    MemoryOutputStream os (memory, false);
    tree.writeToStream (os);
    
    if (overwrite == true)
    {
        file.replaceWithData (&memory, memory.getSize());
    }
    else
    {
        if (file.create())
        {
            if (file.appendData (&memory, memory.getSize()))
                return;
            else
                jassertfalse;        // Failed to write data
        }
        else
        {
            jassertfalse;            // Failed to create file
        }
    }
}

void FileIO::saveTreeToFile (const ValueTree &treeToSave,
                             const bool overwrite)
{
    if (file.existsAsFile() && ! overwrite)
    {
        jassertfalse;    // Overwrite set to false, but the file path points to an existing file.
        return;
    }
    
    if (treeToSave.isValid()
        && (treeToSave.hasType (IDs::OvertoneDistribution) || treeToSave.hasType (IDs::Tuning)))
    {
        file.deleteFile();
        file.create();
        
        FileOutputStream os = FileOutputStream (file);
        
        if (os.openedOk())
            treeToSave.writeToStream (os);
        
        os.flush();
    }
}

OvertoneDistribution FileIO::loadOvertonesFromFile()
{
    tree = ValueTree();
    
    file.loadFileAsData (memory);
    tree.readFromData (&memory, memory.getSize());
    
    OvertoneDistribution distribution;
    
    if (! tree.isValid())
    {
        jassertfalse;               // Failed to reload data from file into ValueTree object
        return distribution;        // Returns an empty OvertoneDistribution if tree is invalid. Soft fail.
    }
    
    distribution.setDistributionName (tree.getPropertyAsValue (IDs::Name, nullptr).toString());
    distribution.setMinInterval (tree.getPropertyAsValue (IDs::MinInterval, nullptr).getValue());
    
    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        distribution.addPartial (tree.getChild (i).getPropertyAsValue (IDs::Freq, nullptr).getValue(),
                                 tree.getChild (i).getPropertyAsValue (IDs::Amp, nullptr).getValue());
    }
    
    return distribution;
}

TuningSystem FileIO::loadTuningFromFile()
{
    tree = ValueTree();
    
    file.loadFileAsData (memory);
    tree.readFromData (&memory, memory.getSize());
    
    TuningSystem tuning;
    
    if (! tree.isValid())
    {
        jassertfalse;           // Failed to reload data from file into ValueTree object
        return tuning;          // Returns an empty TuningSystem if tree is invalid. Soft fail.
    }
    
    tuning.setName (tree.getPropertyAsValue (IDs::Name, nullptr).toString());
    tuning.setMinInterval (tree.getPropertyAsValue (IDs::MinInterval, nullptr).getValue());
    tuning.setRepeatRatio (tree.getPropertyAsValue (IDs::RepeatRatio, nullptr).getValue());
    tuning.setReferenceFrequency (tree.getPropertyAsValue (IDs::ReferenceFreq, nullptr).getValue());

    for (int i = 0; i < tree.getPropertyAsValue (IDs::Notes, nullptr).getValue().getArray()->size(); ++i)
    {
        tuning.addInterval (tree.getPropertyAsValue (IDs::Notes, nullptr).getValue()[i]);
    }
    
    return tuning;
}

ValueTree& FileIO::loadTreeFromFile()
{
    FileInputStream is = FileInputStream (file.getFullPathName());
    
    if (is.openedOk())
        tree = ValueTree::readFromStream (is);
    
    return tree;
}

