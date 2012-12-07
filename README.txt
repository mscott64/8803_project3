README
Michelle Scott

There is one Makefile in the root folder.
"make all" will make the client, rpc, proxy, and server executables.
The executables made are client_ex, rpc_ex, proxy_ex, and server_ex.
Running any of these with the -u flag prints its usage.

This system needs both an encryption and decryption server.
In order to run the testing system, there must be one proxy_ex -e, rpc_ex -e, 
proxy_ex, rpc_ex, server_ex, and client_ex.

Usage:
./client_ex [-r num_requests]
	    [-t num_threads]
	    [-f num_files]
	    [-b big_files]

./proxy_ex [-p port_num]
	   [-t num_threads]
	   [-e encrypt] // Set flag for the encryption proxy server

./rpc_ex [-p port_num]
	 [-t num_threads]
	 [-e encrypt] // Set flag for the encryption rpc server

./server_ex [-p port_num] [-t num_threads]

