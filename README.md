# godot-smooth
Fixed timestep interpolation helper nodes for Godot, 3D and 2D

This a module for Godot engine (3.2 master, 19 July 2019 or later) giving two new node types, Smooth and Smooth2d.

https://www.youtube.com/watch?v=lWhHBAcH4sM

**Installation**
To install you will need to compile Godot from source:

http://docs.godotengine.org/en/3.0/development/compiling/index.html

To install the module create a new folder called 'smooth' in the modules folder of the engine, then clone / copy godot-smooth files into this folder. Compile the engine as usual, and the module will be automatically added.


**Instructions**
This is an example, your exact use will vary depending on your needs.

1. Lower the physics tick rate to 10 ticks per second (Project Settings/Physics/Common/Physics FPS)
2. Create a root node for the scene, either Spatial or Node2D.
3. Add a RigidBody (2d or 3d) as a child of the root node, with a MeshInstance to display something as a child of the RigidBody.
4. Run the scene such that the RigidBody drops according to gravity, moving in a jerky fashion 10 times per second.
5. Add a Smooth node into the scene as a second child of the root node, either Smooth, or Smooth2D depending on whether you are creating a 3d or 2d scene.
6. If you click the Inspector options for the Smooth node, you can set the target (for the Smooth node to follow) to be the RigidBody.
7. Now drag the MeshInstance so that instead of being a child of the RigidBody directly, it is a child of the SmoothNode.
8. If you now run the scene, you should now see the mesh smoothly following the RigidBody, instead of moving in a jerky fashion.

**Notes**
You can switch on and off interpolation for different components. You can change the input and output coordinate system of the smoothing node. In most cases you would use the default local space.

For the most part this is all that needs to be done, the target (whether it be a RigidBody, KinematicBody, or simple Spatial or Node2D) can be moved as usual. The one exception is in the case where you are placing an object for example at the start of a level. To prevent it interpolating from the previous position, you should call the 'teleport' function on the Smoothing node, after setting the position of the target node.
