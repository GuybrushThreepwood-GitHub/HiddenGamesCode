
/*===================================================================
	File: MusicMediaPlayer.java
	Game: 

	(C)Hidden Games
=====================================================================*/

package uk.co.hiddengames.cabbyfree;

import java.util.ArrayList;

import android.app.Activity;
import android.content.ContentResolver;
import android.database.Cursor;
import android.database.DatabaseUtils;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;


public class MusicMediaPlayer implements OnCompletionListener {
	
	static public class Song {
		  public int songIndex;
		  public String songName;	
		  public String songId;
	}

	static public class Playlist {
		  public long playlistId;
		  public int playListIndex;
		  public String playlistName;
		  public int numSongs;
		  public ArrayList<Song> songList;
	}
	
	public static native void songChanged( String songName );
	
	static Activity mActivity;
	static MediaPlayer mMediaPlayer;
	static boolean mIsUsingplaylists;
	static boolean mIsPlaying;
	static boolean mIsPaused;
	static int mShuffleMode;
	static int mRepeatState;
	
	static ArrayList<Playlist> mPlayLists;
	static ArrayList<Song> mTempSongList;
	static Cursor mPlayListsCursor;
	static String mCurrentPlaylistName;
	static String mCurrentSongName;
	static int mCurrentPlaylistIndex;
	static int mCurrentSongIndex;
	
	public MusicMediaPlayer(Activity activity) {
    	mActivity = activity;
    	mMediaPlayer = new MediaPlayer();
    	mMediaPlayer.setOnCompletionListener(this);
    	
    	mIsPlaying = false;
    	mIsPaused = false;

    	mIsUsingplaylists = false;
    	
    	mCurrentPlaylistName = null;
    	mCurrentSongName = null;	
    	
    	mCurrentPlaylistIndex = -1;
    	mCurrentSongIndex = -1;
    	
    	UpdatePlaylists();
    }	
    
    public static void UpdatePlaylists()
    {
    	if( mPlayLists != null )
    		mPlayLists.clear();
    	else
    		// allocate
    		mPlayLists = new ArrayList<Playlist>();
    	
    	UpdateGoogleMusicPlaylists();
    	UpdateSDCardPlaylists();
    }
    
    public static String GetSongNameForId( String songId )
    {
    	int i=0;
    	for( i=0; i < mTempSongList.size(); ++i )
    	{
    		Song songInList = mTempSongList.get(i);
    		
    		if( songInList.songId.equals(songId) )
    			return songInList.songName;
    		
    	}
    	return null;
    }
    
