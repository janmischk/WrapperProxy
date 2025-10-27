#include "ToastWindow.h"

// Constructor
ToastWindow::ToastWindow(const juce::String& message, juce::Component* parent)
    : parentComponent(parent)
{
    setOpaque(false);

    // Label
    label = std::make_unique<juce::Label>();
    label->setText(message, juce::dontSendNotification);
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(label.get());

    // Close button
    closeButton = std::make_unique<juce::TextButton>("X");
    closeButton->onClick = [this] { closeButtonClicked(); };
    addAndMakeVisible(closeButton.get());

    // Initial size
    setSize(300, 80);

    // Center on parent or screen
    if (parentComponent != nullptr)
        setCentrePosition(parentComponent->getWidth() / 2, parentComponent->getHeight() / 2);
    else
    {
        auto area = juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
        setCentrePosition(area.getCentreX(), area.getCentreY());
        addToDesktop(juce::ComponentPeer::windowHasDropShadow
            | juce::ComponentPeer::windowIsTemporary);
        setAlwaysOnTop(true);
    }
}

ToastWindow::~ToastWindow()
{
    label = nullptr;
    closeButton = nullptr;
}

void ToastWindow::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    closeButton->setBounds(bounds.removeFromTop(20).removeFromRight(20));
    label->setBounds(bounds);
}

void ToastWindow::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black.withAlpha(0.85f));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);
}

void ToastWindow::closeButtonClicked()
{
    // If we are a child of some parent, remove from parent
    if (getParentComponent() != nullptr)
        getParentComponent()->removeChildComponent(this);

    // If top-level, remove from desktop
    if (isOnDesktop())
        removeFromDesktop();

    delete this;
}

// Static show function
void ToastWindow::show(const juce::String& message, juce::Component* parent)
{
    auto* toast = new ToastWindow(message, parent);
    if (parent != nullptr)
        parent->addAndMakeVisible(toast);
    else
        toast->setVisible(true);
}
