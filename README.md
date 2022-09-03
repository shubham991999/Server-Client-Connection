### Server-Client communication using sockets

1. Server Side-

   The server creates a socket and listens on some speciﬁc port for client connection.
   After connecting with the client, it waits for client requests. 
   The server should parse the string received from the client, which will contain
   either of NLINEX, READX or INSERTX requests.
	 
   
   > NLINEX             : returns the no. of lines in the ﬁle to the client.
	 
	 > READX k            : Reads the k-th line from the ﬁle and return its content to client.  
	
	 > INSERTX k message  : Inserts the string speciﬁed in message in the k-th line to the ﬁle.
	 
	 Here file is **server_file.txt**.
	 
2. Client Side-

   The client establishes a connection with the server socket on the speciﬁed port.
	 It passes NLINEX, READX and INSERTX requests to the server and fetches the output message from the server and displays it to the user.
	 
### Instructions for Execution

1. Compile :

   **gcc file_name.c -o file_name**
	 
2. Run :

   **./file_name**
	
	
     
  

