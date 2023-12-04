0:libregex.so
	./regex.k
libregex.so:regex.c
	$(CC) -shared -fPIC $^ -L../.. -lk -L /opt/homebrew/lib -lonig -o $@
