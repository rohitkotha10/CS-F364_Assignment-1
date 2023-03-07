# Polygon Decomposition
J Fernandez, L Canovas, B Pelegrin  
Algorithms for the decomposition of a polygon into convex polygons,  
European Journal of Operational Research,  
Volume 121, Issue 2,  
2000,  
Pages 330-342,  
ISSN 0377-2217,  
https://doi.org/10.1016/S0377-2217(99)00033-8.  

## Build Instructions
- Tested on `Windows` using `Visual Studio` with `C++ Desktop Environment`
- freeglut library is statically linked for the visualizer to run
- all additional include directories and libraries can be found in the `dependencies` folder
- build the project with visual studio and press F5 to debug the application within visual studio itself
- first a blank window appears in which you are expected to draw a convex polygon with left clicks
- when you are done right click on the screen and then the polygon automatically closes
- after closing the polygon, you may exit the window
- now in a while, the input will be processed and the decomposed polygon is shown on the screen on a new window
- the output is also written to a text file

Here is a sample output from the application

![sample](Resources/sample.png)