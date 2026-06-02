#ifndef _VIDEO_H
#define _VIDEO_H

#include "../video.h"

/*=
Video playback
3850
**/

// Introduction
/**
Raydium supports simple video playback, using a simple dedicated video 
codec (JPGS), useful for menus enhancements, "speaking" thumbnails, ...
This codec now supports audio syncing with OGG files.
You will find an small utility, ##mk_jpgs## in Raydium source tree, didacted to
movie creation.
The Raydium official source tree also provides a small JPGS player,
named ##jpgs_play.c##. You can compile it like any other Raydium application.
**/

// How to create a JPGS movie ?
/**
First, compile ##mk_jpgs##: example: ##gcc mk_jpgs.c -o mk_jpgs## or any other
standard build command.
Then, generate JPEG pictures (using a temporary directory, if possible), with
something like mplayer, for example:
##mplayer movie.avi -vo jpeg:quality=50 -vf scale=256:256 -nosound -fps 1000##, 
where you may change quality factor and output size. Use "hardware friendly" 
sizes (64, 128,256,...) !
You can now build JPGS file:
##./mk_jpgs 25 256 256 video.jpgs## (fps, size x, size y, output file)
If you want to extract the audio, use this:
##mplayer -vc nul -vo null -ao pcm:file=audio.dump movie.avi##
##oggenc audio.dump##
**/

__rayapi void raydium_video_init(void);
/**
Internal use.
**/

__rayapi signed char raydium_video_isvalid(int i);
/**
Internal use, but you can call this function if you want to check if a
video id is valid (in bounds and open).
**/

__rayapi int raydium_video_find_free(void);
/**
Internal use.
Finds a free video slot.
**/

__rayapi int raydium_video_find(char *name);
/**
Resolvs video ##name##, returning video id.
Returns -1 when video is not found.
**/

__rayapi void raydium_video_jpeg_decompress(FILE *fp,unsigned char *to);
/**
Internal.
**/

__rayapi int raydium_video_open(char *filename, char *as);
/**
This function will open and prepare video ##filename##, and will attach
this video to a "live texture" (see Live API chapter, if needed).
The playback starts automatically.
You can then display the video with OSD functions like, for
example, ##raydium_live_texture_mask_name()## or using it within meshes.
**/

__rayapi int raydium_video_open_with_sound(char *filename, char *as, char *ogg);
/**
Same as above, but it will also play the ##ogg## file, synced with the
video stream.
**/

__rayapi void raydium_video_callback_video(int id);
/**
Internal use.
**/

__rayapi float raydium_video_sound_callback(void);
/**
Internal use. Callback for sound.c BufferData function.
**/

__rayapi void raydium_video_callback(void);
/**
Internal use. Frame callback.
**/

__rayapi void raydium_video_delete(int id);
/**
Will delete video ##id##. Warning: this function will not delete
associated Live texture, so you may open a new video with the same
texture name, but video size must be the same a the previous one.
**/

__rayapi void raydium_video_delete_name(char *name);
/**
Same as above, using video name.
**/

__rayapi void raydium_video_loop(int id, signed char loop);
/**
Sets loop attribute for the video ##id##. By defaults, video loops. Call
this function with loop=0 to disable this behavior.
**/

__rayapi void raydium_video_loop_name(char *name, signed char loop);
/**
Same as above, using video name.
**/

__rayapi signed char raydium_video_isplaying(int id);
/**
Returns **1** is video ##id## is playing, **0** if this video is stopped,
and **-1** if function failed.
**/

__rayapi signed char raydium_video_isplaying_name(char *name);
/**
Same as above, using video name.
**/

__rayapi void raydium_video_fps_change(int id, float fps);
/**
This function will change the play rate of video ##id## to ##fps## frames
per second.
**/

__rayapi void raydium_video_fps_change_name(char *name, float fps);
/**
Same as above, using video name.
**/

__rayapi float raydium_video_elapsed(int id);
/**
Will return, in seconds, the elapsed time for the video ##id##.
**/

__rayapi float raydium_video_elapsed_name(char *name);
/**
Same as above, using video name.
**/

__rayapi float raydium_video_duration(int id);
/**
Will return, in seconds, the total duration for the video ##id##.
**/

__rayapi float raydium_video_duration_name(char *name);
/**
Same as above, using video name.
**/

__rayapi signed char raydium_video_seek(int id, float time);
/**
Will jump at ##time## (in seconds) in the ##id## video.
**/

__rayapi signed char raydium_video_seek_name(char *name, float time);
/**
Same as above, using video name.
**/

__rayapi signed char raydium_video_seek_rel(int id, float time);
/**
Will seek from ##time## seconds in the video ##id## from the current elasped
time. It's a relative jump, in other words.
You can use negative values for forward jumps.
**/

__rayapi signed char raydium_video_seek_rel_name(char *name, float time);
/**
Same as above, using video name.
**/

#endif
