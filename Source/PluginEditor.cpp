#include "PluginEditor.h"

JRemixAudioProcessorEditor::JRemixAudioProcessorEditor (JRemixAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    setSize (720, 440);

    titleLabel.setText ("JREMIX AUTO-MIXER (VST3)", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (20.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00e5ff));
    addAndMakeVisible (titleLabel);

    dropZoneLabel.setText (audioProcessor.currentFolderName, juce::dontSendNotification);
    dropZoneLabel.setJustificationType (juce::Justification::centred);
    dropZoneLabel.setColour (juce::Label::backgroundColourId, juce::Colour (0xff1e242b));
    dropZoneLabel.setColour (juce::Label::outlineColourId, juce::Colour (0xff00e5ff));
    addAndMakeVisible (dropZoneLabel);

    minutesLabel.setText ("DURACION OBJETIVO (MINUTOS):", juce::dontSendNotification);
    addAndMakeVisible (minutesLabel);

    minutesSlider.setRange (5, 240, 1);
    minutesSlider.setValue (audioProcessor.targetMinutes);
    minutesSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 26);
    minutesSlider.onValueChange = [this] {
        audioProcessor.targetMinutes = (int) minutesSlider.getValue();
        rebuildPlaylist (false);
    };
    addAndMakeVisible (minutesSlider);

    randomButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2b3540));
    randomButton.onClick = [this] { rebuildPlaylist (true); };
    addAndMakeVisible (randomButton);

    generateButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff00b894));
    generateButton.onClick = [this] {
        if (audioProcessor.currentPlaylist.empty()) {
            statusLabel.setText ("Primero arrastra una carpeta con musica.", juce::dontSendNotification);
            return;
        }
        if (BridgeExporter::sendToAbleton (audioProcessor.currentPlaylist)) {
            statusLabel.setText ("Enviado a Ableton Live! Revisa tus pistas en Arrangement.", juce::dontSendNotification);
        } else {
            statusLabel.setText ("Error al escribir orden para Ableton.", juce::dontSendNotification);
        }
    };
    addAndMakeVisible (generateButton);

    exportButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff3742fa));
    exportButton.onClick = [this] {
        if (audioProcessor.currentPlaylist.empty()) return;
        juce::File out = juce::File::getSpecialLocation (juce::File::userDesktopDirectory)
                             .getChildFile ("JREMIX_Tracklist.txt");
        if (BridgeExporter::exportTracklistTxt (audioProcessor.currentPlaylist, out)) {
            statusLabel.setText ("Tracklist guardado en el Escritorio (JREMIX_Tracklist.txt).", juce::dontSendNotification);
        }
    };
    addAndMakeVisible (exportButton);

    tracklistDisplay.setMultiLine (true);
    tracklistDisplay.setReadOnly (true);
    tracklistDisplay.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff12161a));
    tracklistDisplay.setColour (juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible (tracklistDisplay);

    statusLabel.setText ("Listo. Arrastra una carpeta indexada por Ableton (.asd).", juce::dontSendNotification);
    statusLabel.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible (statusLabel);
}

void JRemixAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (juce::Colour (0xff161a1f));
}

void JRemixAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced (15);
    titleLabel.setBounds (area.removeFromTop (30));
    area.removeFromTop (10);

    auto leftCol = area.removeFromLeft (280);
    dropZoneLabel.setBounds (leftCol.removeFromTop (110));
    leftCol.removeFromTop (15);

    minutesLabel.setBounds (leftCol.removeFromTop (22));
    minutesSlider.setBounds (leftCol.removeFromTop (30));
    leftCol.removeFromTop (15);

    randomButton.setBounds (leftCol.removeFromTop (38));
    leftCol.removeFromTop (12);
    generateButton.setBounds (leftCol.removeFromTop (44));
    leftCol.removeFromTop (12);
    exportButton.setBounds (leftCol.removeFromTop (36));

    area.removeFromLeft (15);
    statusLabel.setBounds (area.removeFromBottom (25));
    area.removeFromBottom (8);
    tracklistDisplay.setBounds (area);
}

bool JRemixAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray& files) {
    for (const auto& f : files)
        if (juce::File (f).isDirectory()) return true;
    return false;
}

void JRemixAudioProcessorEditor::filesDropped (const juce::StringArray& files, int, int) {
    for (const auto& f : files) {
        juce::File folder (f);
        if (folder.isDirectory()) {
            audioProcessor.currentFolderName = "Carpeta: " + folder.getFileName();
            dropZoneLabel.setText (audioProcessor.currentFolderName, juce::dontSendNotification);
            audioProcessor.loadedPool = TrackScanner::scanFolder (folder);
            rebuildPlaylist (true);
            break;
        }
    }
}

void JRemixAudioProcessorEditor::rebuildPlaylist (bool shuffle) {
    audioProcessor.currentPlaylist = audioProcessor.mixEngine.buildMix (
        audioProcessor.loadedPool, audioProcessor.targetMinutes, shuffle
    );

    juce::String text;
    for (size_t i = 0; i < audioProcessor.currentPlaylist.size(); ++i) {
        const auto& item = audioProcessor.currentPlaylist[i];
        int s = (int) item.startTimeSeconds;
      text += juce::String::formatted ("[%02d:%02d] %02d. ", s / 60, s % 60, (int) i + 1)
              + item.track.title
          + juce::String::formatted (" (%.0f BPM)\n", item.track.bpm);
    }
    tracklistDisplay.setText (text);
    statusLabel.setText (juce::String::formatted ("%d tracks seleccionados | Pool total: %d canciones",
                                                (int) audioProcessor.currentPlaylist.size(),
                                                (int) audioProcessor.loadedPool.size()),
                         juce::dontSendNotification);
}
