scrabble
========

Implementation of Scrabble AI from:
    https://www.cs.cmu.edu/afs/cs/academic/class/15451-s06/www/lectures/scrabble.pdf
    https://ericsink.com/downloads/faster-scrabble-gordon.pdf


Compilation on Ubuntu 16.04 (there's a linker bug?):
    $ CC=gcc-8 CXX=g++-8 cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold ..


# Plan:
- [x] Implement rules naively
- [x] Implement scoring naively
- [x] Implement move generation from [original paper](https://www.cs.cmu.edu/afs/cs/academic/class/15451-s06/www/lectures/scrabble.pdf) with DAWG
- [ ] Implement move scoring from [original paper](https://www.cs.cmu.edu/afs/cs/academic/class/15451-s06/www/lectures/scrabble.pdf)
- [ ] Implement move generation from [new paper](https://ericsink.com/downloads/faster-scrabble-gordon.pdf) with GADDAG
- [ ] Implement official API:
    - [ ] Implement engine API in C
    - [ ] Implement rules API in C
    - [ ] Wrap engine API in C++ with better type checking
    - [ ] Wrap rules API in C++ with better type checking
    - [ ] Utils C++ library for parsing various move formats
- [ ] Test with games from online:
  - [x] ISC games
    - [x] ISC move parser
    - [x] ISC games grabbed manually
    - [ ] Is there a way to download games?
  - [x] cross-tables.com games
    - [x] Minimal GCG parser
    - [x] **NOTE:** not all games have all the tiles if opponent didn't share tiles
    - [x] **NOTE:** GCG/cross-tables use flipped row/column notation (i.e. letters=columns)
- Engine self play fuzz test
    - [x] Engine self-player
- Find dictionaries:
  - [x] CSW19
  - [ ] TWL15
  - [ ] NWL18
  - [ ] Older:
      - [ ] CSW15
      - [ ] CSW12
      - [ ] CSW07
      - [ ] TWL06
      - [ ] TWL98
- [ ] DAWG creator for dictionary
- [ ] GADDAG creator for dictionary
- [ ] Benchmark vs [Quackle](https://github.com/quackle/quackle)
- [ ] NN + [MCTS](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) + [RL](lilieanweng.github.io/lil-log/2018/02/19/a-long-peek-into-reinforcement-learning.html) for evaluation function?
