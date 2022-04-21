# Rapid

Visual code editor primarily built for game engines.

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

## License

This project is licensed under MIT.
