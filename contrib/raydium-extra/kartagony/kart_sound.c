// Kartagony - sound management

#define SOUND_MAX  1

#define SOUND_ENGINE 0

int sounds[SOUND_MAX];

void sound_init(void)
{
raydium_sound_DefaultReferenceDistance=4.f;

//sounds[SOUND_ENGINE]=raydium_sound_LoadWav("scooter2.wav");
sounds[SOUND_ENGINE]=raydium_sound_LoadWav("scooter2.wav");
//sounds[SOUND_ENGINE]=raydium_sound_LoadWav("murcielago2.wav");
raydium_sound_SetSourcePitch(sounds[SOUND_ENGINE],0);
}
