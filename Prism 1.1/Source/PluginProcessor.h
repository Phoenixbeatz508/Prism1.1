/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../prism_dsp/include/juce_dsp/juce_dsp.h"

//==============================================================================
/**
*/
class Prism1_1AudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Prism1_1AudioProcessor();
    ~Prism1_1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    enum class DSP_Options
    {
        shine
        Glitch,
        Width,
        Digital,
        Space,
        Emotion,
        End_OF_LIST
    };

	juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters Tree", createParameterLayout() };

	name getDSPName(DSP_Options option);
	layout.add(std::make_unique<juce::AudioParameterFloat>("shine", "Shine", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("glitch", "Glitch", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("digital", "Digital", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("space", "Space", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("emotion", "Emotion", 0.0f, 1.0f, 0.5f));


        using DSP_order = std::array<DSP_Options, static_cast<size_t>(DSP_Options::End_OF_LIST)>;
		prism1.1AudioProcessor::DSP_order dsp_order;

		juce::AudioParamenterfloat* getParameterForDSP(DSP_Options option)
		{
			switch (option)
			{
			case DSP_Options::shine:
				return apvts.getRawParameterValue("shine");
			case DSP_Options::Glitch:
				return apvts.getRawParameterValue("glitch");
			case DSP_Options::Width:
				return apvts.getRawParameterValue("width");
			case DSP_Options::Digital:
				return apvts.getRawParameterValue("digital");
			case DSP_Options::Space:
				return apvts.getRawParameterValue("space");
			case DSP_Options::Emotion:
				return apvts.getRawParameterValue("emotion");
			default:
				return nullptr;
			}
		}
private:
	DSP_order dsp_order;

	template<typename DSP>
	struct DSP_choice : : public juce::dsp::ProcessorBase
	{
		DSP_choice() = default;
		void prepare(const juce::dsp::ProcessSpec& spec) override
		{
			if (currentChoice.has_value())
				std::visit([&](auto&& dsp) { dsp.prepare(spec); }, currentChoice.value());
		}
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override
		{
			if (currentChoice.has_value())
				std::visit([&](auto&& dsp) { dsp.process(context); }, currentChoice.value());
		}
		void reset() override
		{
			if (currentChoice.has_value())
				std::visit([&](auto&& dsp) { dsp.reset(); }, currentChoice.value());
		}
		std::optional<std::variant<juce::dsp::Shine<float>, juce::dsp::Glitch<float>, juce::dsp::Width<float>, juce::dsp::Digital<float>, juce::dsp::Space<float>, juce::dsp::Emotion<float>>> currentChoice;
	};

	juce::dsp::ProcessorChain<juce::dsp::Shine<float>, juce::dsp::Glitch<float>, juce::dsp::Width<float>, juce::dsp::Digital<float>, juce::dsp::Space<float>, juce::dsp::Emotion<float>> processorChain;

	using DSP_pointers = std::array<std::optional<std::variant<juce::dsp::Shine<float>, juce::dsp::Glitch<float>, juce::dsp::Width<float>, juce::dsp::Digital<float>, juce::dsp::Space<float>, juce::dsp::Emotion<float>>>, static_cast<size_t>(DSP_Options::End_OF_LIST)>;
	prism1.1AudioProcessor::DSP_pointers dsp_pointers;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Prism1_1AudioProcessor)
};
