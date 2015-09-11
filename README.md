
# ice - immutable data structures in C++ #

## Current Status ##

Early development, highly unstable.

# What #

A C++ library for immutable data structures.

These abstract data types are supported:

 - List (aka array)
   - Implementations: flat array, cons cell, slice, empty list.
 - Table, with preserved key order.
   - Implementations: flat hashtable, empty table. (more implementations planned)
 - Blob (raw bytes, aka string)
   - Implementations: flat byte blob, empty blob. (more implementations planned)
 - Symbol (the lispy kind)
   - Implementations: same as Blob
 - Boolean
 - Float
 - Integer
 - Nil

GC is done by refcounting. Values are stored in tagged 64-bit values.

A small Lisp interpreter is also included for fun, with Clojure-like definitions.

## Why? ##

One reason, because I'm planning on using this as a foundation for future language
projects.

Also a pragmatic reason, it seems like most persistent data structure implementions
are inside GC-ed environments, but there are interesting optimizations when
the data structures work together with the GC. Specifically - if we have fully
refcounted data structures (which is fine because it's impossible to create cycles
in immutable values), then we can use the refcount to take some high-performance
shortcuts. If a function consumes a value with a refcount of 1, then that function
is allowed to modify-in-place or dismantle that input value, since no one else is using it.

The Ice library tries to leverage this trick as much as possible. Ice functions
are written in a consume-inputs-by-default style, and most functions will check for
situations where it's safe to modify-in-place.
