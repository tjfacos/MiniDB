MiniDB Project Notes
====

---
# TODO

1. Start with `Page`, `FileManager`, `DataType`
2. Build `Schema` and basic `Row` handling
3. Implement `Table` and `RecordManager` for data files
4. Create `BTreeNode` and `BPlusTree` for indexes
5. Build `Index` and `IndexManager` to tie it together
6. Finally, implement `QueryExecutor` for actual database operations

---

# Data Types

| Type        | Description                  | Size (Bytes)        |
|-------------|------------------------------|---------------------|
| `INT`       | 32-bit integer               | `4`                 |
| `FLOAT`     | 32-bit floating point number | `4`                 |
| `BOOL`      | Boolean Value                | `1`                 |
| `STRING(n)` | `n`- character string        | `n (1 <= n <= 255)` |
| `BINARY(n)` | `n` - byte data chunk        | `n (1 <= n <= 255)` |


# Data Representation

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

## Schema Files

* Each table has a schema file `db/<table_name>/<table_name>.schema`
* Structure
```
[Number of Attributes]
// For each attribute
[Length of Attribute Name]
[Attribute Name]
[Type]
[Type Argument (for e.g. STRING or BINARY, this will be the size n)]
[Flags (e.g. 1 in the first position means value must be unique)]
```

## Index Files

* Each attribute in table generates a new index file, by which we can search for rows by this attribute's value
* The file will be a binary file named `db/<table_name>/<attr_name>.idx`. 
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

* `db/<table_name>/<table_name>.dat` stores the rows for `<table_name>`
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
# Program Structure

# MiniDB Implementation Classes and Processes

Based on your data representation design, here are the key processes/classes you'll need to implement:

## Core Storage Layer

**`Page`** - Represents a single 8KB page
- Load/save page from/to disk
- Manage slot allocation using bitmap
- Handle different page types (header, index, data, overflow)

**`FileManager`** - Handles low-level file I/O
- Open/close files
- Read/write specific pages
- Memory-mapped file access or buffered I/O
- File growth management

**`BufferPool`** - Manages pages in memory (if not using mmap)
- Page caching and replacement
- Pin/unpin pages for active use
- Dirty page tracking and flushing

## Schema Management

**`Schema`** - Represents table structure
- Parse `.schema` files
- Validate data types and constraints
- Calculate row sizes and slot configurations

**`SchemaManager`** - Manages all table schemas
- Load schemas on startup
- Create/modify/drop table schemas
- Schema validation for operations

## B+ Tree Implementation

**`BPlusTree`** - Main B+ tree interface
- Search, insert, delete operations
- Handle duplicate keys with overflow pages
- Tree traversal and range queries

**`BTreeNode`** - Represents individual tree nodes
- Internal node vs leaf node handling
- Key/pointer management
- Node splitting and merging logic

**`BTreeNodeFactory`** - Creates nodes of appropriate type
- Determine node type from page data
- Handle different key sizes and degrees

## Data Management

**`Table`** - Represents a database table
- Insert/update/delete rows
- Schema enforcement
- Integration with indexes

**`Row`** - Represents a single table row
- Serialize/deserialize to/from binary format
- Type validation and conversion
- Handle fixed-size slot formatting

**`RecordManager`** - Manages row storage in data files
- Free slot allocation in data pages
- Row insertion and retrieval
- Garbage collection of deleted rows

## Index Management

**`IndexManager`** - Manages all indexes for a table
- Create/drop indexes
- Coordinate updates across multiple indexes
- Handle index file creation and initialization

**`Index`** - Wrapper around B+ tree for specific attributes
- Attribute-specific key extraction
- Duplicate key handling with overflow pages
- Index maintenance during table operations

## Query Processing

**`QueryExecutor`** - Executes basic operations
- SELECT with WHERE clauses
- INSERT, UPDATE, DELETE
- Join operations (if you implement them)

**`Predicate`** - Represents WHERE clause conditions
- Evaluate conditions against rows
- Optimize for index usage

## Utility Classes

**`DataType`** - Handle type system
- Serialize/deserialize different data types
- Type validation and conversion
- Size calculations

**`Pointer`** - Your `[Type][Page][Slot]` structure
- Navigate between pages and slots
- Type safety for different pointer targets

**`BitMap`** - Utility for slot and page bitmaps
- Set/clear/test bits efficiently
- Find first free slot/page

## Database Engine

**`Database`** - Top-level database interface
- Open/close database
- Manage tables and their associated files
- Coordinate schema, data, and index files

**`Transaction`**
- Basic transaction boundaries
- Simple rollback on errors

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

