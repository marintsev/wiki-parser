all: debug release
	make -C ./Debug all
	
release:
	make -C ./Release all

wiki0.xml: wiki1.xml
	cat $< | head -n 10000 > $@

clean:
	make -C ./Debug clean
	
valgrind: ./wiki-parser wiki0.xml
	valgrind --leak-check=full --show-leak-kinds=all ./wiki-parser wiki0.xml
