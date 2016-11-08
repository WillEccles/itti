/* Info of the song:
 * all numbers are in fractional seconds
 * "[songname]SONG_PART_DELIM[artist]SONG_PART_DELIM[album]SONG_PART_DELIM[duration]SONG_PART_DELIM[position]" is returned.
 * duration will be the total seconds, position = how many seconds into the song you are */
#define SONG_INFO "osascript -e 'tell app \"itunes\" to set theInfo to {name, artist, album, duration} of current track & {player position, sound volume}' -e \"set AppleScript's text item delimiters to \\\"SONG_PART_DELIM\\\"\" -e 'set theInfoString to theInfo as string'"
/* Command to get the player's current position (float) */
#define PLAYER_POS "osascript -e 'tell app \"itunes\" to player position'"
/* Command to pause the player */
#define PLAYER_PAUSE "osascript -e 'tell app \"itunes\" to pause'"
/* Command to resume playback */
#define PLAYER_PLAY "osascript -e 'tell app \"itunes\" to resume'"
/* Command to toggle between playing and paused */
#define PLAYER_TOGGLE "osascript -e 'tell app \"itunes\" to playpause'"
/* Command to go to previous track */
#define PLAYER_PREV "osascript -e 'tell app \"itunes\" to previous track'"
/* Command to go to next track */
#define PLAYER_NEXT "osascript -e 'tell app \"itunes\" to next track'"
#define PLAYER_SKIP PLAYER_NEXT
/* Command to stop playback */
#define PLAYER_STOP "osascript -e 'tell app \"itunes\" to stop'"
/* Get the player state - "playing" "paused" "stopped" */
#define PLAYER_STATE "osascript -e 'tell app \"itunes\" to player state as string'"
/* Get the player volume from 0 to 100 */
#define PLAYER_VOLUME "osascript -e 'tell app \"itunes\" to sound volume"
/* Change the sound volume to percent (0-100) */
#define SET_PLAYER_VOLUME(percent) (std::string("osascript -e 'tell app \"itunes\" to set sound volume to " + std::to_string(percent) + "'").c_str())
