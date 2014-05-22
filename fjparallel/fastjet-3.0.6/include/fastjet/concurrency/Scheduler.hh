#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
//#include "Scheduler.hh"


/*
all Scheduler objects are preceeded with "s_"
     "s_someobject"

s_multicore
s_firstevent

s_flag
s_flagmtx
s_top
s_bottom

s_tilethreads
s_tiles
s_tilelocks

*/

bool s_multicore = false ; // user sets this to true if concurrency is desired
bool s_firstevent = true ; // fastjet sets this to false after fist event completes

int s_flag = 0 ;
boost::mutex s_flagmtx , s_bottommtx , s_topmtx ;
bool s_top = false ;
bool s_bottom = false ;

boost::thread_group s_tilethreads ;
std::vector<fastjet::ClusterSequence::Tile> s_tiles ;
std::vector<boost::mutex*> s_tilelocks ;


/*
all Scheduler methods are preceeded with "s_" are are all caps
     "s_SOME_METHOD"

OLD METHODS:

void SEND_TILES_TO_THREADS( ) ; TODO ***DONE s_CREATE_THREADS

void CLEAN_TILES( vector<Tile> _tiles ) ;   TODO 

void WAIT_FOR_THREADS() ; TODO ***DONE

void RESET_FLAG() ; TODO ***DONE

void TELL_MAIN_IM_READY() ;  TODO s_NOTIFY_MAIN ***DONE

void NN_INITIALIZATION( Tile * tilee ) ;  TODO s_NN_INIT
*/

void s_NN_INIT(fastjet::ClusterSequence::Tile *tile ){//one thread per tile

void st_COUT_FLAG( std::string S ){
  s_flagmtx.lock() ;
  std::cout << S << s_flag ;
  s_flagmtx.unlock() ;
}

void s_RESET_FLAG(){
  s_flagmtx.lock() ;
  s_flag = s_tiles.size() ;
  s_flagmtx.unlock() ;
}

void CLEAN_TILES(){
  for ( int i = 0 ; i < s_tiles.size() ; ++i ){
    s_tiles[i].head = NULL ;
    s_tiles[i].tagged = false ;
  }
}

void s_CREATE_THREADS(){
  s_RESET_FLAG() ;
  for ( int i = 0 ; i < s_tiles.size() ; ++i ) {
    fastjet::ClusterSequence::Tile * tile = &(s_tiles[i]) ;
    tile->address = i ;
    boost::mutex * _mtxqaz ;
    s_tilelocks.push_back( _mtxqaz ) ;
    s_tilethreads.create_thread(  boost::bind(&s_NN_INIT , tile)  ) ;
  }
}

void s_NOTIFY_MAIN(){
  s_flagmtx.lock() ;
  --s_flag ;
  s_flagmtx.unlock() ;
}

void s_WAIT_FOR_THREADS(){
  while(1){
    s_flagmtx.lock() ;
    if ( s_flag == 0 ){
      s_flagmtx.unlock() ;
      break ;
    }
    s_flagmtx.unlock() ;
  }
}
/*
void s_NN_INIT(){//written for small number of cores
  TiledJet * jetA , * jetB ;
  while(1){
    s_NOTIFY_MAIN() ;  
    while(1){
      s_topmtx.lock() ;
      if(s_top) {break;}
      s_toplock.unlock() ;
    }
    st_COUT_FLAG(" go: ") ;
      // set up the initial nearest neighbour information
      vector<Tile>::const_iterator tile;
      for (tile = _tiles->begin(); tile != _tiles->end(); tile++) {
        s_tilelocks[tile->address]->lock() ;
        // first do it on this tile
        for (jetA = tile->head; jetA != NULL; jetA = jetA->next) {
          for (jetB = tile->head; jetB != jetA; jetB = jetB->next) {
            double dist = _bj_dist(jetA,jetB);
            if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
            if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
          }
        }
        s_tilelocks[tile->address]->unlock() ;
        // then do it for RH tiles
        for (Tile ** RTile = tile->RH_tiles; RTile != tile->end_tiles; RTile++) {
	  bool lock1 = false ;
	  bool lock2 = false ;
	  while(1){
            lock1 = s_tilelocks[tile->address]->try_lock() ;
            lock2 = s_tilelocks[(*RTile)->address]->try_lock() ;
	    if ((lock1) && (lock2)) { break ; }
	    else{
              if(lock1) { s_tilelocks[tile->address]->unlock() ; }
              if(lock2) { s_tilelocks[(*RTile)->address]->unlock() ; }
	    }
          }
          for (jetA = tile->head; jetA != NULL; jetA = jetA->next) {
            for (jetB = (*RTile)->head; jetB != NULL; jetB = jetB->next) {
              double dist = _bj_dist(jetA,jetB);
              if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
              if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
            }
          }
          s_tilelocks[tile->address]->unlock() ;
          s_tilelocks[(*RTile)->address]->unlock() ;
        }
      }
    while(1){
      s_bottommtx.lock() ;
      if(s_bottom) {break;}
      s_bottomlock.unlock() ;
    }
    st_COUT_FLAG(" done: ") ;
  }//end while(1)
}
*/




void s_NN_INIT(fastjet::ClusterSequence::Tile *tile ){//one thread per tile
  fastjet::ClusterSequence::TiledJet * jetA ;
  fastjet::ClusterSequence::TiledJet * jetB ;
  while(1){
    s_NOTIFY_MAIN() ;  
    while(1){
      s_topmtx.lock() ;
      if(s_top) {break;}
      s_topmtx.unlock() ;
    }
    st_COUT_FLAG(" go: ") ;
      // set up the initial nearest neighbour information
        s_tilelocks[tile->address]->lock() ;
        // first do it on this tile
        for (jetA = tile->head; jetA != NULL; jetA = jetA->next) {
          for (jetB = tile->head; jetB != jetA; jetB = jetB->next) {
            double dist = _bj_dist(jetA,jetB);
            if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
            if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
          }
        }
        s_tilelocks[tile->address]->unlock() ;
        // then do it for RH tiles
        for (fastjet::ClusterSequence::Tile ** RTile = tile->RH_tiles; RTile != tile->end_tiles; RTile++) {
	  bool lock1 = false ;
	  bool lock2 = false ;
	  while(1){
            lock1 = s_tilelocks[tile->address]->try_lock() ;
            lock2 = s_tilelocks[(*RTile)->address]->try_lock() ;
	    if ((lock1) && (lock2)) { break ; }
	    else{
              if(lock1) { s_tilelocks[tile->address]->unlock() ; }
              if(lock2) { s_tilelocks[(*RTile)->address]->unlock() ; }
	    }
          }
          for (jetA = tile->head; jetA != NULL; jetA = jetA->next) {
            for (jetB = (*RTile)->head; jetB != NULL; jetB = jetB->next) {
              double dist = _bj_dist(jetA,jetB);
              if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
              if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
            }
          }
          s_tilelocks[tile->address]->unlock() ;
          s_tilelocks[(*RTile)->address]->unlock() ;
        }
      }
    while(1){
      s_bottommtx.lock() ;
      if(s_bottom) {break;}
      s_bottommtx.unlock() ;
    }
    st_COUT_FLAG(" done: ") ;
  }//end while(1)
}













