# COMP3016CW2 
## CW2 OPENGL 

  
### This is a C++ OpenGL 4 project, loading models of different file types which use vertex paint. This program loads 3 various models, an OBJ, FBX, and a DAE. These models use vertex paint. OBJ loads in the center, the others around it, their vertices are then combined. a timed animation of rotation is applied, showcasing the OBJ sword, while the others rotate around it. a dynamic white light pulses up and down while a green flows left to right.  
### Given the opportunity, I would like to add a way to load in and replace models at run time to add to the program and get it to where i wanted it to be. 
### I started with nothing, from scratch, went through learnopengl.com to learn and find out how to go about my project, for inspiration and knowledge. 
 

## Controls 
### The program uses basic WASD movement, W for forward, A for left, D for right and S for back, along with mouse camera controls. 

  
## Exe 
### To execute the program, please navigate to OpenGLComp3016/x64/Debug directory. 
### In here, find the OpenGLComp3016 application executable. Double click on this and it shall run. 
### I have provided the necessary libraries for this project. These need to be placed in C:\Users\Public\OpenGL. 


## YouTube 
### [COMP306 CW2 VIDEO](https://youtu.be/Xc6uKByWTEM) 

## GitHub 
### [Git Repo](https://github.com/KacperZmu/COMP3016CW2) 
 

## Inspiration 
### The main bit of inspiration behind creating this, is various videos showcasing 3D models, which I love. This is not a modelling course, so the models are not of high quality, but the idea behind the program is to showcase different models. 

  
## Program architecture 
### The program focuses around loading models, therefore it is designed around these key components: 

- Model loader: this class focuses on loading the models. It provides code for errors, Normals and colours, making sure they load properly. this goes for each type, OBJ, FBX and DAE 

- Shaders: this class loads the shaders from main cpp, taking the source code, compiling, checking for linking errors, ID, using, destroying and attaching. 

  

## Unique Features 
### what makes the project stand out is the way it displays the models. Three models are loaded, OBJ in the middle, other 2 around it. their vertices are then combined. This allows for a timed animation of spinning around the OBJ, showcasing the models. should another model be made with vertex paint, just adjust the path within the code to display and showcase. 
### secondly is the lighting. There are 2 sets of dynamic lights that look like they fade in and out. a white light and a green. These lights are placed in different places, with the white light moving up and down and the green moving left to right. First comes the white light showcasing the model's vertex colours, which are then overlapped by the green light. 
  

## Dependencies 

- GLFW 

- GLEW 

- GLM 

- ASSIMP 

- CMATH 

- IOSTREAM 

- VECTOR 

- STB_IMAGE 

- STRING 


## Signature 
### for my signature i used vertex paint to apply KZ to the DAE model, as my initials. All models used are my own creations. 

  
## Research 
- [Learn OpenGL](https://learnopengl.com/) 
- [glProgramming](https://www.glprogramming.com/red/chapter05.html) 
- [Assimp](https://assimp-docs.readthedocs.io/en/latest/) 

### next are a few for Procedural generation which is something i would have like to include to generate a terrain for my program if I had the time. 
- [OpenGL-Notes](https://opengl-notes.readthedocs.io/en/latest/topics/complex-objects/procedural-generation.html) 
- [Noise](https://www.omardelarosa.com/posts/2020/01/06/infinity-terrain-in-c++-using-perlin-noise-and-opengl) 


