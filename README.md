# Copilot Hash Collider

Used to find collisions in the copilot's hash function. This uses a meet in the middle attack. P.S. I was lazy and used `unordered_multimap` so your compiler needs to be able to do C++11.


# Usage

## Hashing a string

```
./copilotcollider hashme
```
```
./copilotbrute hashme
```


## Meet in the middle attack

`prefix-length` should be at least 6, otherwise use early exit brute force. `suffix-length` should be at least `prefix-length - 1`.
```
./copilotcollider target-hash prefix-length suffix-length
```


### Example

This will take about a minute, use about 620 MiB of memory, and should find 854673 collisions.
```
./copilotcollider 69712246 5 6 > out.txt
```


## Early exit brute force

This should be about as fast or faster than meet in the middle attack when `prefix-length` is less than 12.
```
./copilotbrute target-hash length
```


### Example

This will take about 10 seconds and should find 854673 collisions.
```
./copilotbrute 69712246 11 > out.txt
```


# How it works

## Meet in the middle attack

The algorithm takes a 32 bit integer and multiplies by 33 then xors a character and keeps the lower 32 bits. If you try to go backwards imagine having the full 38 bit result after a step. You could easily just guess a character that solves `(result38 ^ ch) % 33 == 0`, but we don't have the full 38 bit result. What we can do is make a guess at it and go from there. There are only 33 possible `result38`s. `result38 = result32 + (i << 32)` where `i` is an integer 0 through 32. We take each one and solve `(result38 ^ ch) % 33 == 0`. Note you are on average going to get character set length number of solutions for all of the `result38`s. For this it's just lowercase letters so character set length is 26. There is a 26/33 chance at solving `(result38 ^ ch) % 33 == 0` and there are 33 of these to try so you are expected to get about 26/33\*33 or 26.

There are some optimizations on solving `(result38 ^ ch) % 33 == 0` because it's just lowercase letters. `result38 ^ 0x60` because the values are 01100001b to 01111010b and 01100000b (0x60) is always set and the values left to xor are 5 bits: 00001b to 11010b. Only one or zero of these 5 bits values can make it work and it's easy to solve. It's either `(result38 ^ 0x60) - (result38 ^ 0x60) % 33` or `(result38 ^ 0x60) - (result38 ^ 0x60) % 33 + 33` and you know which one depending on if `(result38 ^ 0x60) >= (result38 ^ 0x60) % 33`.


# License issue

`obfuscateWord()` was taken from https://gist.github.com/moyix/f78e0b0d5724a1bf02e1a035e8bec136 which has no license.
