# Project #2: Pacman 3D [3D Game Project]
### Group Members:

- Berna Yıldıran
- Kamil Atakan Çelikyürek

### Game Description

In this project, we will demonstrate the Pacman game in 3D. Our initial plan is to make a black world, and blue surroundings, as is the case for the original Pacman. Moreover, the rules of our Pacman will also be similar to the original Pacman, some rules must be different as the game is in 3D, e.g., rotations from the corners. Also, the viewpoint will be such that the player can see his/her front side, i.e., where the controlled creature is going, to increase the player’s experience. For the advanced features, our initial plan is to incorporate the features such as; 

- **On-screen control panels (HUD)** will be added to show the data related to the current state of the user within the game. The implemented functionalities are ***mini-map, score board,*** and ***remaining lives (3D object mapped as texture)**.*
- **Collision detection** is required for restricting Pacman’s movement. Since Pacman won’t be able to pass through walls and must follow the roads between the walls. ***Bounding Box*** algorithm is used for implementing collision detection.
- **Artificial intelligence** is required for managing the movements of the monsters with respect to some rules. ***Dijkstra Shortest Path*** algorithm is used for implementing the AI functionality.
- **Shadow mapping** is required  to provide a pleasant and high-quality game play experience for the user.

### How to Play

After launching the game, when any of the arrow buttons is pressed, the game starts.

Before the game starts, you can click 1, 2, and 3 buttons for choosing the difficulty as easy, normal and hard respectively.

Throughout the game, you have three lives. If you catched by a monster, the game restarts from the initial positions of the characters.

If you are out of lives, the game is over and screen will be closed. 

To win the game you need to collect all the light bulbs.

### 3D Models of the Pacman 3D
![Untitled](https://user-images.githubusercontent.com/78401458/214561225-b9d4f0d4-ba76-457d-99ea-89333cf933c4.png)
![Untitled1](https://user-images.githubusercontent.com/78401458/214561257-44a95f86-9e5a-4a95-a077-02b9ee6ce006.png)
![Untitled2](https://user-images.githubusercontent.com/78401458/214561281-7f705247-681f-4e51-bcdf-69cd5206250a.png)

### Screenshots from the Implemented Game
![Untitled3](https://user-images.githubusercontent.com/78401458/214561365-41425253-498d-4934-87de-67d89dd355fc.png)
![Untitled4](https://user-images.githubusercontent.com/78401458/214561368-2f961cfc-3530-436a-a83d-8a49d4338551.png)

### References
**OpenGl:** [https://www.opengl-tutorial.org/](https://www.opengl-tutorial.org/)
**OpenGl:** [https://learnopengl.com/](https://learnopengl.com/)
