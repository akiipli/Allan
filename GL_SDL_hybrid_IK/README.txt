/*README*/

Author here is:
Allan Kiipli

7:57 PM 1/14/2020
Ctrl click in hierarchys List toggles selected state for locators.
Toggling collapsing selects item as current.

5:00 PM 12/22/2019
Shift + L while playing (ctrl + D). switches linear poses on and off.

4:34 PM 12/9/2019
Select ik goal and press f to make ik goal fixed.

7:43 PM 11/20/2019
IK development added.
Still need UI and removing. Alt + A adds IK chain in Bind Mode. With deformer selected, chains are added to
deformer, else they are independent in bones hierarchy. Such hierarchys need to be bound in Bind Mode and
chains are added to selected deformer.

1:23 PM 10/7/2019
Now pressing a in Bind Mode toggles Align Mode. This is useful when drawing bones or
changing their placement.

10:30 AM 7/6/2019
Enchancements to objects binding. 
Now only selected objects are initiated in deformers List by pressing b.
Also zero length Selections are automaticly deleted now.

9:27 PM 11/8/2018
If displacement map has inconsistency at uv seams, 
Shift + M can be used to fix it for current subdivision level.
However this change is not saved with images.

12:31 PM 11/7/2018
Displacement is saved. Use Alt + and Alt - to tune it for current object.
It uses bump map.
Shadows must be on (Alt+S) to see the effect.

Now Bump map and Normal map are loaded also with Windows dialog (pressing I), if bump or normal or image dialog was visited previously.

Bug fix: now all tangent maps are uv-based and uv-edge based.

11:36 AM 10/23/2018
Bug fix where poses were not allocated in deformer.

10:42 AM 10/22/2018
Now bone rotating in bind mode with Shift-S is supported.
Moving bone nodes around however creates alignement instead.

2:38 PM 9/13/2018
Bump maps are there. Press b when in uv_view (Shift-v).

Now normal maps are integrated. They can be loaded and saved with scene.
They become visible in shadows mode with shaders.
They need to be in tangent space and smooth normals (see ZBrush).

3:27 PM 8/25/2018
Shininess is saved. Use + and - to tune it for current object.
Shadows must be on (Alt+S) to see the effect.

In Selections window:
For Vertex selections:
If split groups are generated in current session (s after b in Deformers window),
pressing a selects vertex split group A and b B.

Pressing u, unassigns selected elements from all
element groups for selected objects.

In Deformers section:
pressing s after b generates
large vertex groups and smooth binding.

In Poses window:
Ctrl + up/down rearranges selected pose in deformer.
In window: Ctrl+d plays all poses for all deformers.

Latest update: Now saving and loading works.
Textures are loaded from image files, not saved.
Else poses and deformers come in.
Objects are prepared for subdivision surfaces.
Hit page up to subdivide them.
Added The MIT license.

As i go i add features and rewrite code.
There is no quarantee in a particular moment in time.

I release my code under 'LICENSE' conditions.
However, i use library files that may have different conditions.
Thats why, users need to accept them as well.

TO DO:
lots of.
First no saving:
To save a scene, a save system needs to be made.
Preferrably separate files for poses, selections, bones, deformers.
Ability to import them later one by one or all together.
Of course knowing what depends on what.

Shaders are not there:
All GL is compatibility mode.
It uses buffers for objects, but not shading programs.
Note also: no use of modelview matrix. It stays identity.
All vertex transformation is implemented to make up deformation system inside.
Therefore if shaders are made, no modelview matrix is to be used.

Future:
Add timeline.
Posebased animation, positions for root nodes in deformers.
Maybe schematics for shaders, if shaders were to be made.

General shortcuts:

