---
layout: post
title: Flood-fill in MMS 
date: 2026-07-01 10:00:00 +0200
tags: [algo, mms]
---

* TOC
{:toc}

#### 1. The real path : the claude path 🤖

I did not write this algorithm from scratch. I asked Claude Code to write the flood-fill for the [mms](https://github.com/mackorone/mms) simulator. It worked on the first runs, but it was a black box 🧙.

So I read the code line by line and asked Claude to explain it to me. This post is not a tutorial. It is the list of things I ended up understanding, that caught my interest and that I wanted to share.

I did not know the right words at the time. So I asked Mistral to write a proper prompt for me to get mms working. Here is the full prompt it gave me: <br>
[floodfill_prompt.txt](https://github.com/lili-lines/ratatouille/blob/main/algo/floodfill_prompt.txt).

<figure style="text-align:center; margin:0;">
  <img src="{{ '/assets/img/mms_perso.gif' | relative_url }}" alt="mms maze simulator" width="360" style="display:block; margin:0 auto;">
  <figcaption style="font-size:0.85rem;">Screen capture of the algorithm running in the mms simulator</figcaption>
</figure>

**The algorithm**

Here is the core of the algorithm, to help us understand it better.

$$d(\text{cell}) = 1 + \min_{n}\, d(n), \qquad d(\text{goal}) = 0$$

$n$ = the neighbours with no wall in between <br>
$d$ = smallest distance to the goal among the neighbours, plus 1

Every cell gets its distance to the goal. The mouse then walks downhill (big → small) until it reaches $0$.

Here are more detailed explanations:<br>
. [Micromouse-from-scratch walkthrough](https://medium.com/@minikiraniamayadharmasiri/micromouse-from-scratch-algorithm-maze-traversal-shortest-path-floodfill-741242e8510) <br>
. [UC Irvine's flood-fill page](https://ieee.ics.uci.edu/micromouse/floodfill.html) <br>
. [academic PDF](https://marsuniversity.github.io/ece387/FloodFill.pdf)


#### 2. Coordinates & directions in MMS

MMS puts the origin $(0,0)$ at the bottom-left, like a math graph:

<div class="two-col" markdown="1">
<div class="col" markdown="1">
<img src="{{ '/assets/img/graph_maths.png' | relative_url }}" alt="MMS coordinate graph" width="95%">
</div>
<div class="col" markdown="1" style="flex: 1.3">
→ x = column, going EAST (right), x increases <br>
↑ y = row, going NORTH (up), y increases <br>
← WEST = x decreases <br>
↓ SOUTH = y decreases <br>
</div>
</div>

Cardinal directions NORTH, EAST, SOUTH, WEST are coded $0,1,2,3$ in that order.


#### 3. The modulo $\%$, the rotation trick

The mouse keeps its heading in `mouseDir` $(0=N, 1=E, 2=S, 3=W)$. The directions follow each other clockwise, so turning right = +1, turning left = -1. The $\% 4$ wraps around to stay in $[0:3]$:

```cpp
turnRight() { mouseDir = (mouseDir + 1) % 4; }   // N→E→S→W→N...
turnLeft()  { mouseDir = (mouseDir + 3) % 4; }   // +3 is the same as -1
```

One left = three rights. I use +3, not -1, because $-1\%4$ gives $-1$ in C++, which is not a valid direction.


#### 4. The flush

The mouse talks to the simulator through `stdin/stdout`. The computer does not send letters one by one: it piles them in a buffer and sends only when the buffer is full.

A command like `moveForward` is short, so the buffer never fills, the command stays stuck, and the mouse waits for a reply that never comes.

The flush forces the buffer to empty now. In C++, `std::endl` does both: newline + flush.

```cpp
std::cout << cmd << std::endl;   // sends & empties the buffer right away
```

Every command must be flushed, or the dialogue breaks.


#### 5. On the real robot

The real robot has 3 runs: exploration, return, and speed run. It must not lose the wall map it built.

Robot stays on: the map lives in RAM. Clear the distance grid, keep the walls, re-run flood-fill, and the path is recomputed in a fraction of a second.

Robot off: RAM is wiped. The walls must be written to non-volatile memory (EEPROM / Flash). A 16×16 maze is 256 bytes, one per cell, which is tiny.

