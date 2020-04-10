scrabble
========

Implementation of Scrabble AI from:
    https://www.cs.cmu.edu/afs/cs/academic/class/15451-s06/www/lectures/scrabble.pdf
    https://ericsink.com/downloads/faster-scrabble-gordon.pdf


Compilation on Ubuntu 16.04 (there's a linker bug?):
    $ CC=gcc-8 CXX=g++-8 cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold ..


Plan:
+ Implement rules naively   [DONE]
+ Implement scoring naively [DONE]
+ Implement move generation [DONE]
+ Test with games from online:
  + ISC games               [DONE]
    + ISC move parser       [DONE]
    + ISC games grabbed manually [DONE]
    + Is there a way to download games?
  + cross-tables.com games  [DONE]
    + Minimal GCG parser    [DONE]
    + NOTE: not all games have all the tiles if opponent didn't share tiles
    + NOTE: GCG/cross-tables use flipped row/column notation (i.e. letters=columns)
+ Engine self-player        [DONE]
+ Engine self play fuzz test?
+ Find dictionaries:
  + CSW19                   [DONE]
  + TWL15
  + NWL18
  + Older:
      + CSW15
      + CSW12
      + CSW07
      + TWL06
      + TWL98
+ DAWG creator for dictionary
+ GADDAG creator for dictionary
+ Benchmark vs [Quackle](https://github.com/quackle/quackle)
+ NN + [MCTS](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) + [RL](lilieanweng.github.io/lil-log/2018/02/19/a-long-peek-into-reinforcement-learning.html) for evaluation function?
