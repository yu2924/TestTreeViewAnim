//
//  Main.cpp
//  TestTreeViewAnim_App
//
//  created by yu2924 on 2023-09-09
//

#include <JuceHeader.h>
#include "AnimTreeView.h"

class MainComponent : public juce::Component
{
private:
	juce::TextButton addButton;
	juce::TextButton delButton;
	juce::TextButton moveDownButton;
	juce::TextButton moveUpButton;
	AnimTreeView treeview;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
public:
	MainComponent()
	{
		addAndMakeVisible(addButton);
		addButton.setButtonText("+");
		addButton.setTooltip("add a sub node to the selected node");
		addButton.onClick = [this]() { treeview.addSubNodeToSelection(); };
		addAndMakeVisible(delButton);
		delButton.setButtonText("x");
		delButton.setTooltip("delete the selected node");
		delButton.onClick = [this]() { treeview.deleteSelectedNode(); };
		addAndMakeVisible(moveDownButton);
		moveDownButton.setButtonText("_");
		moveDownButton.setTooltip("move down the selected node");
		moveDownButton.onClick = [this]() {treeview.moveSelectedItem(false); };
		addAndMakeVisible(moveUpButton);
		moveUpButton.setButtonText("^");
		moveUpButton.setTooltip("move up the selected node");
		moveUpButton.onClick = [this]() {treeview.moveSelectedItem(true); };
		addAndMakeVisible(treeview);
		setSize(320 - 2, 320 - 32);
	}
	virtual ~MainComponent() override
	{
	}
	virtual void resized() override
	{
		enum { Margin = 8, Spacing = 8, ButtonHeight = 24, ButtonWidth = 32 };
		juce::Rectangle<int> rc = getLocalBounds().reduced(Margin);
		juce::Rectangle<int> rcb = rc.removeFromTop(ButtonHeight);
		addButton.setBounds(rcb.removeFromLeft(ButtonWidth));
		delButton.setBounds(rcb.removeFromLeft(ButtonWidth));
		rcb.removeFromLeft(Spacing);
		moveDownButton.setBounds(rcb.removeFromLeft(ButtonWidth));
		moveUpButton.setBounds(rcb.removeFromLeft(ButtonWidth));
		rc.removeFromTop(Spacing);
		treeview.setBounds(rc);
	}
	virtual void paint(juce::Graphics& g) override
	{
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	}
};

class MainWindow : public juce::DocumentWindow
{
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
public:
	MainWindow(juce::String name) : DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
	{
		setUsingNativeTitleBar(true);
		setContentOwned(new MainComponent(), true);
#if JUCE_IOS || JUCE_ANDROID
		setFullScreen(true);
#else
		setResizable(true, true);
		centreWithSize(getWidth(), getHeight());
#endif
		setVisible(true);
	}
	virtual void closeButtonPressed() override
	{
		juce::JUCEApplication::getInstance()->systemRequestedQuit();
	}
};

class TestTVAnimApplication : public juce::JUCEApplication
{
private:
	std::unique_ptr<MainWindow> mainWindow;
public:
	TestTVAnimApplication() {}
	virtual const juce::String getApplicationName() override { return ProjectInfo::projectName; }
	virtual const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
	virtual bool moreThanOneInstanceAllowed() override { return true; }
	virtual void initialise(const juce::String&) override
	{
		mainWindow.reset(new MainWindow(getApplicationName()));
	}
	virtual void shutdown() override
	{
		mainWindow = nullptr;
	}
	virtual void systemRequestedQuit() override
	{
		quit();
	}
	virtual void anotherInstanceStarted(const juce::String&) override
	{
	}
};

START_JUCE_APPLICATION(TestTVAnimApplication)
