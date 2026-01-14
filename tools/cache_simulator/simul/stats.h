#ifndef STATS_H
#define STATS_H

#undef min
#undef max

#include <set>
#include <map>
#include <string>

class Statistics {
 private:

  std::set<int> virtualAddr;
  
  std::map<int,int> mapHitL1;
  std::map<int,int> mapMissL1; 
  std::map<int,int> mapHitL2; 
  std::map<int,int> mapMissL2;

  unsigned long nbhits;
  unsigned long nbmisses;
  unsigned long nbrefs;


  unsigned long nbhitsL2;
  unsigned long nbmissesL2;
  unsigned long nbrefsL2;

 public:
  Statistics() {
    nbhits = nbmisses = nbrefs =0;
    nbhitsL2 = nbmissesL2 = nbrefsL2 =0;
  };
  ~Statistics(){;};
  void incrNbHits(int virtualAddrOfCurrentInstruction);// {nbhits++; nbrefs++;}
  void incrNbMisses(int virtualAddrOfCurrentInstruction );// {nbmisses++; nbrefs++;}

  void incrNbHitsL2(int virtualAddrOfCurrentInstruction);// {nbhitsL2++; nbrefsL2++;}
  void incrNbMissesL2(int virtualAddrOfCurrentInstruction);// {nbmissesL2++; nbrefsL2++;}


  void Print(std::string);/* {
    printf("nbrefs %ld nbhits %ld nbmisses %ld\n",nbrefs,nbhits,nbmisses);
    printf("nbrefsL2 %ld nbhitsL2 %ld nbmissesL2 %ld\n",nbrefsL2,nbhitsL2,nbmissesL2);
    }*/
};

#endif
