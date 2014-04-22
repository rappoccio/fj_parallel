


void INIT_THREADS( vector< vector<Tile> *> & buffs, vector< mutex > & locks  )
{
  for ( int i = 0 ; i < NCORES ; i++ )
  {
    // Some how you have to keep track of pbuff!
    std::vector<Tile> * pbuff = new std::vector<Tile>() ;
    mylock = getMutexFromSomewhere(); 
    buffs.push_back( pbuff );
    locks.push_back( mylock );

    TGROUP.create_thread( boost::bind( &NN_INITIALIZATION , pbuff ) ) ;
  }
  
}



void SEND_TILES_TO_THREADS( vector<Tile> const & _tiles, vector< vector<Tile> * > & buffs, vector<mutex> & locks )
{
  // Steps : 
  // 1. lock buffer (stored in "buffs"
  // 2. copy tiles to buffer
  // 3. unlock buffer
  // 4. Signal "computation" to go 
  // 5. Access result


  for ( int nthread = 0 ; nthread < NTHREAD; ++nthread ) {
    // 1. Lock buffer
    locks[nthread].lock();

    // 2. Copy the data
    // Here you do the tile ---> buffer allocation logic you prefer. 
    // This is what we will play with! 
    copy( _tiles.begin(), _tiles.end(), buffs[nthread].begin() );  // Check the syntax ??


    // 3. Unlock buffer
    
    locks[nthread].unlock();
  }

  // 4. Signal computation to "go"
  // (Maybe as simple as waiting for buffer.size() > 0

  // 5. Get the result
  // 
  // Clients lock the buffer
  // Clients release the data
  // Clients unlock the buffer


  
  

}

void CLEAN_TILES() // remove bj from tiles???
{

  // Wait until all calculations are done (critical!)
  // 

  for (  vector< vector<Tile> *>::iterator i = buffs.begin(); i != buffs.end(); ++i ) {
    if ( *i != 0 ) delete *i;
  }
}

void WAIT_FOR_THREADS()
{
  while( 1 ) {

    lock();
    check();
    unlock(); 
/*  WAIT HERE  */} //if threads are still working, wait here
}

void RESET_FLAG()
{
  _mtx.lock() ;
  flag = _tiles.size() ;
  _mtx.unlock() ;
}

void TELL_MAIN_IM_READY()
{
  _mtx.lock() ;
  flag-- ;  //decrement flag, since main's waiting for flag=0 (meaning, zero threads are still working)
  _mtx.unlock() ;
}

void NN_INITIALIZATION( Tile * tilee )
{
  /cout << "W" ; //sees if a thread is actually working
  TiledJet * jetA ;
  TiledJet * jetB ;
  while( true ) // LOOP THIS PROCESS FOR MANY EVENTS
  {
    TELL_MAIN_IM_READY() ;  //tell the MAIN you're waiting at the top
    while( TOP == false ) {/*  WAIT HERE  */}

      _tile_mutex[ tilee->address ]->lock() ;
      // first do it on this tile
      for (jetA = tilee->head; jetA != NULL; jetA = jetA->next) {
        for (jetB = tilee->head; jetB != jetA; jetB = jetB->next) {
	  double dist = _bj_dist(jetA,jetB);
          if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
	  if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
        }
      }
      _tile_mutex[ tilee->address ]->unlock() ;

      // then do it for RH tiles
      for (Tile ** RTilee = tilee->RH_tiles; RTilee != tilee->end_tiles; RTilee++) {
        int locks = 0 ;
        while ( locks < 2 )
        {
          locks = 0 ;
          if ( _tile_mutex[ tilee->address ]->try_lock() )
            { locks++ ; }
          if ( _tile_mutex[ (*Rtilee)->address ]->try_lock() )
            { locks++ ; }
          if ( locks != 2 )
            {
              _tile_mutex[ tilee->address ]->unlock() ;
              _tile_mutex[ (*Rtilee)->address ]->unlock() ;
            }
        }
        for (jetA = tilee->head; jetA != NULL; jetA = jetA->next) {
	  for (jetB = (*RTilee)->head; jetB != NULL; jetB = jetB->next) {
	    double dist = _bj_dist(jetA,jetB);
  	    if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
	    if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
	  }
        }
        _tile_mutex[ tilee->address ]->unlock() ;
        _tile_mutex[ (*Rtilee)->address ]->unlock() ;
      }
    }
    while( BOTTOM == false ) {/*  WAIT HERE  */}
  }
  /cout << "d" ; //sees if a thread is actually working
}


