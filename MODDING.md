# Modding Feed Snivy

Want to add characters or modify existing ones or mod Feed Snivy in general or otherwise want to know how the game works? Here's how.

# Animation Format

Feed Snivy uses a semi-proprietary format called ".anm2" for tweened character animations, sourced from a spritesheet. This file format comes from the game _The Binding of Isaac: Rebirth_ (a game I mod on my own time) for animations. 

You can either use my own animation editor for that game and this one (recommended), [Anm2Ed](https://github.com/ShweetsStuff/anm2ed), or if you have that game on Steam, you can find that game's own proprietary animation editor in that game's Steam folder and then in tools/IsaacAnimationEditor. I can't guarantee the stability or perfect efficacy of either, but Anm2Ed has crash mitigations and autosave (but of course, save often, for whatever you do).

Anm2Ed has slightly more extensions to the base format; in particular, something called "Regions", which allows you to set spritesheet regions (areas in the spritesheet that can be repurposed) to quickly reference in animations; previously, one would have to manually create a perfectly spaced spritesheet and then manually set values; this is no longer the case, so using "Regions" is recommended. For the leanest files, in Anm2Ed, go to Settings -> Configure -> File -> Compatibility, and set it to "Anm2Ed Limited".

My process for making characters is that they're typically comprised of many "cells" (small graphics). I make these cells their own file, and then in the editor, I drag them onto "Spritesheets" and then right click -> merge them, with the "Make Spritesheet Regions" option enabled. Then, right click -> Pack on a spritesheet to optimize it all nice for one big spritesheet atlas with everything in one place. It won't produce a very comprehensible spritesheet, but it works just fine (not my problem). Make sure to save the spritesheets you're using once you're done.

If you'd like a quick primer on how to use either program, check either of [these](https://www.youtube.com/watch?v=qU_GMo2l7NY) [videos](https://www.youtube.com/watch?v=QiXVM6Gwzlw) out. Much of the information is interchangeable between the programs, though both are a little out of date on their current version.

For each "stage" of the character (graphical change), an animation equivalent will be expected. Stages are zero-indexed in this context; so the first stage would be 0. If you have an idle animation and want that for the first, starting stage, you'd need to name the animation "Idle0", for example, and then reference that in character.xml (more on that later). Don't manually input the number for the animation, just put in the first part, "Idle".

Know that despite Anm2Ed supporting sounds for animations, don't expect these to work in context based on names; use the .xml sound attributes when applicable.

# Resources

There's two folders inside resources; "characters" and "font".

"font" is just a font containing the font used in the selection screen. That's literally it. This technically isn't needed; Dear ImGui has its own font that will be used if this font is missing.

EVERYTHING ELSE is stored inside bespoke character archives (.zips) in the "characters" folder. This has all data associated with characters. Think of characters more as tailored game experiences rather than literally being just the characters. Not only is there the character graphics, but backgrounds, items, parameters, etc. These are intensely customizable to suit whatever experience you'd like (within the confines of the engine, of course).

Feed Snivy uses a collection of [XML](https://en.wikipedia.org/wiki/XML) files to parse data; make sure to brush up on the format. The engine can expect six files in the archive's root: 

- areas.xml
- character.xml
- cursor.xml
- dialogue.xml
- items.xml
- menu.xml
- play.xml

(dialogue.xml may or may not be optional; but in future updates I'll make sure of it, for dialogueless characters). Regardless, it's a safe bet to have. If

If you're making your own character, your best bet would probably be just to copy the Snivy character and edit it based on your needs, just as a helpful start.

# Character

## areas.xml
"Areas" refers to the backgrounds of the game. I'd planned for the game to dynamically switch areas based on stage (in case you want a character to become a blob that grows bigger than a city or whatever) but this isn't implemented. I'd just have one entry in here for the game's background.

### Areas
#### TextureRootPath (path)
Working folder/directory of textures the areas will use.

#### Area
#### Texture (path)
The file path of the area's texture (background).
#### Gravity (float)
Gravity the area has; applies to items' velocities per tick.
#### Friction (float)
Friction of the area; applies to items' velocities when grounded or hitting walls.
### AirResistance (float)
Air resistance of the area; applies to items' velocities when airborne.

## character.xml
This is the main character file where much of the functionality is stored.

### Character
#### Name (string)
Name of the character; will appear in dialogue, stats, etc.
#### TextureRootPath (path)
Working folder/directory of where other textures will be contained within.
#### SoundRootPath (path)
Working folder/directory of where other sounds will be contained within.
#### Render (path)
Texture for the character's "render" (i.e., a typical full-body display), will show in the Select screen.
#### Portrait (path)
Texture for the character's "portrait" (i.e., a cropped profile view), will show in the Select screen.
#### Anm2 (path)
Character's "anm2" file (uses TextureRootPath); should have all the character's animations.
#### Description (string)
A general description of the character; will show in the Select screen.
#### Author (string)
The author of the character.
#### Weight (float, kilograms)
The character's starting weight, in kilograms.
#### Capacity (float, calories)
The character's starting capacity, in calories.
#### CapacityMin (float, calories)
The character's minimum capacity, in calories.
#### CapacityMax (float, calories)
The character's maximum capacity, in calories. Know that max capacity is determined by Capacity * CapacityMaxMultiplier; this determines the max of the "base" capacity.
#### CapacityMaxMultiplier (float)
Determines the effective max capacity; will be capacity times this number.
#### CapacityIfOverStuffedOnDigestBonus (float, percent)
When a character is over stuffed (i.e., over base capacity), the character will an additional capacity when digesting based on how overstuffed they are, based on how many calories over the base capacity.
#### CaloriesToKilogram (float)
Determines how many calories become a kilogram (1 cal -> X kg).
#### DigestionRate (float, percent/tick)
The base digestion rate, in percent per tick (60 ticks per second).
#### DigestionRateMin (float, percent/tick)
The minimum digestion rate for the character, in percent per tick.
#### DigestionRateMax (float, percent/tick)
The maximum digestion rate for the character, in percent per tick.
#### DigestionTimerMax (int, ticks)
When digesting, the digestion bar will count down, and then when it hits 0, the current calories will be digested. This determines how long this takes, in ticks.
#### EatSpeed (float)
A multiplier that speeds/slows down the eating animation, at base.
#### EatSpeedMin (float)
Determines the minimum eating speed multiplier.
#### EatSpeedMax (float)
Determines the maximum eating speed multiplier.
#### GurgleChance (float, percent)
Determines how often the character will gurgle (see the Gurgle sounds later) per tick. 
#### GurgleCapacityMultiplier (float)
Per the character's capacity, multiplies the character's gurgle chance based on the percent of capacity filled (based on max capacity). Higher capacity = higher gurgle chance, using this number at maximum.
#### DialoguePoolID (string)
Determines the character's base dialogue options (for the "How are you feeling?" option in Chat). This is effectively "Stage 1"'s dialogue; each stage should have its own dialogue pool (see later). Also see dialogue.xml for how "Pools" work.
### AlternateSpritesheet
Determines the alternate spritesheet of the character, if applicable (in Pokemon terms, the "shiny").
#### Texture (path)
The alternate spritesheet texture; uses TextureRootPath.
#### Sound (path)
The sound that will play when the spritesheet is set to alternate (either on new game, or with an item that has IsToggleSpritesheet; see items.xml); uses SoundRootPath.
#### ID (int)
ID of spritesheet that the alternate spritesheet will replace in the character's .anm2.
#### ChanceOnNewGame (float, percent)
Chance of rolling for the alternate spritesheet on starting a new game, in percent.
### Stages
A "stage" represents each visual change of the character as the weight increases. By default, there's always one stage; adding more here will add additional stages.
#### Threshold (float, kilograms)
The weight threshold to reach this stage, in kilograms.
#### DialoguePoolID (string)
Determines the stage's dialogue options (for the "How are you feeling?" option in Chat). Also see dialogue.xml for how "Pools" work.
### Animations
The character's animations. Know that a lot of character animations are typically easily played/activated through Dialogue; this is just for animations that aren't reliant on that system. Know that _these animations should just be the base name_; all animations are expected to have a stage number after them, so don't include the number for these.
### Start
The animation name that will first play when starting a new game; used for like a character's introduction.
### Idle
The idle animation that the character will regularly return to.
### IdleFull
When over capacity, this idle animation will be used instead.
### StageUp
When going to a new stage after digestion and going over a stage's threshold, this animation will play.
#### Animation
The name of the animation to be used for each of these.
### Overrides
"Overrides" are the term I use for when some animations will be tweaked by the game engine for effect (blinking and talking being the ones used). There are two elements; "Talk" and "Blink" for this. Typically, how this is handled is that each animation with have an invisible blinking/talking layer (just the graphic that blinks), which will change when called for. Again, review the .anm2 format for what a "layer" is. All that's sourced is just the spritesheet crop in these cases; so don't worry about how the blink/talk layers are set up.
### Override
#### LayerSource (string)
The layer in the animation which will be sourced for the override.
#### LayerDestination (string)
The layer in the animation which the source will be applied to.
### EatAreas
An "eat area" is where the food should be dragged to. I'm pretty much expecting this to only be a mouth for most characters but hey maybe you want the character to also eat through their butt or something. Again, not something that's well-developed at the moment.
### EatArea
#### Null (string)
The null area in the animation where food will be checked for. Again, review the .anm2 format.
#### Animation (string)
The animation that will play when food is dragged to it.
#### Event (string)
The event that will be checked for, detrermining the time the food is actually eaten/chewed. Again, review the .anm2 format.
### ExpandAreas
The areas which will expand based on capacity percent; usually a stomach. This adds additional scale onto the layer of an animation. This also can scale a null as well.
ExpandAreas are presently hardcoded to scale with both capacity and weight at a 50/50 ratio; this could be changed in the future for custom ratios.
#### ExpandArea
#### Layer (string)
The layer of the animation that will expand.
#### Null (string)
The null of the animation that will expand.
#### ScaleAdd (float, percent)
The scale that will be added at maximum. Know that scale is a percent; a {100, 100} scale is the default and is the normal scale of an animation.
### InteractAreas
The areas on a character which can be interacted with; usually for belly rubs, kisses, etc.
### InteractArea
#### Type ("Rub", "Kiss", "Smack")
Three types are presently hard-coded in, but this is kind of hacky and custom support for different interactions may later be added. The cursor will need to be set from the "Tools" in order for each interact area to be activated.
#### Null (string)
The null in which the interact area can be triggered. Again, review the .anm2 format.
#### Animation (string)
The animation that will play when the interact area is activated (hovering and clicking).
#### AnimationFull (string)
The above, but when character is full.
#### AnimationCursorHover (string)
The animation the cursor will play when hovering over the interact area. 
#### AnimationCursorActive (string)
The animation the cursor will play when holding down click over the interact area.
#### DialoguePoolID (string)
The dialogue pool which will be drawn from when activating an interact area (see dialogue.xml).
### Sound
An interact area can play multiple sounds when interacting; add additional Sound elements to achieve this. Don't worry about repeatedly-loaded sounds; the game will cache them beforehand for efficiency.
#### Path (path)
The path of the sound being used.
### Sounds
Sounds that play in some contexts.
### Digest
Sounds that will play when the character begins digesting (digestion bar full).
### Gurgle
Ambient sounds that will play randomly based on capacity; see GurgleChance and GurgleCapacityMultiplier above.
#### Sound (path)
The path of the sound for the specific sound type.

## cursor.xml
Determines the cursor appearance and behavior.
### TextureRootPath (path)
Working folder/directory of textures the cursor will use.
#### SoundRootPath (path)
Working folder/directory of sounds the cursor will use.
#### Anm2 (path)
The anm2 file the cursor will use (depends on TextureRootPath).
### Animations
### Idle
The cursor's default idle animation.
### Hover
The cursor's hover animation, when hovering over an item.
### Grab
The cursor's grab animation, when grabbing an item.
### Pan
The cursor's pan animation, when panning with middle mouse button.
### Zoom
The cursor's zoom animation, when zooming in/out with the mouse wheel.
### Return 
The cursor's return animation, when holding right click to return an item to the inventory (or to dispose of it if chewed.)
#### Animation (string)
The animation the cursor will use in these contexts.
### Sounds
Know that multiple of these sounds can be defined for each context.
### Grab
The sound that will play when grabbing an item.
### Release
The sound that will play when releasing an item.
### Throw
The sound that will play when throwing an item (releasing when dragging quickly).
#### Sound (path)
The path of the sound for these respective contexts.

## dialogue.xml
The collection of character dialogue; likely the biggest file, depending on your needs. 
### Dialogue
### Entries
### Entry
Each bit of dialogue is referred to as an "entry".
#### ID (string)
Name for the entry. Other entries will rely on this for dialogue chains.
#### Next (string)
The ID of the entry that will follow after this one. If no Next entry, the text will not continue. Make sure to connect these.
#### Text (string)
The actual dialogue content of the entry. Should support Unicode, provided the font contains the characters.
#### Animation (string)
A character animation that will play at the beginning of the dialogue. This may be expanded into the future to allow animations to play dynamically per dialogue index. Again, make sure it's just the base name for the animation, no stage numbers.
### Choice
Dialogue can be branching; simply added "Choice" elements into the Entry element.
#### Next (string)
The dialogue the choice will lead to.
#### Text (string)
The text of the choice; will appear as a series of buttons in the text window.
### Pools
A "pool" of dialogue refers to a collection of entries that can be randomly picked in some contexts.
### Pool
#### ID (string)
The name of the dialogue pool; can be referenced elsewhere.
### PoolEntry
### ID (string)
The name of the entry; to be added to the pool.

### Start
This dialogue will play upon a new game, once its animation has concluded.
#### Animation (string)
The name of the animation that will be played.
#### ID (string)
The name of the entry that will be played.
### End
The dialogue that will play upon completion of the game (character hitting max stage).
#### ID (string)
The name of the entry that will be played.
### Help
The dialogue that will be play when the player presses the "Help" button in "Chat".
#### ID (string)
The name of the entry that will be played.

### StageUp
Dialogue that will play after a character undergoes a stage up.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Random
The dialogue that will be play when the player presses the "Let's chat!" button in "Chat".
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Feed
The dialogue that will be play when the user begins holding a food item.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### FeedFull
The dialogue that will be play when the user begins holding a food item, when the character is over capacity.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Eat
The dialogue that will be play after the character finishes a food item.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### EatFull
The dialogue that will be play after the character finishes a food item, when the character is over capacity.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Full
The dialogue that will be play when the character is completely full and will deny the user feeding them.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Throw
The dialogue that will be play when the user throws food.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### LowCapacity
The dialogue that will be play when the character is presented a food item completely over their maximum capacity.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### Digest
The dialogue that will be play when digesting is finished.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### FoodTaken
The dialogue that will play when food is removed from the null area during a character's eating animation.
#### PoolID (string)
The name of the pool that entries will be drawn from.
### FoodTakenFull
The dialogue that will play when food is removed from the null area during a character's eating animation, when the character is over capacity.
#### PoolID (string)
The name of the pool that entries will be drawn from.


# Saves

Outside of resources, Feed Snivy also has a few files it writes outside of the game. You will find these in %AppData%/snivy on Windows and ~/.local/share/snivy on Linux.

## settings.xml
Stores general game settings and configuration; beyond invididual characters.
### MeasurementSystem ("Imperial", "Metric")
Determines measurement system (kg/lb).
### Volume (int, percent)
Master volume.
### ColorR, ColorG, ColorB (float, 0-1)
Red/Green/Blue components of menu color.
### WindowX, WindowY (int)
Window position.
### WindowW/H (int)
Window size.

## *.save
Saves are per-character; will be named "[blank].save", stored in "saves" folder, from the name of the character's archive.

### Save
#### IsPostgame (bool)
Determines if the game has been completed (character's max stage has been reached); if true, will enable cheats.
#### IsAlternateSpritesheet (bool)
Determines if the character's spritesheet is using the alternate version (in Pokemon, would be the "shiny" version

### Character
#### Weight (float)
Character's current weight, in kilograms.
#### Calories (float)
Character's current consumed calories.
#### Capacity (float)
Character's capacity, in calories. Remember, max capacity effectively is capacity * CapacityMaxMultiplier (from character.xml) 
#### DigestionRate (float, percent)
Character's digestion rate in percent, per game tick (game ticks 60 times per second).
#### EatSpeed (float)
Eat speed multiplier for the character's Eat animation.
#### IsDigesting (bool)
Determines if character is currently digesting (when the bar is going down)
#### DigestionProgress (float, percent)
Character's digestion progress. Digestion max is always 100%.
#### DigestionTimer (int)
When digestion bar is going down, this is the remaining time to 0 (in ticks)
#### TotalCaloriesConsumed (float)
Total calories consumed by the character, per save file.
#### TotalFoodItemsEaten (int)
How many food items have been completely consumed by the character, per save file.

### Play
#### TotalPlays (int)
However many times the "play" game has been attempted (hitting the bar counts as one "play")
#### HighScore (int)
Highest score the player has achieved in the "play" game.
#### BestCombo (int)
Highest combo the player has achieved (how many successful hits the player has gotten in one session)
#### Grades
Play grades are the ratings the game gives based on where the player hit.
##### ID (int)
ID of grade being tracked (see play.xml)
##### Count (int)
How many times the grade has been hit.

### Inventory
Items.
#### ID (int)
ID of item being tracked (see items.xml)
#### Quantity (int)
Count of the item.
