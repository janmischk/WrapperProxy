#pragma once
#include <JuceHeader.h>

class ToastWindow : public juce::Component
{
public:
    ToastWindow(const juce::String& message, juce::Component* parent = nullptr);

    ~ToastWindow() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    /** Convenience function to show a centered toast with close button */
    static void show(const juce::String& message, juce::Component* parent = nullptr);

private:
    void closeButtonClicked();

    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::TextButton> closeButton;

    juce::Component* parentComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToastWindow)
};