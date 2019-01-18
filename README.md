# Taboo
Mandy Mai

## Description
A game of taboo where 5 players connect to the game server in multiple terminals. Every turn, one player is the designated "hinter" who has a list of taboo words they can't use to get the others to guess correctly. Each correct guesser receives a point and the hinter gets a point for every correct guesser. The player who gets 10 points (if multiple people, the one with the most points) wins and the game ends.

## To Start Game:
1. open 6 terminals
2. make and ./server in one
3. ./client in the other 5

## DEVLOG
###### Jan 2
Created the necessary files and came up with a networking diagram of the program to start working from.
###### Jan 4
Moved over networking and socket-related files posted by Mr. K. Modified to fit the format of my idea.
###### Jan 11
Got the server to state whether the client guessed right or wrong and give points if correct (from a set of words in server).
###### Jan 15
Clients wait until there are five players before starting the game.
Have to get server to establish turns and assign the hinter.INCOMPLETE

###### Jan 18
