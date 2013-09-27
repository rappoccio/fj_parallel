#include<iostream>
#include<numeric>
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<time.h>

using namespace std;



class TILING {
public :
  TILING(unsigned int N) : NNdist(N) {}
  double & operator[] (unsigned int j) {return NNdist[j];}

  void compute(int leftbound, int rightbound, int jpt, int intensity){
    for (int j=leftbound; j<rightbound; j++){
      // first do it on this tile
      for (int k=0; k<jpt; k++){
	for (int l=0; l<jpt; l++){
          int RandInt = rand() % 10;
	  //            int RandInt = 6;
	  if (((-1)^RandInt) > 0) {
	    for (int m=0; m<intensity; m++){
	      NNdist[j] = sqrt(RandInt);
	    }
	  }
	  if (RandInt % 3 == 0) {
	    for (int m=0; m<intensity; m++){
	      NNdist[0] = sqrt(j);
	    }
	  }
	}
      }
      // then do it for RH tiles
      for (int k=0; k<jpt; k++){
	for (int l=0; l<4*jpt; l++){
          int RandInt = rand() % 10;
	  //            int RandInt = 6;
	  if (((-1)^RandInt) > 0) {
	    for (int m=0; m<intensity; m++){
	      NNdist[j] = sqrt(RandInt);
	    }
	  }
	  if (RandInt % 3 == 0) {
	    for (int m=0; m<intensity; m++){
	      NNdist[0] = sqrt(j);
	    }
	  }
	}
      }            
    }
  }
  // Updated data entry
protected : 
  std::vector<double> NNdist;
};








///~~~~~~~~~~~~~~~~~///
///**THE SCHEDULER**///
///~~~~~~~~~~~~~~~~~///
int main (int argc, char ** argv){
  unsigned int N = 0;
  if ( argc > 1 ) 
    N = atoi( argv[1] );
  else 
    N = 700;    
  std::cout << "   // set up the initial nearest neighbour information" << std::endl;
  TILING jptTILING(N);
  ///~~~~~~~~~~~~~~~~~~~///
  ///  TIMING FUNCTION  ///
  ///~~~~~~~~~~~~~~~~~~~///
  //  **STEP1****************************************************************
  //  EXECUTE MULTI-THREADED FUNCTION ON MULTIPE CORES
  int C;
  int intensity;


  cout  <<  "Number of cores? ";
  cin >> C;
  //      C=2;
  //
  //      cout << endl << "Intensity? "; 
  //      cin >> intensity;
  intensity = 1;

  for (int runs=0; runs<5; runs++)
    {
      cout << "N=" << N << endl;
      time_t timer1 = time(NULL);
      boost::thread_group TGROUP;
      for (int i=0; i<C; i++){
	TGROUP.create_thread(boost::bind(&TILING::compute,&jptTILING,i*N/C,(i+1)*N/C, N/C, intensity));
      }
      TGROUP.join_all();
      time_t timer2 = time(NULL);
      time_t seconds = difftime(timer2,timer1);
      std::cout << "Elapsed time, parallel : " << seconds << std::endl;
      //  **STEP2****************************************************************
      //  EXECUTE SINGLE THREAD FUNCTION
      jptTILING.compute(0,N,N/C,intensity);
      time_t timer3 = time(NULL);
      seconds = difftime(timer3,timer2);
      std::cout << "Elapsed time, linear once : " << seconds << std::endl;
      //  **STEP3****************************************************************
      //  EXECUTE "MULTI-THREADED" FUNCTION ON ONE CORE
      for (int i=0; i<C; i++){
        jptTILING.compute(i*N/C,(i+1)*N/C, N/C,intensity);
      }
      time_t timer4 = time(NULL);
      seconds = difftime(timer4,timer3);
      std::cout << "Elapsed time, linear " << C << " times : " << seconds << std::endl;
    }
  return 0;
}
