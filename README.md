# Genesis Engine v0.2
  Genesis Engine is a game engine made in C and C++ for learning purposes. I'm Marc Pagès Francesch, 
  a Game Development Student in CITM (Terrassa, Catalonia, Spain).

  Engine GitHub page: https://github.com/marcpages2020/GenesisEngine
  Engine Page: https://marcpages2020.github.io/GenesisEngine/
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

### Shaders
 - To create a new shader right click on an empty space in the assets window and click on Create->Shader, write the name you want for your shader and click on Done.
 - To apply a shader to an object select the desired object and in the inspector drag the shader onto the button of the current shader. An optional way of applying a shader is selecting an object and dragging the shader onto the scene window. 
 - To edit a shader select an object which has it applied and click the button "Open shader editor". From here you can swap between the vertex and fragment shader. To save and compile the shader click on the compile button at the bottom of the editor. 
 - If a new uniform is added it will be added to the shader when the compile button is clicked. Then it will be available for editing, if an object with that shader is selected, in the uniforms section under the uniforms tab from the shader part in the inspector. To save their values click under the "Save uniforms" button at the bottom of the uniforms section.
- If a vec3 or vec4 uniform wants to be used as a color and wants to be edited more easily click the color checkbox next to the uniform in the inspector. Then on the right if you click the little square with the color a color selector panel will open. 
- For easer debug if an error is commited when editing the shader such as not writing a semicolon a red line will appear showing the exact line with the error or the following one. To know in which of the two shaders there is an error look that the vertex or fragment shader will turn red. If an error is not caused by a line but for an external reason it will be shown in the console. 
- For this demo some uniforms have been set in the code itself but if you want to edit the water shader from the inspector just add the word uniform before the variables you want to edit, compile the shader and they will appear in the inspector. 
- To enable mesh transparency in the inspector next to the enabled button there is a checkbox called blend. If it is ticked the mesh will have transparency enabled and if in the shader the alpha value of the out Fragment shader is under 1.0 the effect will be seen. 
- Some shaders might need all of their uniforms set in order to work properly.
- To import a shader dragging it onto the engine first drag the .frag and after the .vert. If you did it the other way round and for any reason the engine crashes delete the shader and the meta and open the engine again.  
- Unfiorms won't appear in the inspector until they hace a real effect on the code because OpenGL optimizes the code deleting the uniforms which are not used. 
- Some uniforms are provided by the code so you can use them in your shaders. Just declare them in the shader an they will be available for use.
	- model_matrix: global transform of the object (mat4).
	- projection: projection matrix of the camera (mat4).
	- view: view matrix of the camera (mat4).
	- time: time in seconds (float).
	- normalMatrix: matrix that transforms normals to world space (mat3).
	- cameraPosition: position of the camera in world coordinates (vec3). 
	- diffuseMap: diffuse texture of the material(sampler2D).
	- normalMap: normal map of the material(sampler2D).
	- diffuseMapTiling: tiling of the diffuse texture of the material (vec2).
	- normalMapTiling: iling of the normal map of the material (vec2).
	- diffuseColor: albedo color of the diffuse texture of the material (vec4). 

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
## Third assignment
- Debug errors on the shader editor screen.
- Gerstner waves for the water shader.
- Color editor in the uniforms editor. 