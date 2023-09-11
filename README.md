# TestTreeViewAnim
 
https://github.com/yu2924/TestTreeViewAnim/assets/88780921/a7bdbb6a-ea91-4c5b-a24f-4447785e54cb

## What is this?

ユーザがGUIを操作する際に、アニメーションは状態の遷移を把握するのに役立ちます。
JUCEによるGUIの構築は容易ですが、アニメーションはあまり活用されていません。  
このコードはツリービューの描画に、ユーザー操作に応じたアニメーションを加える実験です。  
As users interact with a GUI, animation helps them understand state transitions.
Building a GUI with JUCE is easy, but animation is not widely used.  
This code is an experiment to add animations to the tree view drawing in response to user actions.

  ## Requirement

* JUCE framework 7.0.7: [download](https://juce.com/get-juce/download), [repository](https://github.com/juce-framework/JUCE)
* Projucer: [download](https://juce.com/discover/projucer), or build from source
* C++ build tools: Visual Studio, Xcode, etc.
* ASIO SDK (optional, Windows only): https://www.steinberg.net/developers/

## How to build

1. Open the .jucer file with the Projucer.
2. Correct the JUCE module path and properties, add exporters and save.
3. Build the generated C++ projects.

## Written by

[yu2924](https://twitter.com/yu2924)

## License

CC0 1.0 Universal
