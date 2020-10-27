# Genesis Engine
 Genesis Engine is a game engine made in C and C++ for learning purposes. I'm Marc Pagès Francesch, 
 a Game Development Student in CITM (Terrassa, Catalonia, Spain).

 Engine GitHub page: https://github.com/marcpages2020/GenesisEngine
 My personal GitHub profile: https://github.com/marcpages2020

## Controls: 
 
 to freely move the camera in the scene the scene window must be selected or focused.

### Camera Controls:

 - W / Up Arrow: Move Forward
 - A / Left Arrow: Move Left
 - S / Down Arrow: Move Backwards
 - D / Right Arrow: Move Right

 - Mouse Wheel: Zoom In/Out
 - Alt + Left Click: Orbit around the selected object. (If an object is not selected it will orbit around the center
   of the scene).
 - F: Focus the camera around the selected object.(If an object is not selected it will focus around the center
   of the scene).
 - Hold Shift: Duplicate movement speed.

## Engine Functionality

 FBX models can be imported dragging and dropping them directly onto the engine. Textures can also be imported doing 
 the same and if an object is selected it will be automatically applied to it. 
 
### Game Objects Inspector actions
 - Transform: Translate, Rotate and Scale the object.
 - Mesh: Toggle the view of the vertex and face normals. 
 - Material: Toggle the checkers image.  

### Actions in tabs:
 - Windows: Enable the engine windows. 
 - Edit: 
   - Configuration: Change the engine settings and view graphs about the performance. 

 - Game Object: There are six primitives available to be created at any time. These are: Cube, Plane, Cylinder, Sphere,
   Cone and Pyramid.  

 - About: Get the pages where the project is held in GitHub and get more information about the engine.

 ## Additional functionality
 - The engine settings are loaded directly from a JSON file named config and located in Engine/Assets/Config. 
   this settings include the modules values and which windows are opened on start. 
 - When and FBX is loaded the hierarchy of the objects is mantained and the name of the objects in the FBX will be assigned
   to the new Game Object.  
 - Objects can be deleted selecting them and pressing SUPR.  
 