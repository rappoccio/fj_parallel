#include "fastjet/ClusterSequence.hh"
#include <iostream>
using namespace fastjet;
using namespace std;

int main () {

  // Buffers where we will store the pseudojets that get clustered.
  // On each event, these buffers are "cleared", then filled with
  // information from "this" event.
  // They are visible to the manager and all the threads
  std::vector<PseudoJet> buffer1;
  std::vector<PseudoJet> buffer2;
  std::vector<PseudoJet> buffer3;
  std::vector<PseudoJet> buffer4;

  std::vector<MyThread> threads(4);

  // Thread manager will allocate threads and tell them about the buffers
  ThreadManager manager( threads, buffer1, buffer2, buffer3, buffer4 );

  // Allocate the threads necessary to do the computation
  manager.allocate();

  for( auto event = getFromSomewhere();
       event != end; ++event) {

    vector<PseudoJet> particles;
    // an event with three particles:   px    py  pz      E
    particles.push_back( PseudoJet(   99.0,  0.1,  0, 100.0) ); 
    particles.push_back( PseudoJet(    4.0, -0.1,  0,   5.0) ); 
    particles.push_back( PseudoJet(  -99.0,    0,  0,  99.0) );

    // choose a jet definition
    double R = 0.7;
    JetDefinition jet_def(antikt_algorithm, R);

    // run the clustering, extract the jets
    ClusterSequence cs(particles, jet_def, &manager);
    vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

    // print out some infos
    cout << "Clustering with " << jet_def.description() << endl;

    // print the jets
    cout <<   "        pt y phi" << endl;
    for (unsigned i = 0; i < jets.size(); i++) {
      cout << "jet " << i << ": "<< jets[i].perp() << " " 
	   << jets[i].rap() << " " << jets[i].phi() << endl;
      vector<PseudoJet> constituents = jets[i].constituents();
      for (unsigned j = 0; j < constituents.size(); j++) {
	cout << "    constituent " << j << "'s pt: " << constituents[j].perp()
	     << endl;
      }
    }
  }

  thread1.join();
  thread2.join();
  thread3.join();
  thread4.join();


} 

/// The new signature for clustersequence will be 
ClusterSequence::ClusterSequence( std::vector<fastjet::Pseudojet> const & particles,
				  fastjet::JetDefinition const & jetdef,
				  ThreadManager * manager = 0 )
{
  if ( manager != 0 ) {
    // Do parallel stuff
    

    // 1. split up the pseudojets by whatever metric (eta slices? randomly? by last night's baseball scores?)
    std::vector<PseudoJet> slice1;
    std::vector<PseudoJet> slice2;
    std::vector<PseudoJet> slice3;
    std::vector<PseudoJet> slice4;

    copy_to_slice1( particles, slice1);
    // etc


    
    // 2. Manager pushes data to the buffers
    manager->move( slice1 ); // takes "ownership" of slice1's data, pushes to buffer1

    // 3. threads process :
    ///    Each thread will pull data from the buffers, then process.
    ///    After, can "push" back clustered jets (implementation detail)
    std::vector<PseudoJet> results1;
    std::vector<PseudoJet> results2;
    std::vector<PseudoJet> results3;
    std::vector<PseudoJet> results4;
    manager->process(results1, results2, results3, results4);

    // 4. sanity checks (physics)
    // (Don't worry about this yet)

    // 5. manager pulls back the data
    std::vector<PseudoJet> the_inclusive_jets = manager->splice( results1,results2, results3, results4);
    
    
  }
  else {
    // Do regular stuff
  }


  // Down here, ready to call "inclusive_jets" in short-example.cc
}
