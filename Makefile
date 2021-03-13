all:
	gcc traceroute.c -o traceroute
test:
	./traceroute localhost
test2:
	./traceroute www.csie.ntu.edu.tw
clean:
	rm ./traceroute
