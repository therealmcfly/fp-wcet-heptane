
#include <iostream>
#include <fstream>
#include "stdio.h"
#include "stats.h"
using namespace std;

  void Statistics::incrNbHits(int virtualAddrOfCurrentInstruction) {
	nbhits++;
	nbrefs++;
	virtualAddr.insert(virtualAddrOfCurrentInstruction);
	if(mapHitL1.find(virtualAddrOfCurrentInstruction)!=mapHitL1.end()){
		mapHitL1[virtualAddrOfCurrentInstruction]=mapHitL1[virtualAddrOfCurrentInstruction]+1;
	}else{
		mapHitL1[virtualAddrOfCurrentInstruction]=1;
	}
	
  }
  void Statistics::incrNbMisses(int virtualAddrOfCurrentInstruction ) {
	nbmisses++;
	nbrefs++;
	virtualAddr.insert(virtualAddrOfCurrentInstruction);
	if(mapMissL1.find(virtualAddrOfCurrentInstruction)!=mapMissL1.end()){
		mapMissL1[virtualAddrOfCurrentInstruction]=mapMissL1[virtualAddrOfCurrentInstruction]+1;
	}else{
		mapMissL1[virtualAddrOfCurrentInstruction]=1;
	}

  }

  void Statistics::incrNbHitsL2(int virtualAddrOfCurrentInstruction) {
    nbhitsL2++;
    nbrefsL2++;
    if(mapHitL2.find(virtualAddrOfCurrentInstruction)!=mapHitL2.end()){
      mapHitL2[virtualAddrOfCurrentInstruction]=mapHitL2[virtualAddrOfCurrentInstruction]+1;
    }else{
      mapHitL2[virtualAddrOfCurrentInstruction]=1;
    }
  }
  
  void Statistics::incrNbMissesL2(int virtualAddrOfCurrentInstruction) {
    nbmissesL2++;
    nbrefsL2++;
    if(mapMissL2.find(virtualAddrOfCurrentInstruction)!=mapMissL2.end()){
      mapMissL2[virtualAddrOfCurrentInstruction]=mapMissL2[virtualAddrOfCurrentInstruction]+1;
    }else{
      mapMissL2[virtualAddrOfCurrentInstruction]=1;
    }
  }


  void Statistics::Print(string exitFile) {

	ofstream sortie (exitFile.c_str(), ios::out);

    sortie << "<!DOCTYPE PROGRAM SYSTEM 'simul.dtd'>" << endl;
    sortie << "<EXEC>" << endl;
    set<int>::iterator it;
    for(it=virtualAddr.begin();it!=virtualAddr.end();it++){
      int current = *it;
      int hL1=0;
      int hL2= 0;
      int mL1=0;
      int mL2=0;
      if(mapMissL1.find(current)!=mapMissL1.end()){mL1=mapMissL1[current];}
      if(mapMissL2.find(current)!=mapMissL2.end()){mL2=mapMissL2[current];}
      if(mapHitL1.find(current)!=mapHitL1.end()){hL1=mapHitL1[current];}
      if(mapHitL2.find(current)!=mapHitL2.end()){hL2=mapHitL2[current];}
      //std::cout << "adr: " << current << " L1: " << hL1 << "H " << mL1 << "M" << " L2: " << hL2 << "H " << mL2 << "M\n";
	  //<ATTR type="unsignedlong" name="NBL1H" value="0" />
	  sortie << "<ADDR begin=\""<< current <<"\" >" << endl;
	  sortie << "\t<ATTR type=\"unsignedlong\" name=\"NBL1HEXEC\" value=\"" << hL1 << "\" />" << endl;
	  sortie << "\t<ATTR type=\"unsignedlong\" name=\"NBL1MEXEC\" value=\"" << mL1 << "\" />" << endl;
	  sortie << "\t<ATTR type=\"unsignedlong\" name=\"NBL2HEXEC\" value=\"" << hL2 << "\" />" << endl;
	  sortie << "\t<ATTR type=\"unsignedlong\" name=\"NBL2MEXEC\" value=\"" << mL2 << "\" />" << endl;
	  sortie << "</ADDR>" << endl;
    }
    sortie << "</EXEC>" << endl;
	sortie.close();
	
    printf("nbrefs %ld nbhits %ld nbmisses %ld\n",nbrefs,nbhits,nbmisses);
    printf("nbrefsL2 %ld nbhitsL2 %ld nbmissesL2 %ld\n",nbrefsL2,nbhitsL2,nbmissesL2);
	
  }
