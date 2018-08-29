Cooper Timmerman
CSCI 4273
Programming Assignment 4 - Proxy Server

--------------------------------------

Included files:

webproxy.c
webproxy.h
forbiddensites.txt

--------------------------------------

Usage:

Change proxy settings in browser to route to specified port number, along with 127.0.1.1 IP.

Compile with: gcc -o webproxy webproxy.c
Run with: ./webproxy <Proxy port number set in browser> <timeout (seconds)>

example: ./webproxy 10001 60

--------------------------------------