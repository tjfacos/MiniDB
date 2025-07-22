MiniDB Project Notes
====

---
# TODO

1) Function to read config file is written (util::get_config). Use this + cli arguments (see main) to read config, and load DB_SECRET to Server.
2) Use DB_SECRET to complete the authentication method and message encryption
3) Use GoogleTest to write a test client (in test directory), that performs the other side of the handshake, and tries to send and receive messages

---
# Authentication

**(Let `S` be the server, and `C` be the client)**

1) **Mutual authentication**
    * `S` generates a nonce (random number), `nonceS` (16-bytes), and sends it to `C`
    * `C` calculates `HMAC(DB_SECRET, nonceS)` (32-bytes), and returns that to `S`
    * `S` also calculates `HMAC(DB_SECRET, nonceS)`. 
      * If it reaches the same answer, `S` returns `OK`. 
      * Otherwise, `S` returns `NO` and disconnects.
    * `C` generates a nonce (random number), `nonceC` (16-bytes), and sends it to `S`
    * `S` calculates `HMAC(DB_SECRET, nonceC)` (32-bytes), and returns that to `C`
    * `C` also calculates `HMAC(DB_SECRET, nonceC)`. 
      * If it reaches the same answer, `C` returns `OK`. 
      * Otherwise, `C` returns `NO` and disconnects.
2) **Shared Key Generation**
    * `SESSION_KEY = HMAC(DB_SECRET, nonceS APPEND nonceC)`
3) All future messages are encrypted with the `SESSION_KEY`

---
# MIDB Message Protocol (MIMP)

```
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  MAGIC (0xDB)  |   VERSION    |        PAYLOAD LENGTH         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                        NONCE (24 bytes)                       +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
|              ENCRYPTED PAYLOAD (MAC + Ciphertext)             |
|                      (length = LENGTH)                        |
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

