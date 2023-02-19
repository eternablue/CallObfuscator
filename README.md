# CallObfuscator

Call any export in a obfucated way without exposing the api string name or in the IAT.
It does so by looping all loaded modules, and all exports in those to find the export address. It uses djb2 hashing algorithm at compile time to compare api's.

Syntax :
```cpp
CALL(Sleep, 2000);
```

Result (Before / After ):

![image](https://media.discordapp.net/attachments/780153367305256981/1076963877226565662/image.png?width=720&height=348)
