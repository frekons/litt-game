#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>


//The music that will be played
Mix_Music* main_music;	// tek bir tane ana tema muzigi oldugunu varsayarak buraya koydum. ana tema muzigi ile fonksiyonlar bunu baz alıyor sadece

Mix_Chunk* effect;	// ister buraya koyariz ses efektlerini ister main'de tanimlariz fonksiyonlari cagirirken fark etmez


// MUSIC
void play_music(const char* music_file_dir);
Mix_Music* loadMusic(const char* music_file_dir);	// play_music's subfunction
void close_music();		// free function
bool is_playing_music();
bool is_paused_music();
void resume_music();
void pause_music();
void terminate_music();	// sanirim muzigi durdurmakla ayni islemi yapan bir fonksiyonu wrap ettim



// SOUND EFFECTS
Mix_Chunk* play_sound_effect(const char* sound_effect_file_dir);
Mix_Chunk* load_sound_effect(const char* sound_effect_file_dir);	// play_sound_effect's subfunction
void close_sound_effect(Mix_Chunk* sound_effect);	// free function


// QUIT LIBRARY
void quit_audio_library();

#endif
