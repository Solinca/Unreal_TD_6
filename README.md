# Unreal_TD_6

## Controls:

### Common Inputs:

Z Q S D     -> Player Movement

Mouse       -> Look around

Space       -> Jump

Escape      -> Trigger in game pause menu

### Survivor Inputs:

Left Shift  -> Hold to run

Left Ctrl   -> Crouch (Hold), Uncrouch (Release)

E (hold)    -> Interact with objective or dead player

F           -> Toggle Flashlight On/Off

### Monster Inputs:

Left Click  -> Trigger an attack if cooldown is up

Right Click -> Trigger a special ability if cooldown is up

E           -> Trigger a Player Scream (mimic) if cooldown is up. Has 10% chance to trigger a monster scream

## Session

A session can only be launched locally.

When creating a session, you can select the name of the session, your player name, number of players and max number of monsters. Number of players can't be lesser or equal to max number of monsters.

When joining a session, you can select your player name.

Once a game session is launched or full, players can't find it anymore. If the session was found before launch or before it was full, the joining player will be denied access.

## Gameplay

At start, the number of player objectives spawned is equal to the number of players in game.

In order to be fair and to forbid monsters to camp a player objective until the end, Survivors can win if every player objectives are completed or 
at the end of the timer if CompletedPlayerObjectives >= NumberOfObjectives - NumberOfMonsters.

Monsters can win if no player are alive or at the end of the timer if CompletedPlayerObjectives < NumberOfObjectives - NumberOfMonsters.

Monsters ability:

Butcher: launch a knife forward colling with everything. If a survivor is hit, he is killed. After hitting something, the knife is reeled back for 2 seconds. The butcher is stunned during the ability.

Hunter: Place a trap on the ground, snaring any survivor colliding with it for 3 seconds. Can place a maximum of 5 traps.

Ghost: Turn invisible for 3 seconds.

Slime: Gain x2.5 speed for 3 seconds after a 1 second transformation (takes 1 second to transform back to normal).

Predator: For 3 seconds, can see everyone on the map through everything.

## References

### Musics

Victory: https://pixabay.com/sound-effects/musical-victory-chime-366449/

Defeat: https://pixabay.com/sound-effects/musical-marcha-f%c3%banebre-8-bits-260615/

Ambiance: https://pixabay.com/sound-effects/horror-son-ambiance-film-1-253767/

### Sounds

Rain: https://pixabay.com/sound-effects/nature-gentle-rain-07-437321/

Thunder: https://pixabay.com/sound-effects/nature-thunder-sound-375727/

Player Down: https://pixabay.com/sound-effects/people-young-man-being-hurt-95628/

Player Dead: https://pixabay.com/sound-effects/people-man-screaming-417689/

Player Asking For Help: https://pixabay.com/sound-effects/people-help-help-322552/

Monster Fake Help Mimic: https://pixabay.com/sound-effects/people-help-me-mp3-54969/

Player Work On Objective: https://pixabay.com/sound-effects/film-special-effects-large-steampunk-factory-machine-188048/

Player Footstep:  https://pixabay.com/sound-effects/film-special-effects-st1-footstep-sfx-323053/

Butcher Hook: https://pixabay.com/sound-effects/film-special-effects-chains-48217/

Butcher Trigger: https://pixabay.com/sound-effects/horror-clown-come-here-487802/

Predator Trigger: https://pixabay.com/sound-effects/horror-monster-growl-382706/

Ghost Trigger: https://pixabay.com/sound-effects/film-special-effects-loud-woosh-sound-effect-no-copyright-390905/

Slime Trigger: https://pixabay.com/sound-effects/film-special-effects-slime-alien-sound-with-reverb-291370/

Trap placed: https://pixabay.com/sound-effects/film-special-effects-bear-trap-103800/

Trap triggered: https://pixabay.com/sound-effects/film-special-effects-dbd-bear-trap-being-disarmed-135902/
