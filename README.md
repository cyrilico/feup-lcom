# "To Infinity and Beyond!": A retro-style game in Minix 3
This repository holds all the code made made for a Computer Labs (LCOM) class @ MIEIC, FEUP. It was made in collaboration with <a href="https://github.com/antonioalmeida">@antonioalmeida</a>.

The first set of folders correspond to mini-labs where we learnt how to interact and manipulate some common peripherals via the MINIX 3 operating system. These include the *internal timer*, *Real Time Clock (RTC)*, *keyboard*, *mouse* and *graphics card*. The ultimate goal was to develop a project that incorporated as many peripherals as possible.

Our project then consisted of a simple retro-style infinite 2D shooter with Buzz Lightyear as our main character. Please refer to our [**report**](https://github.com/cyrilico/to-infinity-and-beyond-minix/blob/master/proj/doc/report/Project%20Report.pdf) for complete implementation details.

![Main Menu](https://i.imgur.com/XVLDUAP.png)

The enemies are represented by squared boxes with different number signaling their difficulty, i.e., the number of shots needed to break them. Collision detection forces the player down the screen in case of contact, losing the game ultimately if reaching the bottom. The game screen also shows the current score, current ammo and bonus (bonuses are generated periodically and give the player extra abilities, such as double damage or invincibility).

![In-game](https://i.imgur.com/7KFsGLJ.png)
