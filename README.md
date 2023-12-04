# Oniguruma extension for ngn/k

Provided functions and intended behavior:
```k
> m:`"./libregex.so"2:(`match;2)
> m["asd";"someasdstring"]
4


> c:`"./libregex.so"2:(`indices;2)
> c["(\\d+)-(\\w+)";"123-abc 456-def"]
0 7 0 3 4 7 8 15 8 11 12 15

> cs:`"./libregex.so"2:(`substrings;2)
> cs["(\\d+)\\s(\\w+)";"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"]
`3`blue`4`red`1`red`2`green`6`blue`2`green

> r:`"./libregex.so"2:(`replace;3)
> r["(asd)";"some asd string";"bsd"]
some bsd string
```
