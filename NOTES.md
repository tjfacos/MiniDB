MiniDB Project Notes
====

---
# TODO

---

# Data Types

| Type        | Description                  | Size (Bytes)        |
|-------------|------------------------------|---------------------|
| `INT`       | 32-bit integer               | `4`                 |
| `FLOAT`     | 32-bit floating point number | `4`                 |
| `BOOL`      | Boolean Value                | `1`                 |
| `STRING(n)` | `n`- character string        | `n (1 <= n <= 255)` |
| `BINARY(n)` | `n` - byte data chunk        | `n (1 <= n <= 255)` |

# Schema Representation

# Data Representation

MiniDB utilises a `db/` directory, with two subdirectories

* `index/`, which stores index files for each table (B+ trees)
* `data/`, which stores the rows themselves for each table

Both these file types are subdivided into pages.

## Pages

A page is a `8 KiB / 8192 B` run of data, made up of 

```
[HEADER]
[DATA]
[FREE SPACE]
```

* The `HEADER` contains
  * A unique ID (Page Number within this file)
  * A page type flag
  * Number of slots
  * Number of slots being used
  * Bitmap of occupied slots
* Pages have **slots** of a fixed size. These could be B+ tree nodes, or table rows

## Index Files

* Each attribute in table generates a new index file, by which we can search for rows by this attribute's value
* The file will be a binary file named `index/<table_name>/<attribute_name>.idx`. 
  * This file grows to handle the size of the B+ tree it stores.
* The file is structured...

```
[HEADER PAGE ]
[INDEX PAGE 1]
[INDEX PAGE 2]
...
[INDEX PAGE N]
```
* The `HEADER` contains
   * Total number of pages
   * Total number of **index** pages
   * Bitmap of overflow vs index pages in the file
   * Bitmap of **full** index pages
   * Bitmap of **full** overflow pages
   * Degree of the B+ Tree
   * Key size
   * Pointer to the root of the tree
* Each page slot (in the standard index pages) contains a B+ Tree node, where the degree is determined by the below function, based on key size

```c++
int calculate_degree(int key_size) {
    int max_degree = (USABLE_PAGE_SIZE + key_size) / (key_size + POINTER_SIZE);
    return (int)(max_degree * 0.85);  // Use 85% of maximum
}
```

* An internal node, in binary, of a B+ tree of degree `d` is structured: `[Ptr 1][Key 1][Ptr 2][Key 2] ... [Key d - 1][Ptr d]`
   * A key is a data value
   * A pointer refers to another slot in the file, represented as `[Type (Root / Internal / Leaf / Row / Overflow)][Page Number][Slot Number]`
* A leaf node, of a degree-`d` tree, is structured: `[Key 1][Ptr 1][Key 2][Ptr 2] ... [Key d - 1][Ptr d - 1][PADDING]`
  * These pointers point to rows or overflow nodes (linked lists in the overflow pages), which are structured `[Key][Ptr]`

## Data Files

* `data/<table_name>.dat` stores the rows for `<table_name>`
* Structured... 
```
[HEADER PAGE    ]
[ROW DATA PAGE 1]
[ROW DATA PAGE 2] 
... 
[ROW DATA PAGE N]
```
* Header contains
  * The number of pages
  * Bitmap of **full** pages

---
# Networking & Authentication

**(Let `S` be the server, and `C` be the client)**

## Initialisation (Handshake / Mutual Authentication)
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
 * Both sides generate a session key,`SESSION_KEY = HMAC(DB_SECRET, nonceS APPEND nonceC)`

## Messaging (MIMP : MiniDB Message Protocol)

For one side to send a message to the other side...

 * Generate a 24-byte nonce, using `libsodium`'s `random_buf` function
 * Generate an encrypted payload (containing the encrypted data and a MAC), using `libsodium`'s `crypto_secretbox_easy`
 * Generate the MIMP packet, which includes...
   * A magic byte (Always `0xDB`) (1 B)
   * A version byte (1 B)
   * The length of the payload (2 B) (this is the **length of the encrypted payload**, _**NOT**_ the original message)
   * The nonce (24 B)
   * The Encrypted Payload

### MIMP Packet Structure

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

