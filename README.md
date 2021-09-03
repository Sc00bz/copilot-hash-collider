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

`prefix-length` should be at least 4, otherwise use early exit brute force. `suffix-length` should be at least `prefix-length - 1`.
```
./copilotcollider target-hash prefix-length suffix-length
```


### Example

This will take about a minute, use about 620 MiB of memory, and should find 854673 collisions.
```
./copilotcollider 69712246 5 6 > out.txt
```


## Early exit brute force

This should be about as fast or faster than meet in the middle attack when `prefix-length` is less than 5.
```
./copilotbrute target-hash length
```


### Example

This will take about 3 minutes and should find 854673 collisions.
```
./copilotbrute 69712246 11 > out.txt
```


# License issue

`obfuscateWord()` was taken from https://gist.github.com/moyix/f78e0b0d5724a1bf02e1a035e8bec136 which has no license.
