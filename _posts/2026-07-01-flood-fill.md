---
layout: post
title: Flood-fill in MMS
date: 2026-07-01 10:00:00 +0200
tags: [algo, mms]
---

[DESSIN FLOOD-FILL] + [gif simu mms car satisfaisant]

Flood-fill gives every cell its distance to the goal. The mouse then walks downhill (big → small) until it reaches $0$, the goal.

I won't re-explain the algorithm, there are detailed explanations all over the web. Here I keep only what I actually did in the [mms](https://github.com/mackorone/mms) simulator.

Just the core of the algorithm:

$$d(\text{cell}) = 1 + \min_{n}\, d(n), \qquad d(\text{goal}) = 0$$

$n$ = ranges over the neighbours with no wall in between<br>
$d$ = the smallest distance to the goal among its neighbours + 1 


References flood-fill :<br>
. [Micromouse-from-scratch walkthrough](https://medium.com/@minikiraniamayadharmasiri/micromouse-from-scratch-algorithm-maze-traversal-shortest-path-floodfill-741242e8510) <br>
. [UC Irvine's flood-fill page](https://ieee.ics.uci.edu/micromouse/floodfill.html) <br>
. [academic PDF](https://marsuniversity.github.io/ece387/FloodFill.pdf) <br>


#### (1) Coordinates & directions in MMS

MMS puts the origin $(0,0)$ at the bottom-left, like a math graph:

<div class="two-col" markdown="1">
<div class="col" markdown="1">
<img src="{{ '/assets/img/graph_maths.png' | relative_url }}" alt="MMS coordinate graph" width="95%">
</div>
<div class="col" markdown="1" style="flex: 1.3">
**→ x** = column, going EAST (right), **x** increases <br>
**↑ y** = row, going NORTH (up), **y** increases <br>
← WEST = **x** decreases <br>
↓ SOUTH = **y** decreases <br>
</div>
</div>

🧭 Cardinal directions = NORTH, EAST, SOUTH, WEST, coded as $0,1,2,3$ in that order.


#### (2) The modulo $\%$ in C++, the rotation trick

The mouse keeps its heading in `mouseDir` $(0=N, 1=E, 2=S, 3=W)$. Since the directions follow each other clockwise, turning right = +1, turning left = -1. The $\% 4$ modulo wraps around to stay within $[0:3]$:

```cpp
turnRight() { mouseDir = (mouseDir + 1) % 4; }   // N→E→S→W→N...
turnLeft()  { mouseDir = (mouseDir + 3) % 4; }   // +3 is the same as -1
```

So 1 left = 3 rights <br>
And $−1\%4$ gives −1, a negative number in C++, which is not a valid direction $[0:3]$


#### (3) The flush in MMS

The mouse sends text to the simulator through `stdin/stdout`. But the computer doesn't send letters one by one: it piles them up in a buffer, like a bucket, and only sends when the bucket is full.

The problem is that a command like `moveForward` is short → the bucket isn't full → the command stays stuck → the mouse waits for a reply that never comes.

To fix it, we use the flush: it forces the bucket to empty immediately. In C++, `std::endl` does 2 things at once: `'\n'` + flush.

```cpp
std::cout << cmd << std::endl;   // sends & empties the buffer right away
```

Every command sent to MMS must be followed by a flush, otherwise the dialogue breaks.


#### (4) On the real robot

On the real robot there are 3 types of runs: exploration, the return (lets you explore more), and the speed run. You must not lose the hard-earned wall map.

- Robot stays powered **on** 🟢: the map lives in RAM (global variables). You just clear `distance_grid`, keep `walls`, and re-run `floodFill()` → the perfect path is recomputed in a fraction of a second.
- Robot powered **off** 🔴: RAM is wiped. You must write `walls` to non-volatile memory (EEPROM / Flash), physical storage. A 16×16 maze = 256 bytes (1 byte per cell), which is tiny.

<br>


#### Takeaways

🤖 And today this kind of task is what an AI coding assistant like Claude Code nails, fast and well. The real value is understanding it, knowing what to ask for, and spotting when the output is wrong.