o: open obj
i: open image
1: objects
2: polygons
3: edges
4: verts
5: bind mode
page up: increase subdivision level
page down: decrease subdivision level
l: show, hide locators. When deformer player is active, toggle Lightshow
g: move camera, mouse over
shift-g: move objects, move locators
ctrl-g: rotate camera (perspective)
s: scale object
shift-s: rotate object
ctrl-s: zoom camera
scroll: change field of view
f: flip camera view, if not turntable
ctrl-f: freeze object coordinates
ctrl-d: play deformers
alt-u: cycle light themes
alt-n: switch night theme
alt-s: switch shadows
f5: switch shaders (if supported)
v: toggle split viewport (under mouse)
shift-v: toggle uv view
shift-o: toggle ortho projection
shift-a: zoom selected, with locators center on selected
backspace: hide object
tabulator: cycle current object
delete: delete object, locator, bone
shift-t: cycle image texture in current object
c: if edges selected, convert to border verts, else convert to border edges
shift-e: change edge weights for subdivided selected edges (deforming objects), for weighted edges when vertexes are selected, change vertexes weights
shift-l: select edge, polygon loop
ctrl-c: convert into verts selection
shift-c: select connected
ctrl-x: grow selection
escape: drop selection, close dialogs
t: triangle view
e: toggle edges
a: toggle deformable object
ctrl-r: reset view
alt-r: reset scale
shift-r: reset rotation
r: reset position
n: with uv_view switch normal mode, else new locator
p: in bind mode, parent locators, else new pose for current deformer. When not first node in deformer is selected, relative pose is created from current position.
alt-d: select current Locators deformer. Do it before using p to place poses.
alt-e: switch element arrays
alt-c: in object mode set camera pose to remember
c: in object mode read camera pose set earlier
alt-b: not in bind pose set Bind Pose For Branch
ctrl-e: clear selected edges weights for selected objects
alt-f: cycle selected objects by framing them in camera view, esc to escape it.

in deformers window: b binds to current deformer selected objects in one go.
As result vertex groups are generated or updated.
After readjusting bones, update binding, however manual work is discarded.
To make better binding, place bones carefully and adjust locators size with scrolling, then press b in deformers window.
Size that fits geometry approximately, rejects far away vertexes and makes better binding.

When binding is there, clicking over bone entry in deformer, selects corresponding vertexes.

right-click cancels movement, rotation, scale
left-click confirms movement, rotation, scale

shift-d: open last dialog

in edit dialogs:
'`' - enter edit mode: change name of item, selection,
'enter', confirm changes, 'escape' cancel changes, 'backspace' - erase last changed letter.

in dialogs:

up and down: change selected item,
scroll mouse: scroll long lists up and down
with mouse click low right corner in dialog and resize it.

in Bind Mode:

b: add bone, starts new bone from selected locator, bone
5: exits bind mode and places default pose for deformers.

to change bones placement enter bind mode again,
use shift-g to move clicked bone,
use shift-alt-g to move with childs.
while moving you see updates on bones orientation and alignement,
moving away from set triangle, poses become invalid.

Its different when clicking with left mouse or click and drag,
for bones click and drag uses right id, click redirects selection to previous bones B transformer.

Exiting bind mode establishes new bind pose.

Selections can be changed and updated with draging right mouse rectangle over vertexes or polygons or edges,
also objects. This works also with Selections window open.
Ctrl-drag makes substraction from selected elements.

Assign and unassign vertexes from verts selections makes for bound selections weights normalization calculation
for involved bones.

7:25 PM 5/16/2018

Now Poses window works.
Shortcuts for Deformers window: 
a adds branch, b binds selected objects, d unbinds current object.

2:11 PM 6/4/2018

'p' in Poses window places rotational and scaled pose.
Up and Down arrow keys make animated transition.

-----

What is involved when developing new dialogs?
First off copy code in UserInterface.h for new dialog using all existing code with its callables.
See whats there and what should be changed.
In main.c dialogs come up and are invoked in draw_Dialog function.
Also behaviour in keypresses is directed through handle_dialog function.
Up and Down arrows and mouse scrolling is handled by handle_UP and handle_DOWN.

Dedicated dialog is opened with a open_Deformers_List type function, 
where 'Deformers' is replaced by your new dialog type.

First there may be create_Deformers_List function at hand. This function is stationed
preferrably in corresponding .h file.
Then blackout function ensures that only one entry, (last highlighted) is highlighted and all else is 'black'.

Then draw_Deformers_Dialog is called and is in UserInterface.h.

There at some point list_deformers is called and is in main.c

All these functions use GL functions with begin and end directives to draw orthogonal depthless UI.

Some considerations maybe:
Currently all lists are drawn with loaded courier font and always redrawn. This makes scrolling bit slow maybe.
Or when Poses are ainmated and dialog is open. Maybe solution is to make a full length list and store it as bitmap into
a framebuffer. Then when needed it only displays a portion of it with proper coordinates.
But this is theory, i dont know if blitting this way is really faster. I think its faster if dialog is large.

Finally, sidebar needs this dialog box in draw_UI_elements and new variables and updated definitions, 
and main.c needs functions associated with new buttons there.
Also SDL poll and mouse need proper blocks to handle everything.
Conclusion: Adding new entry into field of dialogs is a mount high task.
Easy to miss a variable and make hard to discover mistake.
Mistake is often made by quick-hook-and-later-forget type of coding.

12:54 PM 7/17/2018
Bind Pose bug fix:
Bones are now updated with transformers rotation.
Also when entering and exiting bind pose, deformers zero position is restored.