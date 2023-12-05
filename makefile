# manual steps:
#   go to ../..
#   make c  (for "clean")
#   edit makefile to add "-Wl,-E" to compiler options
#   make k libk.so
#   come back to this directory

0:libregex.so
	./regex.k
libregex.so:regex.c
	$(CC) -shared -fPIC $^ -I ../.. -L ../.. -I /opt/homebrew/include -L /opt/homebrew/lib -lk -lonig -o $@
