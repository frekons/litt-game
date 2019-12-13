#include "Audio.h"

// TEK BIR ADET MAIN MUSIC OLDUGU VARSAYILARAK YAZILMISTIR VE main_music DEGISKENI HEADER DOSYASINDA TANIMLANMISTIR


// MUSIC FUNCTIONS
Mix_Music* loadMusic(const char* music_file_dir)
{
	main_music = Mix_LoadMUS(music_file_dir);
	if (!main_music)
	{
		printf("Failed to load music! Error: %s\n", Mix_GetError());
	}

	return main_music;
}

void play_music(const char* music_file_dir)
{
	main_music = loadMusic(music_file_dir);
	Mix_PlayMusic(main_music, -1);
}

void close_music()
{
	Mix_FreeMusic(main_music);
	main_music = NULL;
}

bool is_playing_music()
{
	return Mix_PlayingMusic();
}

bool is_paused_music()
{
	return Mix_PausedMusic();
}

void resume_music()
{
	Mix_ResumeMusic();
}

void pause_music()
{
	Mix_PauseMusic();
}

void terminate_music()
{
	Mix_HaltMusic();
}




// main_music FONKSİIONLARI AKSINE BIRDEN FAZLA SES EFEKTI KULLANILMASI AMACIYLA YAZILMISTIR. O YUZDEN PARAMETRELERI main_music FONKSIYONLARIYLA FARKLILIK GOSTEREBILIR

// SOUND EFFECT FUNCTIONS

Mix_Chunk* load_sound_effect(const char* sound_effect_file_dir)
{
	Mix_Chunk* sound_effect = Mix_LoadWAV(sound_effect_file_dir);
	if (!sound_effect)
	{
		printf("Failed to load sound effect! Error: %s\n", Mix_GetError());
	}
	return sound_effect;
}

Mix_Chunk* play_sound_effect(const char* sound_effect_file_dir)
{
	Mix_Chunk* sound_effect = load_sound_effect(sound_effect_file_dir);
	Mix_PlayChannel(-1, sound_effect, 0);

	return sound_effect;
}

void close_sound_effect(Mix_Chunk* sound_effect)
{
	Mix_FreeChunk(sound_effect);
	sound_effect = NULL;
}




// QUIT LIBRARY
void quit_audio_library()
{
	Mix_Quit();
}