    public static void UpdateGoogleMusicPlaylists()
    {
        // GOOGLE MUSIC DIR
    	
    	// first pass, get all the songs
        final ContentResolver resolver = mActivity.getContentResolver();
        Uri uri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
        final String[] columns = { MediaStore.Audio.Media.TITLE, MediaStore.Audio.Media.DATA };
        mTempSongList = new ArrayList<Song>();

        try
        {
        	mPlayListsCursor = resolver.query(uri, columns, null, null, null);    	
        }
        catch( IllegalArgumentException e )
        {
        	mPlayListsCursor = null;        	
        }
        
        if (mPlayListsCursor == null) 
        {
        	Log.d("MUSICMEDIAPLAYER", "Found no google music files.");
        	//mPlayLists = null;
        	return;
        }
        else
        {
        	// Get all the raw file names and path and store them
        	//Log.d("MUSICMEDIAPLAYER", "Songs:");
        	if( mPlayListsCursor.getCount() > 0 )
        	{
        		for (boolean hasItem = mPlayListsCursor.moveToFirst(); hasItem; hasItem = mPlayListsCursor.moveToNext()) 
        		{
        			String songId = mPlayListsCursor.getString(0);
        			String songName = mPlayListsCursor.getString(1);
        			
        	        // add single song
        			Song newSong = new Song();
        	        		
    	        	newSong.songIndex = -1;
    	        	newSong.songName = songName;
    	        	newSong.songId = songId;
    	        			
    	        	mTempSongList.add(newSong);        	        		
        	     }   	          
        	}
        	
        	mPlayListsCursor.close();
        }    
        
        // next pass, get the playlists
            
        uri = Uri.parse("content://com.google.android.music.MusicContent/playlists");
        final String idKey = MediaStore.Audio.Playlists._ID;
        final String nameKey = MediaStore.Audio.Playlists.NAME;        
        final String[] proj = { idKey, nameKey };
        
        try
        {
        	mPlayListsCursor = resolver.query(uri, proj, null, null, null);    	
        }
        catch( IllegalArgumentException e )
        {
        	mPlayListsCursor = null;
        }
        
        if (mPlayListsCursor == null) 
        {
        	Log.d("MUSICMEDIAPLAYER", "Found no google music playlists.");
        	//mPlayLists = null;
        }
        else
        {
        	// Log a list of the playlists.
        	//Log.d("MUSICMEDIAPLAYER", "Playlists:");
        	String playListName = null;
        	
        	//Log.d("MUSICMEDIAPLAYER",  DatabaseUtils.dumpCursorToString(mPlayListsCursor) );
        	
        	if( mPlayListsCursor.getCount() > 0 )
        	{
        		int playListIndex = 0;
        		
        		for (boolean hasItem = mPlayListsCursor.moveToFirst(); hasItem; hasItem = mPlayListsCursor.moveToNext()) 
        		{
        			long playlistID = mPlayListsCursor.getLong(mPlayListsCursor.getColumnIndex(idKey));
        			playListName = mPlayListsCursor.getString(mPlayListsCursor.getColumnIndex(nameKey));
        			//Log.d("MUSICMEDIAPLAYER", playListName);
        			
        			if( !IsPlaylistAlreadyInList( playListName ) )
        			{
	        			final Uri songUri = Uri.parse("content://com.google.android.music.MusicContent/playlists/" + playlistID + "/members/");
	        			String[] songProj = { MediaStore.Audio.Media.TITLE };
	        	        Cursor tracks;
	        	        
	        	        try
	        	        {
	        	        	tracks = resolver.query(songUri, songProj, null, null, null);    	
	        	        }
	        	        catch( IllegalArgumentException e )
	        	        {
	        	        	tracks = null;        	
	        	        }	        				
	        	          
	        	        // create a new playlist object
	        	        Playlist newPlaylist = new Playlist();
	        	        newPlaylist.playlistId = playlistID;
	        	        newPlaylist.playListIndex = playListIndex;
	        	        newPlaylist.playlistName = playListName;
	        	        newPlaylist.numSongs = 0;
	        	        newPlaylist.songList = null;
	        	        
	        	        if (tracks != null) 
	        	        {
	        	        	tracks.moveToFirst();
	        	        	int trackCount = tracks.getCount();
	        	        	
	        	        	// update playlist object
	        	        	newPlaylist.numSongs = trackCount;
	        	        	if( trackCount > 0 )
	        	        	{
	        	        		// allocate song list
	        	        		newPlaylist.songList = new ArrayList<Song>();
	        	        		
	        	        		for( int i=0; i < trackCount; ++i )
	        	        		{
	        	        			Song newSong = new Song();
	        	        			
	        	        			tracks.moveToPosition(i);
	        	        			//Log.d("MUSICMEDIAPLAYER",  DatabaseUtils.dumpCursorToString(tracks) );
	        	        			
	        	        			final String dataPath = tracks.getString(0);
	        	        			//Log.d("MUSICMEDIAPLAYER", dataPath);
	        	        			
	        	        			newSong.songIndex = i;
	        	        			newSong.songName = GetSongNameForId( dataPath );
	        	        			newPlaylist.songList.add(newSong);
	        	        		}
	        	        	}
	        	        	
	        	        	tracks.close();
	        	        } 
	        	        
	        	        // all songs should be in the playlist
	        	        if( newPlaylist.numSongs > 0 )
	        	        {
	        	        	mPlayLists.add(newPlaylist); 
	        	        	playListIndex++;
	        	        }
        			}
        		}        
        	}
        	
        	mPlayListsCursor.close();
        }    
        
        mTempSongList.clear();
    }
    
