/*
  ==============================================================================

    SynthVoice.h
    Created: 29 Oct 2017 10:18:29am
    Author:  Joshua Hodge

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ThinkSynthSound.h"
#include "Maximilian/maximilian.h"


class ThinkSynthVoice : public SynthesiserVoice
{
public:
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast <ThinkSynthSound*>(sound) != nullptr;
    }

    void setOscillatorType(float *selection)
    {
        oscillatorType = *selection;
    }

    double getOscillator()
    {
        switch (oscillatorType){
            case 0:
                return oscillator.sinewave(frequency);
            case 1:
                return oscillator.saw(frequency);
            case 2:
                return oscillator.square(frequency);
            case 3:
                return oscillator.noise();
            case 4:
                return oscillator.sawn(frequency);
            default:
                return oscillator.sinewave(frequency);
        }
    }

    void setEnvelopeParams(float *attack, float *decay, float *sustain, float *release)
    {
        envelope.setAttack(*attack);
        envelope.setDecay(*decay);
        envelope.setSustain(*sustain);
        envelope.setRelease(*release);
    }

    double getEnvelope()
    {
        return envelope.adsr(getOscillator(), envelope.trigger);
    }

    void setFilterParams(float *filterType, float *filterCutoff, float *filterRes)
    {
        filterChoice = *filterType;
        cutoff = *filterCutoff;
        resonance = *filterRes;
    }

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override
    {
        envelope.trigger = 1;
        frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        level = velocity;
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        envelope.trigger = 0;
        allowTailOff = true;
        
        if (velocity == 0)
            clearCurrentNote();
    }

    void pitchWheelMoved(int newPitchWheelValue) override{}

    void controllerMoved(int controllerNumber, int newControllerValue) override{}

    void renderNextBlock(AudioBuffer <float> &outputBuffer, int startSample, int numSamples) override
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                outputBuffer.addSample(channel, startSample, getEnvelope());
            }
            ++startSample;
        }
    }

    void setOscilattorParametersToTree(AudioProcessorValueTreeState &tree){
        NormalisableRange<float> wavetypeParam (0, 2);
        tree.createAndAddParameter("wavetype", "WaveType", "wavetype", wavetypeParam, 0, nullptr, nullptr);
    }

    void setEnvelopeParametersToTree(AudioProcessorValueTreeState &tree){
        //need these normalisable range objects for the tree state below this
        NormalisableRange<float> attackParam (0.1f, 5000.0f);
        NormalisableRange<float> decayParam (1.0f, 2000.0f);
        NormalisableRange<float> sustainParam (0.0f, 1.0f);
        NormalisableRange<float> releaseParam (0.1f, 5000.0f);

        //params that make it possible to set/get states and automate parameters in your DAW.  Also connects values between the slider and the values here
        tree.createAndAddParameter("attack", "Attack", "attack", attackParam, 0.1f, nullptr, nullptr);
        tree.createAndAddParameter("decay", "Decay", "decay", decayParam, 1.0f, nullptr, nullptr);
        tree.createAndAddParameter("sustain", "Sustain", "sustain", sustainParam, 0.8f, nullptr, nullptr);
        tree.createAndAddParameter("release", "Release", "release", releaseParam, 0.1f, nullptr, nullptr);
    }

private:
    maxiOsc oscillator;
    int oscillatorType;
    maxiEnv envelope;

    double level;
    double frequency;

    int filterChoice;
    float cutoff;
    float resonance;
};
