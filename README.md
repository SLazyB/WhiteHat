# Cavell Teng, Ian Lee, Alex Francoletti - Cryptography and network Security 1 - White Hat Project

## External Dependencies:
C++ 11 or later

## Usage Instructions:
-Compile first: g++ server.cpp toydes.cpp rsa.cpp blum_gold.cpp -o server.exe
               g++ client.cpp toydes.cpp rsa.cpp blum_gold.cpp -o client.exe
-Run the server first (./server.exe)
-Then run the client (./client.exe)

-If it passes initial HMAC verification tests the client side terminal will prompt you to enter a Username
then a Password. After that you are free to communicate between client and server.
- You can close the client in ther client terminal and re-attempt connection with the same Username and password;
failure to enter the correct password will terminate the connection.

## Other Notes:
-Typing q into the client side will terminate the client's connection.
