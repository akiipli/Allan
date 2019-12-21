# Allan
Place GL_SDL_hybrid to c drive.
Place Allan to your windows c drive.
Place SDL-1.2.15 to your c drive too (developing).
Copy runtime librarys to Windows/SysWOW64.

Place Trips Code to c drive.

Exe files are in C:\GL_SDL_hybrid\bin\Release.
Guidelines are in C:\GL_SDL_hybrid\README.txt.

How to use.

One can set up scenes in 3d via importing low res OBJ files.
Also make decent coherent UV maps for them (less uv seams).
This is because element arrays use uv arrays to draw geometry.
In Binding mode one can draw bones via pressing 'b' and drag and click in view.
Then in Deformation panel make sure Meshes you are binding are selected (via Item panel).
And press 'b' to bind, 's' to smooth bind. Use vertex selections panel to
fine tune results. Use 'l' to switch locators. Click-drag to special select locators.
Esc to terminate selections.
Use Ctrl-d to play animation, esc to stop it. Use 'p' to place poses for selected deformers.
Ctrl-up and down in pose panel rearrange placed poses.
After all that place textures, bump maps and normal maps.
To view bump and normal maps use Alt-s to turn on shadows and shading.
Use Alt-+ to make displacement pop up or down. Displacement is bound to bump heightmap.
Also to bring up windows file selectors use 'o' for obj files and 'i' for images.
Images importing is affected by the last O. image menu click.
Thus if 'O. bump image' was clicked, bump map is imported.

---

How to set up development with CodeBlocks.

Assume you did all the above.
Check CodeBlocksLinker.txt for lines to set up Trips Code project in your Code Blocks development environment.
Also these lines can be project based, if you develop something else and wish not mess up primary settings.
To do it right click on project and find target options.

---

IK folder has IK development in it.
To test out IK, select A and B transformers in bone chain. It may have lots of bones and more complicated shape.
Press Alt+A. Gray line appears. You must be in Bind Mode. Exit Bind Mode and select point B, press Shift+G to move
around, right-click to cancel movement.
Submitted into Trips Code folder Scenes IK a1 to serve as testfile. This file opens only in IK project.

---

To fix or unfix a IK goal, press f.

---

How to update old files. Backup loader file. Use old specification to load scenes and save with new specification.
Then replace loader with new loader. And compile again.
