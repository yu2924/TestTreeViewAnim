//
//  AnimTreeView.h
//  TestTreeViewAnim_App
//
//  created by yu2924 on 2023-09-09
//

#pragma once

#include <JuceHeader.h>

class AnimTreeViewItem : public juce::TreeViewItem
{
public:
	static juce::String dispenseIncrementalName()
	{
		static int seed = 0;
		return juce::String::formatted("%03d", seed++);
	}
	class PaintLayer : public juce::Component
	{
	public:
		AnimTreeViewItem* tvitem;
		PaintLayer(AnimTreeViewItem* p) : tvitem(p)
		{
			setInterceptsMouseClicks(false, false);
		}
		virtual void paint(juce::Graphics& g) override
		{
			bool isselected = tvitem->isSelected();
			juce::Colour clrbg = tvitem->getOwnerView()->findColour(isselected ? juce::PopupMenu::ColourIds::highlightedBackgroundColourId : juce::ResizableWindow::ColourIds::backgroundColourId);
			juce::Colour clrtext = tvitem->getOwnerView()->findColour(isselected ? juce::PopupMenu::ColourIds::highlightedTextColourId : juce::PopupMenu::ColourIds::textColourId);
			juce::Colour clricon(0xff00c0c0);
			juce::Rectangle<int> rc = getLocalBounds();
			// background
			g.setColour(clrbg);
			g.fillRoundedRectangle(rc.toFloat(), 2);
			rc.reduce(1, 1);
			// icon
			g.setColour(clricon);
			int cxyi = rc.getHeight();
			juce::Rectangle<int> rci = rc.removeFromLeft(cxyi).reduced(1);
			g.fillRoundedRectangle(rci.toFloat(), 3);
			rc.removeFromLeft(2);
			// text
			g.setColour(clrtext);
			g.drawText(tvitem->text, rc, juce::Justification::left, true);
		}
	} paintLayer;
	juce::String text = dispenseIncrementalName();
	AnimTreeViewItem() : paintLayer(this) {}
	void triggerRepositionPaintLayer()
	{
		// assumes that the owner TreeView will react as juce::AsyncUpdater to invoke repositioning
		if(juce::AsyncUpdater* au = dynamic_cast<juce::AsyncUpdater*>(getOwnerView()))
		{
			au->triggerAsyncUpdate();
		}
	}
	virtual bool mightContainSubItems() override { return 0 < getNumSubItems(); }
	virtual void itemClicked(const juce::MouseEvent&) override
	{
		setSelected(true, true);
	}
	virtual void paintItem(juce::Graphics&, int, int) override
	{
		// do nothing here
	}
	virtual void itemOpennessChanged(bool) override
	{
		triggerRepositionPaintLayer();
	}
	// --------------------------------------------------------------------------------
	// the experiment
protected:
	static juce::TreeViewItem* findHighestClosedAncestorOrItself(TreeViewItem* tvi)
	{
		std::vector<juce::TreeViewItem*> hytvi;
		for(juce::TreeViewItem* tvit = tvi; tvit; tvit = tvit->getParentItem())
		{
			hytvi.push_back(tvit);
		}
		for(std::vector<juce::TreeViewItem*>::const_reverse_iterator i = hytvi.rbegin(); i != hytvi.rend(); ++i)
		{
			juce::TreeViewItem* tvit = *i;
			if((tvit == tvi) || !tvit->isOpen()) return tvit;
		}
		jassertfalse;
		return nullptr;
	}
	static struct { juce::Rectangle<int> rc; bool visible; } getItemRectAndVisibility(TreeViewItem* tvi)
	{
		juce::TreeView* tv = tvi->getOwnerView();
		if(!tv) return {};
		juce::TreeViewItem* tvip = findHighestClosedAncestorOrItself(tvi);
		juce::Rectangle<int> rc = tvip->getItemPosition(false).withHeight(tvi->getItemHeight());
		bool visible = tvip == tvi;
		return { rc, visible };
	}
	enum { fail = 0, alreadyattached, attachednew } ensurePaintLayer()
	{
		juce::TreeView* tv = getOwnerView();
		if(!tv) return fail;
		juce::Component* scrpane = tv->getViewport()->getViewedComponent();
		if(!scrpane) return fail;
		auto r = (scrpane != paintLayer.getParentComponent()) ? attachednew : alreadyattached;
		scrpane->addAndMakeVisible(paintLayer);
		return r;
	}
public:
	void repositionPaintLayer(bool anim)
	{
		auto r = ensurePaintLayer();
		if(r == fail) return;
		else if(r == attachednew)
		{
			juce::TreeViewItem* tvip = getParentItem(); if(!tvip) tvip = this;
			paintLayer.setBounds(tvip->getItemPosition(false).withHeight(getItemHeight()));
		}
		auto rv = getItemRectAndVisibility(this);
		if(anim)
		{
			juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
			animator.cancelAnimation(&paintLayer, false);
			animator.animateComponent(&paintLayer, rv.rc, rv.visible ? 1.0f : 0.0f, 200, false, 1, 1);
		}
		else
		{
			paintLayer.setBounds(rv.rc);
			paintLayer.setAlpha(rv.visible ? 1.0f : 0.0f);
		}
	}
};

