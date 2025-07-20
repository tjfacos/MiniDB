MiniDB Project Notes
====

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

