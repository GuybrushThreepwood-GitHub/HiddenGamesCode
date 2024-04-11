
/*===================================================================
	File: MusicMediaPlayer.h
	Game: Prisoner 84

	(C)Hidden Games
=====================================================================*/

#ifndef __MUSICMEDIAPLAYER_H__
#define __MUSICMEDIAPLAYER_H__

#ifdef BASE_PLATFORM_iOS

#include "Sound/SoundCommon.h"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <MediaPlayer/MediaPlayer.h>

@interface MusicMediaPlayer : NSObject <MPMediaPickerControllerDelegate, AVAudioPlayerDelegate>
{
	snd::MusicMode playerMode;
	
	AVAudioPlayer* mAVPlayer;

	MPMusicPlayerController *mPlayer;
	MPMediaQuery *mMediaList;
	
	snd::MusicCallback* mCallback;

	bool mIsPlaying;
	bool mIsPaused;
	bool mFinishedTrack;
	
	MPMusicPlaybackState mPlaybackState;
	
	bool mIsCurrentTrackInPlaylist;
	bool mPlaylistsExist;
	
	int mCurrentPlaylistIndex;
	int mCurrentSongIndex;
	
	bool mUsingPlaylists;
	
	MPMediaItem* mCurrentSong;
	
	int mNumPlaylists;
	snd::Playlist* mPlaylists;
	
	char mCurrentSongName[core::MAX_PATH];
}

@property (nonatomic, strong) AVAudioPlayer* mAVPlayer;
@property (nonatomic, strong) MPMusicPlayerController* mPlayer;
@property (nonatomic, strong) MPMediaQuery* mMediaList;

-(id) initWithMode:(snd::MusicMode) mode;
-(void) ReleaseMusicPlayer;

-(void) Update: (float) deltaTime;
-(void) UpdatePlaybackState;
-(void) UpdatePlaylists;

-(void) SetShuffleState: (bool) state;
-(bool) GetShuffleState;
-(void) SetRepeatState: (bool) state;
-(bool) GetRepeatState;

-(bool) IsPlaying;
-(bool) IsPaused;

-(bool) IsUsingPlaylist;
-(const char*) GetCurrentSong;

-(const char*) GetPlaylistName: (int)playlistIndex;
-(const char*) GetSongName: (int)playlistIndex whichSong: (int)songIndex;
-(int) GetTotalNumberOfPlaylists;
-(int) GetTotalNumberOfSongsInPlaylist: (int)playlistIndex;
-(int) GetCurrentPlaylistIndex;
-(int) GetCurrentSongIndex;
-(void) PlaySong: (int)playlistIndex whichSong: (int)songIndex;
-(void) PlaySong;
-(void) PauseSong;
-(void) ResumeSong;
-(void) StopSong;

-(void) SelectPreviousSong;
-(void) SelectNextSong;

- (void) setMusicCallback: (snd::MusicCallback *)callback;

/// AV Player (single track only)

-(void) PlaySong: (const char*)filename loopFlag: (bool)loop;

-(void) SetVolume: (float)vol;

-(bool) HasFinishedTrack;

@end

#endif // BASE_PLATFORM_iOS

#endif // __MUSICMEDIAPLAYER_H__
