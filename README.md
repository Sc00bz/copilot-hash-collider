# copilot-hash-collider

Used to find collisions in the copilot's hash function. This uses a meet in the middle attack. P.S. I was lazy and used `unordered_multimap` so your compiler needs to be able to do C++11.

## Usage

### To hash a string
```
./copilotcollider hashme
```

### To collide a hash
```
./copilotcollider target-hash prefix-length suffix-length
```

#### Example (this will take about a minute and use about 620 MiB of memory and should find 854673 collisions)
```
./copilotcollider 69712246 5 6 > out.txt
```

# License issue

`obfuscateWord()` was taken from https://gist.github.com/moyix/f78e0b0d5724a1bf02e1a035e8bec136 which has no license.