class AnimTreeView
	: public juce::TreeView
	, public juce::ComponentListener
	, public juce::AsyncUpdater
{
public:
	std::unique_ptr<AnimTreeViewItem> rootItem;
	AnimTreeView()
	{
		setMultiSelectEnabled(false);
		rootItem = std::make_unique<AnimTreeViewItem>();
		setRootItem(rootItem.get());
		rootItem->setSelected(true, true);
		getViewport()->getViewedComponent()->addComponentListener(this);
	}
	virtual ~AnimTreeView()
	{
		setRootItem(nullptr);
	}
	virtual void componentMovedOrResized(Component&, bool, bool) override
	{
		triggerAsyncUpdate();
	}
	virtual void handleAsyncUpdate() override
	{
		repositionPaintLayers(rootItem.get(), true);
	}
	void repositionPaintLayers(AnimTreeViewItem* tvi, bool anim)
	{
		tvi->repositionPaintLayer(anim);
		for(int c = tvi->getNumSubItems(), i = 0; i < c; ++i)
		{
			if(AnimTreeViewItem* tvisub = dynamic_cast<AnimTreeViewItem*>(tvi->getSubItem(i)))
			{
				repositionPaintLayers(tvisub, anim);
			}
		}
	}
	// --------------------------------------------------------------------------------
	// demo operations
	void addSubNodeToSelection()
	{
		juce::TreeViewItem* tvisel = getSelectedItem(0);
		if(!tvisel)
		{
			rootItem->setSelected(true, true);
			tvisel = rootItem.get();
		}
		AnimTreeViewItem* tvinew = new AnimTreeViewItem;
		tvisel->addSubItem(tvinew);
		tvisel->setOpen(true);
		triggerAsyncUpdate();
	}
	void deleteSelectedNode()
	{
		// don't delete the root item
		juce::TreeViewItem* tvisel = getSelectedItem(0);
		if(!tvisel || (tvisel == rootItem.get())) return;
		juce::TreeViewItem* tviparent = tvisel->getParentItem();
		if(!tviparent) return;
		int isel = tvisel->getIndexInParent();
		tviparent->removeSubItem(isel, true);
		juce::TreeViewItem* tviselnext = tviparent->getSubItem(isel);
		if(!tviselnext) tviselnext = tviparent;
		tviselnext->setSelected(true, true);
		triggerAsyncUpdate();
	}
	void moveSelectedItem(bool up)
	{
		juce::TreeViewItem* tvisel = getSelectedItem(0);
		if(!tvisel) return;
		juce::TreeViewItem* tviparent = tvisel->getParentItem();
		if(!tviparent) return;
		int isel = tvisel->getIndexInParent();
		if(up) { if(isel <= 0) return; }
		else { if(tviparent->getNumSubItems() - 1 <= isel) return; }
		int iselnew = isel + (up ? -1 : 1);
		tviparent->removeSubItem(isel, false);
		tviparent->addSubItem(tvisel, iselnew);
		// trigger the internal updateVisibleItems()
		resized();
		triggerAsyncUpdate();
	}
};
