#!/usr/bin/env k
r:`"./libregex.so"2:(`match;2)
r["asd";"someasdstring"]

c:`"./libregex.so"2:(`captures;2)
c["(.+)-(.+)";"123-abc 456-def"]
