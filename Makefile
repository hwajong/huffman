huffman: main.cpp
	rm -rf $@
	g++ -g -W -Wall -o $@ $<
	./$@ input.txt

