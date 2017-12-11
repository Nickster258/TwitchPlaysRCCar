# TwitchPlaysRCCar
When given the task to connect an RC car to the internet, I was dumbfounded. I had no idea what I could do with that. I brainstormed for a little bit and realised I could try to control it over a certain medium. I could create my own NodeJS web service to do this, but that is just too much and not as fun let alone the time needed for completing that. Then I was reminded of the TwitchPlays channel, where people control characters in video games to try and complete the game. What about control an RC car?

This project was made much easier when I searched into just how Twitch has their chat protocol setup. As it turns out, they use the IRC protocol to communicate between clients. [How convenient](https://github.com/Nickster258/Lakshmi)! So I began work on freshening up my IRC protocol hat and began working on this task.

### Commands
Basic commands are as follows:
* `'move` - Moves in the specified direction.
* `'turn` - Turns in the specified direction.
* `'ping` - Blinks the LED on the RC car.

Commands `'move` and `'turn` have subcommands:
* `forwards` and `back` for move.
* `left` and `right` for turn.

Following the specified subcommand, an integer from `1` to `9` needs to be provided. Here are some example commands:
* `'move forward 3` - Moves forward for 3/4 seconds.
* `'turn right 5`   - Turns right for 5/4 seconds.
* `'move back 2`    - Moves back for 2/4 seconds.
* `'turn left 8`    - Turns left for 8/4 seconds.
* `'ping`           - Blinks the light on the car to test connection.

### Demonstration
[![TwitchPlaysRCCar](https://img.youtube.com/vi/-bFvqEjGWtY/0.jpg)](https://www.youtube.com/watch?v=-bFvqEjGWtY)

#### A note
This is for a final project in Intro to Computer Systems I. I very much enjoyed making this device and appreciated the challenges I overcame in finishing it.
