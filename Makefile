all:
	gcc traceroute_samplecode.c -o trace
test:
	./trace 127.0.0.1
