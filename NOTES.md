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
    * `S` generates a nonce (random number), `nonceS` (16-bytes)
    * `C` returns `HMAC(DB_SECRET, nonceS)` (32-bytes)
    * `S` sends `OK`
    * `C` generates a nonce (random number), `nonceC` (16-bytes)
    * `S` returns `HMAC(DB_SECRET, nonceC)` (32-bytes)
    * `C` sends `OK`
    * If the returned values aren't correct at any stage, the connection closes
2) **Shared Key Generation**
    * `SESSION_KEY = HMAC(DB_SECRET, nonceS || nonceC)`
3) All future messages are encrypted with the `SESSION_KEY`

---
# MIDB Message Protocol

```
0               1               2               3
0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| MAGIC 0xDB    | PADDING       | LENGTH                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| PADDING       | REQUEST TYPE  |                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| DATA ...................................................... |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
```

## Types of Request

| Operation    | Symbol | Description                      | Notes |
|--------------|--------|----------------------------------|-------|
| Create       | C      | CREATE ...                       |       |
| Insert       | I      | INSERT ...                       |       |
| Update       | U      | UPDATE ...                       |       |
| Select       | S      | SELECT ...                       |       |
| Delete       | D      | DELETE ...                       |       |

