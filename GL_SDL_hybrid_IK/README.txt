/*README*/

Author here is:
Allan Kiipli

10:32 AM 9/1/2022
Left and Right Arrows advance currentFrame when Timeline is on. Alt + T switch Timeline on and off.

10:48 PM 8/30/2022
Timeline UI. Alt + T to switch Timeline on. Lists Deformers keyframes based on 0th transformer. Inserting keyframes
is possible with Poses tab open at current keyframe. This can be changed with clicking on Timeline. With Ctrl + I.
Deleting keyframe without tab Alt + I.

3:14 PM 8/30/2022
Timeline introduction. Timeline is integrated into transformer. Loading and saving for keyframes. Ctrl + I places
preset keyframes for current Deformer in Poses tab. Close Poses tab and press Alt + D to play back once.
Issues: placing keyframes with rotated deformer may yield unwanted results. Rotated deformer means that transformer 0 is
rotated.

12:32 PM 1/13/2022
Rendering gone to pthreads. 8 threads speed up testing.

8:18 PM 1/5/2022
Rendering revisited. Pressing F12 starts rendering based on current dimensions. Based on perspective viewtype.

4:46 PM 12/14/2021
Ctrl + A selects All.

8:00 PM 12/12/2021
In Bind Mode pressing Enter places drawed bones selected locators into current mesh along viewport vector.

12:24 PM 4/4/2021
Objects smoothness introduced. Switch off element Arrays to see the effect. Also non smooth objects do not conform
subdivided shape.

4:53 PM 2/28/2021
In Curve mode p switches selected edges patch mode even without curves
Ctrl+E clears edges patch flag in edges mode while in Curve mode. This is also saved.

3:37 PM 2/26/2021
Alt+P switches for patch Mode

4:14 PM 2/25/2021
Pressing up in Edge mode with two consecutive edges selected at least, adds next edge to the row.

9:00 PM 2/24/2021
About converting edge selection into border vertex selection. Pressing C in edge mode converts selected edges into
border selection. It is useful for placing curves. First select poly islands that do not border with existing curves
in object. Press C. This creates edge loops. From these loops vertex selection is created with C. 
Press Ctrl+P in succession to place curves for these edgeloops. When edges are selected, curve placing function
takes this selection as quide. Vertex ordered selection can be used without edges selected. Also two point curve is
valid.

10:24 PM 2/21/2021
Now Shift+E in curves Mode starts curves weights editing when not in vertex Mode.
In vertex Mode it starts weight editing for selected verts.
Ctrl+E erases weights.

12:15 PM 2/15/2021
In curves Mode one can dial up subdivision curve parameter with Ctrl and mousescroll. This is visible
with subdivided objects that have curves. With Alt scrolling one can change edge divisor parameter.
This is experimental now, because it does not yield proper surface. Fanatics could improve it, i not.

10:55 AM 2/11/2021
In curves Mode pressing Ctrl + F transfers Curves Cps coordinates to objects original coordinates.

11:12 PM 2/4/2021
Steps taken to integrate curves.
Pick verts in order and press Ctrl + P. This creates a curve for the current object. It must connect edgeflow.
Further one can manipulate cps, scale selected curves, cps and objects subdivided shape should follow.

Also independent curves can be drawn that dont belong to the objects. This can be done in Curves Mode pressing A
and moving with mouse. Then clikcing.

6:44 PM 1/4/2021
IK constraints and poles. When adding pole to IK chain in Bind Mode, move pole to desired location to make up
IK spine, else spine is undefined when solving outside Bind Mode.

IK constraints must be used with IK goal set to fixed and Stretching in IK List is off.
In addition rotation can be controlled with pin. This is set in Hierarchys List.

7:55 PM 12/31/2020
Multirotation is there. Also scale. Multirotation is flexible and works also in Bind Mode.
It affects Bone A transformers. Also it is constrained to Deformer transformers. Select
multiple transformers and finally select and highlight current transformer in yellow.
Then down the hierarchy from current, rotation and scaling is propagated. Also reset
function uses same concept to reset either rotation or scale.

11:18 AM 12/30/2020
About versions. Save scene to a new file, if program save version is changed.
Else corruption may happen.

3:49 PM 12/18/2020
When creating Subcharacter poses, make sure there is no fixed IK chains, 
that modify the orientations in the limb. Else poses come at an unnatural angle.

3:04 PM 12/15/2020
Updated subcharacter pose applying mechanisms. Now hierarchy is affected after subcharacter.
Avoid subcharacters that are a partial IK chain. Then IK targets may stay afloat after pose apply.

9:44 PM 12/7/2020
IK uses now updated solver. IK spine is updated in IK operations. Fixing and Unfixing do not use
matrix updates to maintain constant pose for the rest of the chain. Deformer player and poses
do not fix or unfix IK goals.

12:14 AM 12/4/2020
Massive work is done with Subcharacters, integrating them into future pose based animation system.
Pressing Enter or Return in Subcharacters List applies current subcharacter pose. Both need to be
current. It is possible to apply pose that is not this subcharacter member, but it needs to have
same number of transformers in it.

10:36 PM 12/2/2020
Pressing c in Hierarchy list, cleans transformers that have no connection to objects they refer to.
Its a sanity check.

11:37 AM 11/4/2020
Enter and return are different now for selection renaming.
Use enter to assign while renaming.

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
5: bones
6: bind mode
ctrl-f in curve mode freezes vert coordinates to cp positions
ctrl-e in curve mode and object mode clears selected curves weight
ctrl-e in curve mode and vertex mode clears cp weights in selected curves
ctrl-e in curve mode and edges mode clears segment weights in selected curves
shift-e in curve mode starts cp weight adjustment 
ctrl-c in curve mode converts curve to cp selection
shift-c in curve mode toggles curve drawing on/off.
alt-a in bind pose adds ik chain for selected transformers.
a: in curve mode. Adds curve and starts mouse drag for last cp. Click to place it.
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
ctrl-a: toggle objects deformer on/off.

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