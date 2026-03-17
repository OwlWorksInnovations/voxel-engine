# To-do list

## Engine Foundation
- [x] Window & OpenGL context management (if not already wrapped)
- [x] Game loop (fixed timestep update + render)
- [ ] Resource manager (load/cache shaders, textures)
- [ ] Event system (decouple input from gameplay)

## World
- [ ] Block/voxel data types
- [ ] Chunk data structure (3D array per chunk)
- [ ] Chunk manager (load/unload chunks around player)
- [ ] Mesh generation from chunk data (greedy or naive)
- [ ] Mesh rebuild on block change

## Camera
- [ ] First-person camera (yaw/pitch from mouse)
- [ ] Projection + view matrix
- [ ] Mouse capture/lock

## Player
- [ ] Player position & velocity
- [ ] Gravity
- [ ] AABB collision against blocks
- [ ] Jumping
- [ ] Walking speed + friction

## Block Interaction
- [ ] Ray casting (DDA algorithm)
- [ ] Block highlight/outline on hover
- [ ] Break block (left click)
- [ ] Place block (right click, on adjacent face)
- [ ] Held block type
- [ ] Scroll wheel to switch block type

## Rendering
- [ ] Texture atlas loading
- [ ] UV mapping per block face
- [ ] Frustum culling (skip chunks outside view)
- [ ] Basic ambient occlusion (optional but big visual bump)

## HUD / UI
- [ ] Text rendering (FreeType or stb_truetype)
- [ ] Crosshair
- [ ] Block selector bar (hotbar)
- [ ] Debug overlay (FPS, coords, facing direction)

## Audio
- [ ] Audio backend (miniaudio is easiest)
- [ ] Block break/place sounds
- [ ] Footstep sounds
- [ ] Ambient background

## Game State
- [ ] State machine (main menu -> playing -> paused)
- [ ] Basic main menu
- [ ] Pause menu + resume/quit

## ECS (do this after sandbox works)
- [ ] Entity & component storage
- [ ] System update loop
- [ ] Refactor player, camera, physics into components

## Weapons
- [ ] Hitscan raycast gun
- [ ] Projectile weapon (rocket/grenade that flies and explodes)
- [ ] Explosion system (destroy blocks in radius + apply knockback)
- [ ] Weapon switching (1/2 keys)
- [ ] Ammo counter
- [ ] Muzzle flash + screen shake
- [ ] Gunshot sounds

## Combat
- [ ] Health system for player
- [ ] Taking damage
- [ ] Death + respawn at spawn point
- [ ] Kill counter
- [ ] Scoreboard (Tab key)
- [ ] Match timer or kill limit win condition
- [ ] Win screen

## Bots
- [ ] Bot entity (position, health, velocity)
- [ ] Line of sight check (raycast to player)
- [ ] Bot shoots at player when visible
- [ ] Bot takes damage and dies
- [ ] Bot respawns at spawn point
- [ ] Basic movement (wander + chase player)
- [ ] Bot difficulty setting (reaction time, accuracy)

## Arena
- [ ] Hand-built destructible arena map
- [ ] Multiple spawn points placed around map
- [ ] Ambient + in-game music
