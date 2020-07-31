# ENSC482-Decision-Making
Introduction to Decision Making in Engineering - 2020 Summer

##### Development Environment
GLFW version: 3.3.2 (precompiled binaries)
GLEW version: 2.1.0 (precompiled binaries)
glm version: 0.9.9.8 (header file only)
FreeType version: 2.10.2 (compile on my own from visual studio 2019)
Visual Studio 2019 Community Version with x64

##### Extension (Optional):
Install Markdown Editor Extension in the Visual Studio to modify/preview the markdown file.

##### Instruction Setup:

glfw, glew setup:
Follow this youtube: https://www.youtube.com/watch?v=gCkcP0GcCe0

glm setup:
1. Download zip file from https://github.com/g-truc/glm/tags
2. Extract it to any place as a folder "glm/".
3. In visual studio, right click the project, go to properties, go to C++, and then General. Then, edit the Additional Include Directories and add the path to the glm folder "YOUR_SOME_PATH/glm".

FreeType setup:
1. Download .tar.gz file from https://download.savannah.gnu.org/releases/freetype/
2. Extract it to any place as a folder "FreeType/". Name of the folder can be anything.
3. Compile the library: Go to freetype directory, open the solution file in builds/windows/vc2010 with Visual Studio. Choose Release and x64 (if you are using 64 bit windows) and build it to get *.lib files.
4. In visual studio, right click the project, go to properties, go to C++, and then General. Then, edit the Additional Include Directories and add the path to the glm folder "YOUR_SOME_PATH/FreeType/include".
5. Continuing in Visual Studio, go to Linker, and then General. Then, edit the Additional Library Directories and add the path to the FreeType folder "YOUR_SOME_PATH/FreeType/objs/x64/Release".
6. Then go to Linker input, edit the Additional Dependencies, and add "freetype.lib". Make sure to check the library name from the built path "YOUR_SOME_PATH/FreeType/objs/x64/Release" since the library could be different for different version.

##### Instruction Help:
GLFW and GLEW: https://www.youtube.com/watch?v=gCkcP0GcCe0
glm: https://www.youtube.com/watch?v=6nGpoY-L2Xk
FreeType: https://medium.com/@jackklpan/using-freetype-in-visual-studio-3ac321ca4cb9
