#include <TColor.h>

#include "MUONDigitsInfo.h"

#include <AliMUONDigit.h>
#include <AliMUONGlobalTrigger.h>
#include <AliMUONLocalTrigger.h>

using namespace Reve;
using namespace Alieve;
using namespace std;

ClassImp(MUONDigitsInfo)

/**************************************************************************/
MUONDigitsInfo:: ~MUONDigitsInfo() 
{

}

/**************************************************************************/
void MUONDigitsInfo::Init()
{

  fDTree = 0;
  fRTree = 0;
  fTTree = 0;

  //CreateColors();

}

/**************************************************************************/
void MUONDigitsInfo::SetDTree(TTree* tree)
{
    
  static const Exc_t eH("MUONDigitsInfo::SetDTree ");

  fDTree = tree;

}

/**************************************************************************/
void MUONDigitsInfo::SetRTree(TTree* tree)
{
    
  static const Exc_t eH("MUONDigitsInfo::SetRTree ");

  fRTree = tree;

}

/**************************************************************************/
void MUONDigitsInfo::SetTTree(TTree* tree)
{
    
  static const Exc_t eH("MUONDigitsInfo::SetTTree ");

  fTTree = tree;

}

/**************************************************************************/
TClonesArray* MUONDigitsInfo::GetDigits(Int_t chamber)
{

  Char_t branchname[30];

  sprintf(branchname,"MUONDigits%d",chamber);
  TClonesArray *digits = 0;
  fDTree->SetBranchAddress(branchname,&digits);
  fDTree->GetEntry(0);  // load event 0

  return digits;

}

/**************************************************************************/
TClonesArray* MUONDigitsInfo::GetClusters(Int_t chamber)
{

  if (chamber > 10) return 0;

  Char_t branchname[30];

  sprintf(branchname,"MUONRawClusters%d",chamber);
  TClonesArray *clusters = 0;
  fRTree->SetBranchAddress(branchname,&clusters);
  fRTree->GetEntry(0);  // load event 0

  return clusters;

}

/**************************************************************************/
TClonesArray* MUONDigitsInfo::GetTracks()
{

  Char_t branchname[30];

  sprintf(branchname,"MUONTrack");
  TClonesArray *tracks = 0;
  fTTree->SetBranchAddress(branchname,&tracks);
  fTTree->GetEntry(0);  // load event 0

  return tracks;

}

/*****************************************************************************/
void MUONDigitsInfo::CreateColors()
{
//    Create the colors palette used to display clusters

    Int_t k,i;
    Int_t color;
    Float_t r,g,b;
    
    for (k=1;k<=5;k++) {

	switch(k) {
	case 1:
	    for (i=1;i<=5;i++) {
		r=1.;
		g=i*0.2;  
		b=0.;
		color=i;
		color=260+23-color;
		new TColor(color,r,g,b);
	    } 
	    break;
	case 2:
	    for (i=1;i<=4;i++) {
		r=1.1-i*0.2;
		g=1.;  
		b=0.;
		color=i+5;
		color=260+23-color;
		new TColor(color,r,g,b);
	    } 
	    break;
	case 3:
	    for (i=1;i<=4;i++) {
		r=0.;
		g=1.;  
		b=i*0.2+0.2;
		color=i+9;
		color=260+23-color;
		new TColor(color,r,g,b);
	    } 
	    break;
	case 4:
	    for (i=1;i<=4;i++) {
		r=0.;
		g=1.1-i*0.2;  
		b=1.;
		color=i+13;
		color=260+23-color;
		new TColor(color,r,g,b);
	    } 
	    break;
	case 5:
	    for (i=1;i<=5;i++) {
		r=i*0.2;
		g=0.;  
		b=1.;
		color=i+17;
		color=260+23-color;
		new TColor(color,r,g,b);
	    } 
	    break;
	}

    }

}
