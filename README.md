# Polygon Decomposition
J Fernandez, L Canovas, B Pelegrin  
Algorithms for the decomposition of a polygon into convex polygons,  
European Journal of Operational Research,  
Volume 121, Issue 2,  
2000,  
Pages 330-342,  
ISSN 0377-2217,  
https://doi.org/10.1016/S0377-2217(99)00033-8.  

## Team Details
- 1 Member  
- Name: Kotha Rohit Reddy  
- ID: 2020A7PS1890H  

## Build Instructions
- Tested on `Windows` using `Visual Studio` with `C++ Desktop Environment`
- Freeglut library is statically linked for the visualizer to run
- All additional include directories and libraries can be found in the `dependencies` folder
- Build the project with visual studio and press F5 to debug the application within visual studio itself
- First a blank window appears in which you are expected to draw a convex polygon with left clicks
- When you are done right click on the screen and then the polygon automatically closes itself with the last edge joining first
- After completing, close the window.
- Now in a while, the input will be processed and the decomposed polygon is shown on the screen on a new window
- The output is also written to a text file

Here is a sample output from the application

![sample](Resources/sample.png)

## Visualizer
- The Visualizer was made using freeglut
- I was able to test the algorithm on only small test cases with vertices from 20-30.
- The visualizer I had made it easy to test on some edge cases and correct my implementation
- The Test Polygons were drawn in my visulaizers and immediately checked
- Note that You can run the decomposer either by drawing it in the visualizer app or by giving a file with the data as input

## Design Discussion
- I faced some trouble for adding an edge into my DCEL data structure
- The way I traverse my polygon is by traversing the edges within the same incident face
- When I add an Edge, the face associations change due to addition of a new face and a removal of a part of the old
- Because of this I was able to find that I was just looping through the same face and not able to go forward
- Initially I tried to reference the vertices to join to add an edge
- But It was better to reference the edges to determine which vertices to join (the edges origin)
- By using edges, it becomes simpler to change the face associations while adding an edge because a single vertex might be associated with many faces and edges
- So to get the face or edge I need from the vertex would be difficult
- But in an edge only one face and one vertex is associated, hence I decided to use the edges to traverse the polygon instead of vertices
- The add edge function in my DCEL structure only proceeds if two of the vertices to be joined belong to the same face