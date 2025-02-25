#pragma once
#include <SFML/Audio.hpp>

class AudioManager {
private:
	Music MainSoundTrack;
	SoundBuffer ShootSoundBuffer;
	Sound* ShootSound;
	bool SoundOn;
	bool SoundTrackOn;
	int currentSoundTrack;

	void SetShootSound(std::string ShootSoundPath)
	{
		if (!ShootSoundBuffer.loadFromFile("music/" + ShootSoundPath))
			std::cerr << "Do not found " << ShootSoundPath << " in 'music/'" << std::endl;
		ShootSound = new Sound(ShootSoundBuffer);

	}

	void SetSoundTrack(std::string SoundTrackPath)
	{
		if (!MainSoundTrack.openFromFile("music/" + SoundTrackPath))
			std::cerr << "Do not found " << SoundTrackPath << " in 'music/'" << std::endl;
	}

public:
	AudioManager(std::string ShootSoundPath)
	{
		SetShootSound(ShootSoundPath);
		currentSoundTrack = 0;
		SetNextSoundTrack();
		SoundOn = false;
		SoundOn = false;
		SetSoundTrackVolume();
		SetSoundVolume();
	}

	void SetNextSoundTrack()
	{
		switch (currentSoundTrack)
		{
		case 0: SetSoundTrack("Moon.ogg"); currentSoundTrack = 1; break;
		case 1: SetSoundTrack("Heart.ogg"); currentSoundTrack = 2; break;
		case 2: SetSoundTrack("Blue.ogg"); currentSoundTrack = 0; break;
		}
		MainSoundTrack.play();
		MainSoundTrack.setLooping(true);
	}

	void SetSoundTrackVolume()
	{
		if (SoundTrackOn)
		{
			MainSoundTrack.setVolume(15);
			SoundTrackOn = false;
		}
		else
		{
			MainSoundTrack.setVolume(0);
			SoundTrackOn = true;
		}
	}

	void SetSoundVolume()
	{
		if (SoundOn)
		{
			ShootSound->setVolume(10);
			SoundOn = false;
		}
		else
		{
			ShootSound->setVolume(0);
			SoundOn = true;
		}
	}

	void PlayShootSound()
	{
		ShootSound->play();
	}
};