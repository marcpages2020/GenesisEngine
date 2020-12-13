# Genesis Engine v0.2
  Genesis Engine is a game engine made in C and C++ for learning purposes. I'm Marc Pagès Francesch, 
  a Game Development Student in CITM (Terrassa, Catalonia, Spain).

  Engine GitHub page: https://github.com/marcpages2020/GenesisEngine
  My personal GitHub profile: https://github.com/marcpages2020

## Controls: 
 
  To freely move the camera in the scene, the scene window must be selected or focused.

### Camera Controls:

 - W / Up Arrow: Move Forward
 - A / Left Arrow: Move Left
 - S / Down Arrow: Move Backwards
 - D / Right Arrow: Move Right

 - Mouse Wheel: Zoom In/Out
 - Middle Mouse: Drag to move horizontally and vertically. 
 - Right Click: Orbit around the selected object. (If an object is not selected it will orbit around the center of the scene).
 - F: Focus the camera around the selected object.(If an object is not selected it will focus around the center of the scene).
 - O: Move Up.
 - L: Move Down.
 - Hold Shift: Duplicate movement speed.
 
### Game Objects Inspector actions
 - Transform: Translate, Rotate and Scale the object. 
 - Mesh: Toggle the view of the vertex and face normals. 
 - Material: 
     - Toggle the checkers image. 
     - Remove the current texture. 

### Actions in tabs:
 - Windows: Enable the engine windows. 
 - Edit: 
   - Configuration: Change the engine settings and view graphs about the performance. 

 - Game Object: There are six primitives available to be created at any time. These are: Cube, Plane, Cylinder, Sphere,
   Cone and Pyramid. These are duplicated, the firsts are generated automatically and have no texture coordinates and the 
   ones marked with FBX will be loaded from FBX files and will have texture coordinates. 

 - About: Get the pages where the project is held in GitHub and get more information about the engine.

### Scene Saving
 - Scenes can be saved and loaded from the editor main bar. Even though they can be saved anywhere it is recommended to do so in the scenes folder to keep everything organised. Select the desired folder and write the name of the scene with no extension, it will be added automatically. If an existing scene is selected it will be overwritten. 

### Importing Files
 For proper file import it is recommended that the files are already inside the Assets folder before starting the engine. If they are not, files can be dragged and dropped onto the engine in order to be imported. One last way to import files is copying them in the Assets folder and push the Reload button under the Assets hierarchy in the Assets window. Even though fbx files can be imported directly dropping them onto the engine it is recommended that for textures they are first copied in a folder inside assets and then dragged onto the engine to acces the importing options. 

 ### Importing Options
There are plenty of importing options specially for textures. When the file is dropped onto the engine an importing window will pop up to select the desired options. The only model format supported is fbx so if a warning or error message is displayed saying that a file from a different format can't be imported, this is the reason. 

### Camera Culling
 - The camera in the center of the screen is the main camera. Move it to see how camera culling is applied to objects. In the menu bar editor camera culling can be applied to see all objects in scene. 

### Things to take into account
 - At the end of the inspector window there is a ab called Resources. In this tab it is shown all the resources which are currently loaded into memory. If numbers seem to not match for textures take into account that assets window icon textures and preview textures are included in the list.

 ## Additional functionality

 ### First Assignment
 - The engine settings are loaded directly from a JSON file named config and located in GenesisEngine/Assets/Config. 
   this settings include the modules values and which windows are opened on start. 
 - When and FBX is loaded the hierarchy of the objects is mantained and the name of the objects in the FBX will be assigned
   to the new Game Object.  
 - Objects can be deleted selecting them and pressing SUPR.  

## Second Assignment
 - Scenes can be saved with custom name and overwritten by just clicking on them.
 - Textures can be previewed in the assets window. 
 - Interactive path in assets window which lets the user move easilly bacwards in the file or folder path. 
 - Meshes can be selected individually from inside a model pushing the arrow next to the file in the assets window. 
 - Inside the configuration window there is an option under the resources tab which lets you delete all meta files and library files at shutdown so projects can be cleaned before releases or sending to mates. 