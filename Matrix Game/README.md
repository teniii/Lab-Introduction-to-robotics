# Matrix Game - Connect Dots


### Description
#### The menu
The game will welome you with a 5 second message, in which you can see the name of the game.
After that, a menu will appear. There you can find the Play, Highscore, Settings and Info options. All the actions in the menus are made with the first joystick, the one on the left of the lcd (the first one).
To toggle between them, you move the joystick left or right and to select the option indicated by the arrow, you move the joystick up or down.
To get out of them, you make the same action, moving the joystick up or down.

The only special tab is the Settings one, which has 2 more submenus, one for the matrix brightness and one for the difficulty.
To go deeper, you move the first joystick down, to go out, you move the first joystick down. 

In the Highscore tab you can find the maximum number of winnings that someone ever done in a session (a session is the period of time since the game started until it is unplugged or restarted)

In the Info tab you can find the game's name, the author, for which course I made it and the github link.

#### The game

In the game you have to connect at least 4 dots vertically, horizontally or obliquely before your opponent does it. The playing space is a 8 * 7 matrix and to select the right collumn, you have the first row reserved. The difficulty, the turn and the score will be shown on the LCD.

For the adventurers that try the hard mode, on the LCD will appear a timer too. If the timer reaches the last 3 seconds, a buzzer will start to alarm you that your turn is almost done. If you don't make the move before the timer runs out, the arduino will drop the piece on the last collumn that you've selected. When a player wins a game, the score wil be updated and a Congratulations message will be shown on the LCD. 



### Requirments fulfilled:
 
#### 1.Hardware: 
a. 8x8 LED matrix
b. LCD
c. Joystick * 2
d. MAX7219 Driver
e. Potentiometer
f. Buzzer

#### 2.Game specifications:
Score: best out of 3 between players and the highscore is the maximum number 
of winnings made by a player in a session of playing.
Progress: Easy and hard, with 2 players and time pressure and save-able 
highscore in EEPROM

#### 3.Lcd display: 
The menu will include Play, Highscore, Settings and Info options



### Link demo:
https://www.youtube.com/watch?v=shDk1exea2k
