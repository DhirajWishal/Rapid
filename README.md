# Rapid

Visual code editor primarily built for game engines.

## How to run

First, clone the repository to a local directory.

```bash
git clone https://github.com/DhirajWishal/Rapid {SOURCE}
cd {SOURCE}
git submodule init
git submodule update
```

Once the repository is downloaded and configures, you can compile the program using CMake.

```bash
mkdir Build
cmake -S . -B Build
cd Build
cmake --build . --config Release
```

## Why Rapid

There were two main reasons to develop this software. One being because I was looking into visual programming as a viable way of programming, and how it could
improve programming in general. The second begin that I was developing a game/ graphics engine and it would be nice to have a visual programming feature to it,
so that we can easily edit and test out pipeline and shaders.

This got me thinking, why not expand it to C++ and then let game engines use this as a scripting language. But with the added benefit of it being coded in C++
rather than a scripting language.

The reason for game engines to use a scripting language for their game logic, for Unreal we got C++ and the blueprint system, Unity uses C# and Godot has their
own scripting language. This is because developers can less focus on the nitty-gritty things of C++, and just script the game logic which directly impact the
development times. But one problem with this approach is that it won't perform as well as just developing it in C++, or the engine is required to be more
complex to do JIT, transpilation or any other thing to optimize it so that we won't waste a lot of performance.

But if there is a way for us to speed up C++ development, lower the bar for new game developers to learn how to develop game logic, and to not worry much about
performance and ease the burden from the developers' side of things, it would be great!

That's the reason why I decided to develop Rapid, making it easier for game engine developers to straight up use C++ to code their game logic and not having to
deal with multiple languages and scripting engines to get this part done, while also being flexible enough for them to come up with their own game programming
library rather than having to use a pre-defined set of templates.

## Customizing the UI

The UI can be heavily customizable using a json file. This file defines all the required style parameters used by ImGui. The following contains the default style file.

```json
{
    "Colors": {
        "TitleBg": [
            26,
            30,
            35,
            128
        ],
        "TitleBgActive": [
            26,
            30,
            35,
            192
        ],
        "WindowBg": [
            26,
            30,
            35,
            255
        ],
        "MenuBarBg": [
            26,
            30,
            35,
            255
        ],
        "Header": [
            25,
            133,
            161,
            128
        ],
        "HeaderHovered": [
            25,
            133,
            161,
            255
        ],
        "Tab": [
            242,
            84,
            91,
            64
        ],
        "TabActive": [
            242,
            84,
            91,
            192
        ],
        "TabHovered": [
            242,
            84,
            91,
            255
        ],
        "TabUnfocusedActive": [
            242,
            84,
            91,
            128
        ],
        "TabUnfocused": [
            242,
            84,
            91,
            64
        ]
    },
    "ChildRounding": 6,
    "FrameRounding": 1,
    "PopupRounding": 3,
    "TabRounding": 1,
    "WindowRounding": 3,
    "Font": [
        "Fonts/Manrope/static/Manrope-Regular.ttf",
        16
    ]
}
```

All the customizable style types are,

```txt
[data type] [variable name]

float     Alpha                     
float     DisabledAlpha             
ImVec2    WindowPadding             
float     WindowRounding            
float     WindowBorderSize          
ImVec2    WindowMinSize             
ImVec2    WindowTitleAlign          
ImGuiDir  WindowMenuButtonPosition  
float     ChildRounding             
float     ChildBorderSize           
float     PopupRounding             
float     PopupBorderSize           
ImVec2    FramePadding              
float     FrameRounding             
float     FrameBorderSize           
ImVec2    ItemSpacing               
ImVec2    ItemInnerSpacing          
ImVec2    CellPadding               
ImVec2    TouchExtraPadding         
float     IndentSpacing             
float     ColumnsMinSpacing         
float     ScrollbarSize             
float     ScrollbarRounding         
float     GrabMinSize               
float     GrabRounding              
float     LogSliderDeadzone         
float     TabRounding               
float     TabBorderSize             
float     TabMinWidthForCloseButton 
ImGuiDir  ColorButtonPosition       
ImVec2    ButtonTextAlign           
ImVec2    SelectableTextAlign       
ImVec2    DisplayWindowPadding      
ImVec2    DisplaySafeAreaPadding    
float     MouseCursorScale          
bool      AntiAliasedLines          
bool      AntiAliasedLinesUseTex    
bool      AntiAliasedFill           
float     CurveTessellationTol     
float     CircleTessellationMaxError
ImVec4    Colors[]
```

Supported color formats,

```txt
Text
TextDisabled
WindowBg
ChildBg
PopupBg
Border
BorderShadow
FrameBg
FrameBgHovered
FrameBgActive
TitleBg
TitleBgActive
TitleBgCollapsed
MenuBarBg
ScrollbarBg
ScrollbarGrab
ScrollbarGrabHovered
ScrollbarGrabActive
CheckMark
SliderGrab
SliderGrabActive
Button
ButtonHovered
ButtonActive
Header
HeaderHovered
HeaderActive
Separator
SeparatorHovered
SeparatorActive
ResizeGrip
ResizeGripHovered
ResizeGripActive
Tab
TabHovered
TabActive
TabUnfocused
TabUnfocusedActive
DockingPreview
DockingEmptyBg
PlotLines
PlotLinesHovered
PlotHistogram
PlotHistogramHovered
TableHeaderBg
TableBorderStrong
TableBorderLight
TableRowBg
TableRowBgAlt
TextSelectedBg
DragDropTarget
NavHighlight
NavWindowingHighlight
NavWindowingDimBg
ModalWindowDimBg
```

Make sure that all the colors are a 4 component/ element array, with values ranging from 0 - 255.

## License

This project is licensed under MIT.
