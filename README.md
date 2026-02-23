## linux_rbtree

Use the linux kernel's (https://github.com/torvalds/linux/) `rbtree` implementation in userspace.
In `src/main.c` you can see a POC of how you can use `rbtree` to implement `std::map<uint32_t, void *>`.

taken from commit: 6de23f81a5e08be8fbf5e8d7e9febc72a5b5f27f
