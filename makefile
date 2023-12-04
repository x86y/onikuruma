0:libregex.so
	./regex.k
libregex.so:regex.c
	$(CC) -shared -fPIC $^ -I ../.. -L ../.. -I /opt/homebrew/include -L /opt/homebrew/lib -lk -lonig -o $@
