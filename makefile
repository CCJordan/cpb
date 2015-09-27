all: cpbCompile

cpbCompile: main.o copyHelper.o copyFileSet.o
	@g++ -std=gnu++11 copyHelper.o copyFileSet.o main.o -o cpb;
	@echo "Build succeded. Now type \"make install\"";

main.o: main.cpp
	@g++ -std=gnu++11 -c main.cpp
copyHelper.o: copyHelper.cpp
	@g++ -std=gnu++11 -c copyHelper.cpp
copyFileSet.o: copyFileSet.cpp
	@g++ -std=gnu++11 -c copyFileSet.cpp

clean:
	@rm *.o ||:

install: cpb
	@if [ -e /usr/local/bin/cpb ]; then \
		echo "cpb has already been installed to your system."; \
		echo "please use \"make reinstall\" to overwrite it."; \
	else \
		cp ./cpb /usr/local/bin/cpb; \
	fi;

reinstall: cpb
	@cp ./cpb /usr/local/bin/cpb

uninstall:
	@echo "Deleting all evidence."; 
	@-rm /usr/local/bin/cpb
