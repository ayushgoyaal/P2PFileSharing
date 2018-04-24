--server.c : Have server code.This code is always ready to recieve new client connections
--client.c : Have clients code.This code is always ready to send files published by it.It can fetch only once but can make multiple publish. 

How to execut server code?
This can be accomplished via a command-line argument. Example: ./server server-port

How to execut client code?
This can be accomplished via a command-line argument. Example: ./client server-ip server-port

--make_file : Can compile both server and client code creating ./server and ./client as a exe file

--server : object file for server
--client : object file for client

