#include "Audio.h"

#include "Globals.h"

typedef struct Music {
	char file_name[128];
	Mix_Chunk* music;
}Music;


struct SMusicList {

	int Count;
	Music** List;

};


typedef struct SMusicList MusicList;



void initialize_music_list(MusicList * list)
{
	list->Count = 0;
	list->List = NULL;
}

void add_music_to_list(MusicList * list, Music* sound_object)
{
	list->Count++;
	list->List = (Music**)realloc(list->List, list->Count * sizeof(Music*));

	list->List[list->Count - 1] = sound_object;
}

Music* find_in_list(MusicList * list, char* name)
{
	for (int i = 0; i < list->Count; i++)
	{
		if (strcmp(list->List[i]->file_name, name) == 0)
			return list->List[i];
	}

	return NULL;
}

MusicList music_list;


void InitializeSounds()
{
	initialize_music_list(&music_list);

	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096);

	Mix_VolumeMusic(0);

}

// TEK BIR ADET MAIN MUSIC OLDUGU VARSAYILARAK YAZILMISTIR VE main_music DEGISKENI HEADER DOSYASINDA TANIMLANMISTIR

// MUSIC FUNCTIONS
//Mix_Music* loadMusic(const char* music_file_dir)
//{
//	main_music = Mix_LoadMUS(music_file_dir);
//	if (!main_music)
//	{
//		printf("Failed to load music! Error: %s\n", Mix_GetError());
//	}
//
//	return main_music;
//}
//
//void play_music(const char* music_file_dir)
//{
//	main_music = loadMusic(music_file_dir);
//	Mix_PlayMusic(main_music, -1);
//}
//
//void close_music()
//{
//	Mix_FreeMusic(main_music);
//	main_music = NULL;
//}

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

Mix_Chunk* load_sound(char* sound_effect_file_dir)
{
	Music* music = find_in_list(&music_list, sound_effect_file_dir);

	if (!music)
	{
		Mix_Chunk* sound_effect = Mix_LoadWAV(sound_effect_file_dir);

		if (sound_effect == NULL)
		{
			printf("ERROR: Failed to load sound! Error: %s\n", Mix_GetError());
			return NULL;
		}

		music = (Music*)malloc(sizeof(Music));

		strcpy(music->file_name, sound_effect_file_dir);
		music->music = sound_effect;

		add_music_to_list(&music_list, music);
	}

		
	return music->music;
}

int play_sound(char* sound_effect_file_dir)
{
	Mix_Chunk* sound_effect = load_sound(sound_effect_file_dir);

	Mix_VolumeChunk(sound_effect, volume);

	int channel = Mix_PlayChannel(-1, sound_effect, 0);

	return channel;
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

