#!/usr/bin/env k

m:`"./libregex.so"2:(`match;2)
m["asd";"someasdstring"]

c:`"./libregex.so"2:(`indices;2)
c["(\\d+)-(\\w+)";"123-abc 456-def"]

cs:`"./libregex.so"2:(`substrings;2)
0N 2#cs["(\\d+)\\s(\\w+)";"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"]

re:`"./libregex.so"2:(`replace;3)
re[".*:";"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green";"redacted|"]
