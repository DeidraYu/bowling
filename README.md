# bowling
I had a lot of fun doing the assessment for the Bowling Kata assessment for Unreal Engine 5.4. The Kata is found at https://kata-log.rocks/bowling-game-kata and the question was to implement this in Unreal Engine 5.4.

Please find below some instructions and remarks. If you have any further questions don't hesitate to contact me.

## Cloning the repo

git clone https://github.com/DeidraYu/bowling.git

## Building

- Navigate into the `blowling` folder
- Shift+Left-click on `bowling.uproject` and select `Generate Visual Studio project files`.
- After this, a file `bowling.sln` is created. Open this solution is Visual Studio.
- In the Solution Explorer Right-click on `Games->bowling` and select "Build".

Double-click on `bowling.uproject` to open it in the Unreal Editor

## Playing the game

- Use `A` and `D` keys for moving left and right.
- Use the mouse pointer to rotate the direction the ball is thrown into.
  The ball is thrown straight forward on your screen, not toward the mouse! This is intentionally.
- Use the left mouse button to throw (roll) the ball.
- At this moment you can cheat by moving the ball left-right even when it is already rolling (a feature handy for debugging).

## Interpretation of the Assessment
The combination of the Bowling Game Kata and the Unreal Engine 5.4 implementation of it suggests that a mini game must be implemented.

I first drafted a simple test level (which actually is the level at this stage of the progrect). Then I started to add C++ classes and corresponding Blueprints to add the functionality.

What is important to note is that the Bowling Game Kata is not solved in a minimal way, that is because this would exclude the use-cases that we need for a game. For example, only the ability to compute a score of the game (which suggests that it is a finished game) is not sufficient for the presentation of the scores and the score-so-far during the game.

The scoring logic actually is a state machine, which code could be improved. But due to time limits it is left the way it is now. I did not cheat by looking into already present solutions or using AI to draft them, as that would conflict with the idea of the assessment. For a real project I would of course use all available information that is available.

The Kata part of the code is found in the `BowlingGameMode.h` and `BowlingGameMode.cpp` files. To meet the requirements of the Kata the two functions are:

```C++
void Roll(int32 RollScore);
int32 Score();
```

### Todos
There is quite some further work to do to improve this game.
- Most disturbing now is the camera movement:
  The camera is connected to the ball (the Pawn) via a SpringArmComponent. Because the physics engine is used (which I just have used for the first time) this somehow breaks smooth camera movement. Also, when the parameters are optimized for shipping the game it does not work well in the editor and vice versa.
- Better gameplay experience:
  Adding start screen. Avoiding the new starts automatically and so on.
- Refactoring:
  - Better code isolation
  - Adding unit-tests (the tight integration of the score logic with engine code makes it hard to test them in isolation)
  - Adding (more) source code documentation
    On the other hand, the view of this depends very from team to team.
  - Fully check const-correctness
  - Changing all public member variables into private (or package) and using getters and setters to access them (now it is kind of mix).
  - Naming conventions:
    Because the real bowling games use the terms Roll and Frame, it is good to use them in code as well. However, sometimes this will be very confusing. For example, angles may be represented by (Pitch, Yaw, Roll), and the term Frame is higly overloaded for other things. Also, because the Kata asks to implement the Roll() function, the actually throwing of the ball should not be called Roll() because Roll() is reserved to adding only the scored pins per roll to the scoring system.
  - Minimize the shipping size of the executable package.
  - Using the Unreal's TObjectPtr<> instead of plain C++ pointers.
  - And probably many more.

## Miscellaneous notes

### Dimensions
For the design of the level I found the following dimensions on-line.

| Component | Metric Dimension | Notes |
| :--- | :--- | :--- |
| **Lane Length** (Foul Line to Headpin Center) | 18.29 m | Primary playing surface length. |
| **Lane Width** (Between Gutters) | 1.064 m | Width of the playable surface. |
| **Gutter Width** (Per side) | 0.235 m | Width of the channel adjacent to the lane. |
| **Pin Height** (Standard Tenpin) | 0.38 m | Height of a regulation pin. |
| **Pin Width** (Widest point) | 0.121 m | The maximum diameter of the pin. |
| **Pin Weight** | 1.53 kg min / 1.64 kg max | Regulation weight range. |
| **Pin Deck Spacing** (Center-to-Center) | 0.3048 m | Distance between the centers of adjacent pins. |
| **Ball Radius** (Max.) | 0.109 m | Calculated from max diameter. |
| **Ball Mass** | 4.54 kg min / 7.26 kg max | USBC regulation weight range. |