    public static void UpdateSDCardPlaylists()
    {
        // SD CARD
        final ContentResolver resolver = mActivity.getContentResolver();
        final Uri uri = MediaStore.Audio.Playlists.EXTERNAL_CONTENT_URI;
        final String idKey = MediaStore.Audio.Playlists._ID;
        final String nameKey = MediaStore.Audio.Playlists.NAME;
        final String[] columns = { idKey, nameKey };
        
        try
        {
        	mPlayListsCursor = resolver.query(uri, columns, null, null, null);
        }
        catch( IllegalArgumentException e )
        {
        	mPlayListsCursor = null;
        }
        
        if (mPlayListsCursor == null) 
        {
        	Log.d("MUSICMEDIAPLAYER", "Found no sd card playlists.");
        	//mPlayLists = null;
        	return;
        }
        else
        {
        	// Log a list of the playlists.
        	//Log.d("MUSICMEDIAPLAYER", "Playlists:");
        	String playListName = null;
        	
        	//Log.d("MUSICMEDIAPLAYER",  DatabaseUtils.dumpCursorToString(mPlayListsCursor) );
        	
        	if( mPlayListsCursor.getCount() > 0 )
        	{
        		int playListIndex = 0;
        		
        		for (boolean hasItem = mPlayListsCursor.moveToFirst(); hasItem; hasItem = mPlayListsCursor.moveToNext()) 
        		{
        			long playlistID = mPlayListsCursor.getLong(mPlayListsCursor.getColumnIndex(idKey));
        			playListName = mPlayListsCursor.getString(mPlayListsCursor.getColumnIndex(nameKey));
        			//Log.d("MUSICMEDIAPLAYER", playListName);
        			
        			if( !IsPlaylistAlreadyInList( playListName ) )
        			{
	        	        final Uri songUri = MediaStore.Audio.Playlists.Members.getContentUri("external", playlistID);
	        			final String dataKey = MediaStore.Audio.Media.DATA;
	        			final String[] songProj = { dataKey };
	        	        Cursor tracks;
	        	        
	        	        try
	        	        {
	        	        	tracks = resolver.query(songUri, songProj, null, null, null);
	        	        }
	        	        catch( IllegalArgumentException e )
	        	        {
	        	        	tracks = null;
	        	        }
	        	        
	        	        // create a new playlist object
	        	        Playlist newPlaylist = new Playlist();
	        	        newPlaylist.playlistId = playlistID;
	        	        newPlaylist.playListIndex = playListIndex;
	        	        newPlaylist.playlistName = playListName;
	        	        newPlaylist.numSongs = 0;
	        	        newPlaylist.songList = null;
	        	        
	        	        if (tracks != null) 
	        	        {
	        	        	tracks.moveToFirst();
	        	        	int trackCount = tracks.getCount();
	        	        	
	        	        	// update playlist object
	        	        	newPlaylist.numSongs = trackCount;
	        	        	if( trackCount > 0 )
	        	        	{
	        	        		// allocate song list
	        	        		newPlaylist.songList = new ArrayList<Song>();
	        	        		
	        	        		for( int i=0; i < trackCount; ++i )
	        	        		{
	        	        			Song newSong = new Song();
	        	        			
	        	        			tracks.moveToPosition(i);
	        	        			//Log.d("MUSICMEDIAPLAYER",  DatabaseUtils.dumpCursorToString(tracks) );
	        	        			
	        	        			final int dataIndex = tracks.getColumnIndex(dataKey);
	        	        			final String dataPath = tracks.getString(dataIndex);
	        	        			//Log.d("MUSICMEDIAPLAYER", dataPath);
	        	        			
	        	        			newSong.songIndex = i;
	        	        			newSong.songName = dataPath;
	        	        			//newSong.songId = -1;
	        	        			
	        	        			newPlaylist.songList.add(newSong);
	        	        		}
	        	        	}
	        	        	
	        	        	tracks.close();
	        	        } 
	        	        
	        	        // all songs should be in the playlist
	        	        if( newPlaylist.numSongs > 0 )
	        	        {
	        	        	mPlayLists.add(newPlaylist); 
	        	        	playListIndex++;
	        	        }
        			}
        		}        
        	}
        	
        	mPlayListsCursor.close();
        }    	
    }
    
    public static boolean IsPlaylistAlreadyInList( String playlistName )
    {
    	int i=0;
    	
    	if( mPlayLists != null )
    	{
    		for( i=0; i < mPlayLists.size(); ++ i )
    		{
    			// does this list already exist
    			Playlist list = mPlayLists.get(i);
    			if( list.playlistName.equals(playlistName) )
    				return true;
    		}
    	}
    	
    	return false;
    }
    
    public static void SetShuffleMode( int mode )
    {
    	
    }
    public static void SetRepeatFlag( boolean state )
    {
    	//mMediaPlayer.setLooping(state);
    }

    public static boolean IsPlaying()
    {
    	return mIsPlaying;
    }
    
    public static boolean IsPaused()
    {
    	return mIsPaused;
    }
    
    public static boolean IsUsingPlaylists()
    {
    	return mIsUsingplaylists;
    }
    
    public static String GetCurrentSong()
	{
		return mCurrentPlaylistName;
	}

    public static String GetPlaylistName( int playlistIndex )
	{
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);

