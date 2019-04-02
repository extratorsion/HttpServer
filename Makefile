server: source/server
	$(shell mv source/server ./)

source/server:  
	make -C source

clean:
	rm server
