# Maze-Game
Course Project for COP290 Design Practices, IIT Delhi 2022 

Group Members 

1. Tanish Tuteja (2020CS10398)
2. Chinmay Mittal (2020CS10336)

---

### Installation Instructions

We have tested the game on Ubuntu (20.04.4 LTS)

Ensure that g++ is installed on your system

On Linux, install the following libraries with the appropriate commands 

1. SDL

```
 sudo apt-get install libsdl2-dev
```

2. SDL_image

```
sudo apt-get install libsdl2-image-dev
```

3. SDL_ttf
```
sudo apt-get intstall libsdl2-ttf-dev
```

4. SDL_mixer
```
sudo apt-get intstall libsdl2-mixer-dev
```

Clone our repository on your system using the following command

```
git clone https://github.com/ChinmayMittal/Maze-Game.git
```

Compile the code using the following command
```
make
```

To setup the server for the game we need the IP address of the host. To get the IP address, use the following command

```
hostname -I
```

On one terminal, run the server using the following command

```
./server/server.out
```
Then run the game on two different PCs connected to the same network as the server, by executing the following command
```
./game.out 10.184.24.96
```

here 10.184.24.96 is the IP address of the server found using hostname -I

----
### Resources Used

1. [ Lazy Foo Tutorials ](https://lazyfoo.net/tutorials/SDL/)
2. [ GeeksForGeeks Sockets & Networking ](https://www.geeksforgeeks.org/udp-server-client-implementation-c/)
3. [ SDL Documentation ](https://wiki.libsdl.org/)