    			return list.playlistName;
    		}
    	}
		return null;
	}
	
    public static String GetSongName( int playlistIndex, int songIndex )
	{
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);

    			if( songIndex >= 0 && songIndex < list.songList.size() )
    			{
    				Song songInList = list.songList.get(songIndex);
        			return songInList.songName;	
    			}
    		}
    	}
		return null;      	       
	}
	
	public static int GetTotalNumberOfPlaylists()
	{
		if( mPlayLists != null )
			return mPlayLists.size();
		else
			return 0;
	}
	
	public static int GetTotalNumberOfSongsInPlaylist( int playlistIndex )
	{
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);

    			return list.numSongs;
    		}
    	}
       
		return 0;
	}
	
	public static int GetCurrentPlaylistIndex()
	{
		return mCurrentPlaylistIndex;
	}
	
	public static int GetCurrentSongIndex()
	{
		return mCurrentSongIndex;
	}
	
	public static void PlaySong( int playlistIndex, int songIndex )
	{
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);
    			if( songIndex >= 0 && songIndex < list.songList.size() )
    			{
    				Song songInList = list.songList.get(songIndex);
    		        try {
    		        	mMediaPlayer.reset();
    		        	
    		        	mMediaPlayer.setDataSource(songInList.songName);
        	        	mMediaPlayer.prepare();
        	        	mMediaPlayer.start();	
        	        	
        	        	mCurrentPlaylistName = list.playlistName;
        	        	mCurrentSongName = songInList.songName;
        	        	
        	        	mCurrentPlaylistIndex = playlistIndex;
        	        	mCurrentSongIndex = songIndex;
        	        	
        	        	mIsPlaying = true;
        	        	mIsPaused = false;
        	        	
        	        	mIsUsingplaylists = true;
        	        	
        	        	songChanged(mCurrentSongName);
        	        	
    		        } catch (Exception e) {
        	        	mCurrentPlaylistName = null;
        	        	mCurrentSongName = null;
        	        	mCurrentPlaylistIndex = -1;
        	        	mCurrentSongIndex = -1;
        	        	mIsPlaying = false;
        	        	mIsPaused = false;
    		        	Log.d("MUSICMEDIAPLAYER", "Failed to start MediaPlayer: " + e.getMessage());
    		            return;
    		        }    				

    			}
    		}
    	}		
	}
	
	public static void PlaySong()
	{
        if (mMediaPlayer != null) 
        {
        	if(mMediaPlayer.isPlaying() == false)
        	{
        		mMediaPlayer.start();
        		
        		mIsPaused = false;
        		mIsPlaying = true;
        		mIsUsingplaylists = true;
        		
        	}
        }			
	}
	
	public static void PauseSong()
	{
        if (mMediaPlayer != null) 
        {
        	if(mMediaPlayer.isPlaying())
        	{
        		mMediaPlayer.pause();
        		mIsPaused = true;
        	}
        }		
	}
	
	public static void ResumeSong()
	{
        if (mMediaPlayer != null) 
        {
        	if(mIsPaused == true)
        	{
        		mMediaPlayer.start();
        		mIsPaused = false;
        		mIsPlaying = true;
        	}
        }		
	}
	
	public static void StopSong()
	{
        if (mMediaPlayer != null) 
        {
        	//if(mMediaPlayer.isPlaying())
        	{
        		mMediaPlayer.stop();
        		mIsPaused = false;
        		mIsPlaying = false;
        	}
        }		
	}
	
	public static void SkipToPreviousSong()
	{
		int playlistIndex = mCurrentPlaylistIndex;
		int songIndex = mCurrentSongIndex-1;
		
		if( songIndex < 0 )
			songIndex = 0;
		
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);
    			if( songIndex >= 0 && songIndex < list.songList.size() )
    			{
    				PlaySong( playlistIndex, songIndex);
    			}
    		}
    	}		
	}
	
	public static void SkipToNextSong()
	{
		int playlistIndex = mCurrentPlaylistIndex;
		int songIndex = mCurrentSongIndex+1;
		
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);
    			
    			if( songIndex >= list.songList.size() )
    				songIndex = list.songList.size()-1;
    			
    			if( songIndex >= 0 && songIndex < list.songList.size() )
    			{
    				PlaySong( playlistIndex, songIndex);
    			}
    		}
    	}		
	}

	@Override
	public void onCompletion(MediaPlayer mp) {

		int playlistIndex = mCurrentPlaylistIndex;
		int songIndex = mCurrentSongIndex+1;
		
    	if( mPlayLists != null )
    	{
    		if( playlistIndex >= 0 && playlistIndex < mPlayLists.size() )
    		{
    			Playlist list = mPlayLists.get(playlistIndex);
    			
    			// reset back to the start of the playlist
    			if( songIndex >= list.songList.size() )
    				songIndex = 0;
    			
    			if( songIndex >= 0 && songIndex < list.songList.size() )
    			{
    				PlaySong( playlistIndex, songIndex);
    			}
    		}
    	}			
	}

}
