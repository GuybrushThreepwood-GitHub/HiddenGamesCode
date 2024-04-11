
/*===================================================================
	File: MusicMediaPlayer.mm
	Game: Cabby
	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#include "CoreBase.h"
#include "MathBase.h"
#import "Audio/ios/MusicMediaPlayer.h"

const int MAX_SONGS_PER_PLAYLIST = 50;

@implementation MusicMediaPlayer

@synthesize mAVPlayer; // the player object
@synthesize mPlayer; // the player object
@synthesize mMediaList; // the playlist

/////////////////////////////////////////////////////
/// Method: initWithMode
/// Params: None
/// 
/////////////////////////////////////////////////////
-(id) initWithMode: (snd::MusicMode) mode
{
	if( self = [super init ] )
	{
		playerMode = mode;
		
		mNumPlaylists = 0;
		mPlaylists = 0;
		
		mCallback = nil;

		mIsPlaying = false;
		mIsPaused = false;
		
		mIsCurrentTrackInPlaylist = false;
		mPlaylistsExist = false;
		
		mCurrentPlaylistIndex = -1;
		mCurrentSongIndex = -1;
		
		mUsingPlaylists = false;
		
		mCurrentSong = nil;
		
		mFinishedTrack = true;
		
		if( playerMode == snd::MusicMode_SingleFilePlayer )
		{
			self.mAVPlayer = [[AVAudioPlayer alloc] init];
		}
		else if( playerMode == snd::MusicMode_PlaylistPlayer )
		{
			self.mPlayer = [MPMusicPlayerController systemMusicPlayer];
			
			NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
			
			[notificationCenter addObserver: self
								selector:    @selector (handleNowPlayingItemChanged:)
								name:        MPMusicPlayerControllerNowPlayingItemDidChangeNotification
								object:      self.mPlayer ];
			
			
			
			[notificationCenter addObserver: self
								selector:    @selector (handlePlaybackStateChanged:)
								name:        MPMusicPlayerControllerPlaybackStateDidChangeNotification
								object:      self.mPlayer ];
			
			[self.mPlayer beginGeneratingPlaybackNotifications];		
			
			[self UpdatePlaybackState];
			
			// allocate space for the playlists
			mMediaList = [[MPMediaQuery alloc] init];
		
			[mMediaList setGroupingType: MPMediaGroupingPlaylist];
			
			// something already playing
			if( [self.mPlayer nowPlayingItem ] != nil )
			{
				mCurrentSong = [self.mPlayer nowPlayingItem ];
				
				std::memset( mCurrentSongName, 0, sizeof(char)*core::MAX_PATH );
				
				NSString *songName = [ mCurrentSong valueForProperty:MPMediaItemPropertyTitle ];
				NSLog ( @"Playing Song %@", songName );
				
				[ songName getCString: (char *)mCurrentSongName maxLength: (core::MAX_PATH-1) encoding: NSNonLossyASCIIStringEncoding ];	
				DBGLOG( "PLAYING SONG: %s\n", mCurrentSongName );
				
				[self UpdatePlaylists];
			}
		}
	}
	
	return self;
}

/////////////////////////////////////////////////////
/// Method: ReleaseMusicPlayer
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) ReleaseMusicPlayer
{
	int i=0;
	
	if( playerMode == snd::MusicMode_SingleFilePlayer )
	{
		self.mAVPlayer = nil;
	}
	else if( playerMode == snd::MusicMode_PlaylistPlayer )
	{
		// deregister notifications
		[[NSNotificationCenter defaultCenter] removeObserver: self
		 name:           MPMusicPlayerControllerNowPlayingItemDidChangeNotification
		 object:         self.mPlayer];
		
		[[NSNotificationCenter defaultCenter]
		 removeObserver: self
		 name:           MPMusicPlayerControllerPlaybackStateDidChangeNotification
		 object:         self.mPlayer];
		
		[self.mPlayer endGeneratingPlaybackNotifications];
		
		if( mPlaylists != 0 )
		{
			for( i=0; i < mNumPlaylists; ++i )
			{
				if( mPlaylists[i].playListSongs != 0 )
					delete[] mPlaylists[i].playListSongs;
			}
			
			delete[] mPlaylists;
			mPlaylists = 0;
		}	
		
		mIsPlaying = false;
		mIsPaused = false;
		
		self.mPlayer = nil;
		self.mMediaList = nil;
	}
	
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) Update: (float) deltaTime
{

}

/////////////////////////////////////////////////////
/// Method: UpdatePlaybackState
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) UpdatePlaybackState
{
	mPlaybackState = [self.mPlayer playbackState];
	
	switch( mPlaybackState )
	{
		case MPMusicPlaybackStateStopped:
		{
			mIsPlaying = false;
			mIsPaused = false;
		}break;
		case MPMusicPlaybackStatePlaying:
		{			
			mIsPlaying = true;	
			mIsPaused = false;
		}break;
		case MPMusicPlaybackStatePaused:
		{
			mIsPlaying = true;
			mIsPaused = true;

		}break;
		case MPMusicPlaybackStateInterrupted:
		case MPMusicPlaybackStateSeekingForward:
		case MPMusicPlaybackStateSeekingBackward:
		default:
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdatePlaylists
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) UpdatePlaylists
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	int i=0;
	int playListIndex = 0;
	int songIndex = 0;
	
	NSArray *collections = [mMediaList collections];	
	
	// remove previous playlists
	if( mPlaylists != 0 )
	{
		for( i=0; i < mNumPlaylists; ++i )
		{
			if( mPlaylists[i].playListSongs != 0 )
				delete[] mPlaylists[i].playListSongs;
		}
		
		delete[] mPlaylists;
		mPlaylists = 0;
	}
	
	if( [collections count] != 0 )
	{
		mPlaylists = new snd::Playlist[[collections count]];
		mNumPlaylists = [collections count];
		
		// clear the playlist pointers to null
		for( i=0; i < mNumPlaylists; ++i )
			mPlaylists[i].playListSongs = 0;
		
		for (MPMediaPlaylist *playlist in collections) 
		{
			NSString *playlistTitle = [playlist valueForProperty: MPMediaPlaylistPropertyName];
			//NSLog ( @"%@", playlistTitle );
			
			std::memset( mPlaylists[playListIndex].playlistName, 0, sizeof(char)*core::MAX_PATH );

			[ playlistTitle getCString: (char *)mPlaylists[playListIndex].playlistName maxLength: (core::MAX_PATH-1) encoding: NSNonLossyASCIIStringEncoding ];
			DBGLOG( "Playlist: %d %s\n", playListIndex, mPlaylists[playListIndex].playlistName );
			
			NSArray *songs = [playlist items];
			
			if( [songs count] != 0 )
			{
				if( [songs count] > MAX_SONGS_PER_PLAYLIST )
					mPlaylists[playListIndex].numSongs = MAX_SONGS_PER_PLAYLIST;
				else 
					mPlaylists[playListIndex].numSongs = [songs count];

				mPlaylists[playListIndex].playListSongs = new snd::Song[mPlaylists[playListIndex].numSongs];
			
				for (MPMediaItem *song in songs)
				{
					if( songIndex < MAX_SONGS_PER_PLAYLIST )
					{
						NSString *songTitle = [song valueForProperty: MPMediaItemPropertyTitle];
						//NSLog (@"\t\t%@", songTitle);
					
						std::memset( mPlaylists[playListIndex].playListSongs[songIndex].songName, 0, sizeof(char)*core::MAX_PATH );
					
						[songTitle getCString: (char *)mPlaylists[playListIndex].playListSongs[songIndex].songName maxLength: (core::MAX_PATH-1) encoding: NSNonLossyASCIIStringEncoding ];
					
						DBGLOG( "\tSong: %d %s\n", songIndex, mPlaylists[playListIndex].playListSongs[songIndex].songName );

						// increase song index
					
						songIndex++;
					}
					else 
						break;
				}
			}
			
			// increase playlist index
			playListIndex++;
			// reset song index
			songIndex = 0;
		}
	}
	
	// assign a playback queue containing all media items on the device
	//[self.mPlayer setQueueWithQuery: [MPMediaQuery playlistsQuery]];
}

/////////////////////////////////////////////////////
/// Method: SetShuffleState
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) SetShuffleState: (bool) state
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	if( state )
		[self.mPlayer setShuffleMode: MPMusicShuffleModeDefault];	
	else
		[self.mPlayer setShuffleMode: MPMusicShuffleModeOff];
}

/////////////////////////////////////////////////////
/// Method: GetShuffleState
/// Params: None
/// 
/////////////////////////////////////////////////////
-(bool) GetShuffleState;
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return false;
	
	switch( [self.mPlayer shuffleMode] )
	{
		case MPMusicShuffleModeDefault:
			return true;
		break;
		case MPMusicShuffleModeOff:
			return false;
		break;
		case MPMusicShuffleModeSongs:
			return true;
		break;
		case MPMusicShuffleModeAlbums:
			return true;
		break;
		default:
			return false;
			break;
	}
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetRepeatState
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) SetRepeatState: (bool) state
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	if( state )
		[self.mPlayer setRepeatMode: MPMusicRepeatModeDefault];
	else
		[self.mPlayer setRepeatMode: MPMusicRepeatModeNone];
}

/////////////////////////////////////////////////////
/// Method: GetRepeatState
/// Params: None
/// 
/////////////////////////////////////////////////////
-(bool) GetRepeatState
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return false;
	
	switch( [self.mPlayer repeatMode] )
	{
		case MPMusicRepeatModeDefault:
			return true;
			break;
		case MPMusicRepeatModeNone:
			return false;
			break;
		case MPMusicRepeatModeOne:
			return true;
			break;
		case MPMusicRepeatModeAll:
			return true;
			break;
		default:
			return false;
			break;
	}
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: IsPlaying
/// Params: None
/// 
/////////////////////////////////////////////////////
-(bool) IsPlaying
{
	return mIsPlaying;
}

/////////////////////////////////////////////////////
/// Method: IsPaused
/// Params: None
/// 
/////////////////////////////////////////////////////
-(bool) IsPaused
{
	return mIsPaused;
}

/////////////////////////////////////////////////////
/// Method: IsUsingPlaylist
/// Params: None
/// 
/////////////////////////////////////////////////////
-(bool) IsUsingPlaylist
{
	return mUsingPlaylists;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(const char*) GetCurrentSong
{
	return &mCurrentSongName[0];
}

/////////////////////////////////////////////////////
/// Method: GetPlaylistName
/// Params: None
/// 
/////////////////////////////////////////////////////
-(const char*) GetPlaylistName: (int)playlistIndex
{
	DBG_ASSERT( playlistIndex < mNumPlaylists );
	
	return &mPlaylists[playlistIndex].playlistName[0];
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfPlaylists
/// Params: None
/// 
/////////////////////////////////////////////////////
-(int) GetTotalNumberOfPlaylists
{
	return mNumPlaylists;
}

/////////////////////////////////////////////////////
/// Method: GetSongName
/// Params: None
/// 
/////////////////////////////////////////////////////
-(const char*) GetSongName: (int)playlistIndex whichSong: (int)songIndex
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return 0;
	
	DBG_ASSERT( playlistIndex < mNumPlaylists );
    
    if( mPlaylists[playlistIndex].numSongs == 0 )
        return 0;
    
	DBG_ASSERT( songIndex < mPlaylists[playlistIndex].numSongs );
	
	return &mPlaylists[playlistIndex].playListSongs[songIndex].songName[0];	
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfPlaylists
/// Params: None
/// 
/////////////////////////////////////////////////////
-(int) GetTotalNumberOfPlaylists: (int)playlistIndex
{
	return mNumPlaylists;
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfSongsInPlaylist
/// Params: None
/// 
/////////////////////////////////////////////////////
-(int) GetTotalNumberOfSongsInPlaylist: (int)playlistIndex
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return 0;
	
	DBG_ASSERT( playlistIndex < mNumPlaylists );
	
	return mPlaylists[playlistIndex].numSongs;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentPlaylistIndex
/// Params: None
/// 
/////////////////////////////////////////////////////
-(int) GetCurrentPlaylistIndex
{
	return mCurrentPlaylistIndex;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSongIndex
/// Params: None
/// 
/////////////////////////////////////////////////////
-(int) GetCurrentSongIndex
{
	return mCurrentSongIndex;
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) PlaySong: (int)playlistIndex whichSong: (int)songIndex
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	DBG_ASSERT( playlistIndex < mNumPlaylists );
    
    if( mPlaylists[playlistIndex].numSongs == 0 )
        return;
    
	DBG_ASSERT( songIndex < mPlaylists[playlistIndex].numSongs );

	NSString* requestedPlaylistName = [ NSString stringWithCString:mPlaylists[playlistIndex].playlistName encoding:NSNonLossyASCIIStringEncoding];
	NSString* requestedSongName = [ NSString stringWithCString:mPlaylists[playlistIndex].playListSongs[songIndex].songName encoding:NSNonLossyASCIIStringEncoding];
	
	MPMediaQuery *myPlaylistsQuery = [MPMediaQuery playlistsQuery];
	NSArray *playlists = [myPlaylistsQuery collections];
	for (MPMediaPlaylist *playlist in playlists) 
	{
		NSString* playlistName = [playlist valueForProperty: MPMediaPlaylistPropertyName];
		
		// playlist name match
		if ([playlistName isEqualToString:requestedPlaylistName]) 
		{
			NSArray *songs = [playlist items];
		
			for (MPMediaItem *song in songs) 
			{
				// song name match
				NSString *songTitle = [song valueForProperty: MPMediaItemPropertyTitle];
				if( [songTitle isEqualToString:requestedSongName] )
				{
					[self.mPlayer setQueueWithItemCollection:[[mMediaList collections] objectAtIndex:playlistIndex]];
															  
					// prepare the playlist with the specific song
					[self.mPlayer setNowPlayingItem:song];

					mCurrentPlaylistIndex = playlistIndex;
					mCurrentSongIndex = songIndex;
					
					mUsingPlaylists = true;
				}
			}
		}
	}
	
	[self.mPlayer play];
	
	mCurrentPlaylistIndex = playlistIndex;
	mCurrentSongIndex = songIndex;
	
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) PlaySong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	[self.mPlayer play];
}

/////////////////////////////////////////////////////
/// Method: PauseSong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) PauseSong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
	{
		if( mIsPlaying )
		{
			[self.mAVPlayer pause];
			mIsPaused = true;
		}
	}
	else if( playerMode == snd::MusicMode_PlaylistPlayer )
	{
		[self.mPlayer pause];
	}
}

/////////////////////////////////////////////////////
/// Method: ResumeSong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) ResumeSong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
	{
		if( mIsPlaying )
		{
			[self.mAVPlayer play];
			mIsPaused = false;
		}
	}
	else if( playerMode == snd::MusicMode_PlaylistPlayer )
	{
		[self.mPlayer play];
	}
}

/////////////////////////////////////////////////////
/// Method: Stop
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) StopSong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
	{
		if( mIsPlaying )
		{
			mFinishedTrack = true;
			mIsPlaying = false;
			mIsPaused = false;
			[self.mAVPlayer stop];
		}
	}
	else if( playerMode == snd::MusicMode_PlaylistPlayer )
	{
		[self.mPlayer stop];
	}
}

/////////////////////////////////////////////////////
/// Method: SelectPreviousSong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) SelectPreviousSong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	[self.mPlayer skipToPreviousItem];
}

/////////////////////////////////////////////////////
/// Method: SelectNextSong
/// Params: None
/// 
/////////////////////////////////////////////////////
-(void) SelectNextSong
{
	if( playerMode == snd::MusicMode_SingleFilePlayer )
		return;
	
	[self.mPlayer skipToNextItem];
}

/////////////////////////////////////////////////////
/// Method: setMusicCallback
/// Params: None
/// 
/////////////////////////////////////////////////////
- (void) setMusicCallback: (snd::MusicCallback *)callback
{
	mCallback = callback;
}

/////////////////////////////////////////////////////
/// Method: handleNowPlayingItemChanged
/// Params: None
/// 
/////////////////////////////////////////////////////
- (void)handleNowPlayingItemChanged:(NSNotification*)notification
{
	// update
	mCurrentSong = [self.mPlayer nowPlayingItem ];
	
	std::memset( mCurrentSongName, 0, sizeof(char)*core::MAX_PATH );
	
	NSString *songName = [ mCurrentSong valueForProperty:MPMediaItemPropertyTitle ];
	NSLog ( @"handleNowPlayingItemChanged: Playing Song %@", songName );
	
	[ songName getCString: (char *)mCurrentSongName maxLength: (core::MAX_PATH-1) encoding: NSNonLossyASCIIStringEncoding ];	
	DBGLOG( "handleNowPlayingItemChanged: PLAYING SONG: %s\n", mCurrentSongName );	
	
	if( mUsingPlaylists )
	{
		// find out which track was playing to update the index
		
	}

	if( mCallback != nil )
	{
		mCallback->PlayingSongChanged();
	}
}

/////////////////////////////////////////////////////
/// Method: handlePlaybackStateChanged
/// Params: None
/// 
/////////////////////////////////////////////////////
- (void)handlePlaybackStateChanged:(NSNotification*)notification
{
	[self UpdatePlaybackState];

	if( mCallback != nil )
	{
		mCallback->PlaybackStateChanged();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: [in]filename, [in]loop
///
/////////////////////////////////////////////////////
-(void) PlaySong: (const char*)filename loopFlag: (bool)loop
{
	if( playerMode == snd::MusicMode_PlaylistPlayer )
		return;
	
	file::TFile fileSplit;
	file::CreateFileStructure( filename, &fileSplit );

	// get rid of the dot in the extension
	char extension[core::MAX_EXT];
	std::memset( extension, 0, sizeof(char)*core::MAX_EXT );

	for( int i=1; i < std::strlen(fileSplit.szFileExt); ++i )
	{
		extension[i-1] = fileSplit.szFileExt[i];
	}

	std::memset( mCurrentSongName, 0, sizeof(char)*core::MAX_PATH );
	snprintf( mCurrentSongName, core::MAX_PATH, "%s", fileSplit.szFile );
	
	NSString *directory = [ NSString stringWithUTF8String: fileSplit.szPath ];
	NSString *fileOnly = [ NSString stringWithUTF8String: fileSplit.szFile ];
	NSString *fileExtOnly = [ NSString stringWithUTF8String: extension ];
	 
	NSString *soundFilePath = [[NSBundle mainBundle] pathForResource: fileOnly ofType: fileExtOnly inDirectory: directory];
	NSURL *fileURL = [[NSURL alloc] initFileURLWithPath: soundFilePath];

	if( [self.mAVPlayer initWithContentsOfURL: fileURL error:nil ] != nil )
	{
		[self.mAVPlayer setDelegate: self];
		[self.mAVPlayer prepareToPlay];
		if( loop )
			[self.mAVPlayer setNumberOfLoops: -1];
		[self.mAVPlayer play];

		mFinishedTrack = false;
		mIsPlaying = true;
		mIsPaused = false;
	}
}

/////////////////////////////////////////////////////
/// Method: SetVolume
/// Params: [in]vol
///
/////////////////////////////////////////////////////
-(void) SetVolume: (float)vol
{
	if( playerMode == snd::MusicMode_PlaylistPlayer )
		return;
	
	if( mIsPlaying )
	{	
		// bit louder
		vol *= 2.0f;
		
		if( vol > 1.0f )
			vol = 1.0f;
		
		[self.mAVPlayer setVolume:vol];
	}
}

/////////////////////////////////////////////////////
/// Method: HasFinishedTrack
/// Params: None
///
/////////////////////////////////////////////////////
-(bool) HasFinishedTrack
{
	return(mFinishedTrack);
}

/////////////////////////////////////////////////////
/// Method: audioPlayerDecodeErrorDidOccur
/// Params: [in]player, [in]error
/// - there is an error
/////////////////////////////////////////////////////
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error
{
	NSLog( @"BGMPlayer Error: @%@ \n", [error localizedDescription] );
}

/////////////////////////////////////////////////////
/// Method: audioPlayerBeginInterruption
/// Params: [in]player
/// - called when there's an incoming phone call
/////////////////////////////////////////////////////
- (void)audioPlayerBeginInterruption:(AVAudioPlayer *)player
{
	if( mIsPlaying )
	{
		[player pause];	
	}
}

/////////////////////////////////////////////////////
/// Method: audioPlayerEndInterruption
/// Params: [in]player
/// - called when phone call has finished
/////////////////////////////////////////////////////
- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player
{
	if( mIsPlaying )
	{	
		[player play];	
	}
}

/////////////////////////////////////////////////////
/// Method: audioPlayerDidFinishPlaying
/// Params: [in]player, [in]completed
///
/////////////////////////////////////////////////////
- (void) audioPlayerDidFinishPlaying: (AVAudioPlayer *) player successfully: (BOOL) completed 
{
    if (completed == YES) 
	{
		mFinishedTrack = true;
		mIsPlaying = false;
    }
}

@end

#endif // BASE_PLATFORM_iOS

