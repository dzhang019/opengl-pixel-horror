## opengl-pixel-horror
My attempt at creating a mini first person pixel horror game with opengl. Submitted as my final project for COMS4160 Computer Graphics @ Columbia. The project was bootstrapped from modifying previous my assignments, including coms-4160-shader-pipeline and coms-4160-meshes.

The project is a custom-built 3D graphics engine written in C++ and OpenGL and a way for me to explore advanced rendering techniques and procedural generation. The engine features a programmable rendering pipeline that supports importing complex obj meshes and managing material properties. The camera system, which was originally designed for object inspection, has been modified to navigate the scene in a first person perspective, allowing users to explore the enviornment.

The engine combines realistic and stylized elements by using Blinn-Phong shading modified by a screen-space ordered dithering effect.

The landscape is procedurally generated instead of a predefined static mesh. The engine uses Tessellation Shaders to dynamically subdivide a coarse plane into a high-resolution grid on the fly (avoiding preloading a heavy static mesh). This grid is then deformed using 3D Perlin Noise to create organic, rolling hills, with surface normals recalculated in real-time for accurate lighting. Additionally, custom micro-bump mapping algorithm in the fragment shader to simulate the granular texture of dirt/grass. 


<img src="coms4160finalprojectdemo.gif" alt="Alt Text" width="1200" />

