#ifndef ALIPHOSFASTRECPARTICLE_H
#define ALIPHOSFASTRECPARTICLE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  A  Particle modified by PHOS response and produced by AliPHOSvFast
//  To become a general class of AliRoot ?    
//               
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---

#include "TParticle.h"
#include "TVector3.h"

// --- Standard library ---

// --- AliRoot header files ---

class AliPHOSFastRecParticle : public TParticle {

 public:
  
  AliPHOSFastRecParticle() ;

  AliPHOSFastRecParticle(const AliPHOSFastRecParticle & rp) ;  // ctor
  AliPHOSFastRecParticle(const TParticle & p) ;  // ctor
  virtual ~AliPHOSFastRecParticle(){
    // dtor
  }
  virtual Int_t DistancetoPrimitive(Int_t px, Int_t py) ; 
  virtual void Draw(Option_t *option) ;  
  virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py) ;
  Int_t GetIndexInList() const { 
    // returns the index of this in the list
    return fIndexInList ; 
  } 
  Int_t GetPrimary(){return fPrimary;}
  
  const Int_t GetType() const { 
    // returns the type of the particle
    return fType ; 
  } 
  
  void SetPIDBit(UInt_t fSet)
  {
  fType |= (1<<fSet) ; 
  } 
   
  Bool_t TestPIDBit(UInt_t fTest){
   if (fType & (1<<fTest) )
     return  kTRUE ;	
   else
     return kFALSE ;
   }

  Bool_t GetPhotonHiPu_LoEf()  {
    Bool_t pid=kFALSE ;
    if(TestPIDBit(8)&&TestPIDBit(7)&&TestPIDBit(6)&& //PCA
       TestPIDBit(5)&&TestPIDBit(4)&&TestPIDBit(3)&& //TOF
       TestPIDBit(2)&&TestPIDBit(1)&&TestPIDBit(0))  //RCPV
      pid = kTRUE;
    return pid ;
  }
  Bool_t GetPhotonMed_Pu_Ef(){
    Bool_t pid=kFALSE ;
    if(TestPIDBit(7)&&TestPIDBit(6)&& //PCA
       TestPIDBit(5)&&TestPIDBit(4)&&TestPIDBit(3)&& //TOF
       TestPIDBit(2)&&TestPIDBit(1)&&TestPIDBit(0))  //RCPV
      pid = kTRUE ;
    return pid ;
  } 
  Bool_t GetPhotonHiEf_LoPu()  {
    Bool_t pid=kFALSE ;
    if(TestPIDBit(6)&& //PCA
       TestPIDBit(5)&&TestPIDBit(4)&&TestPIDBit(3)&& //TOF
       TestPIDBit(2)&&TestPIDBit(1)&&TestPIDBit(0))  //RCPV
      pid = kTRUE ;
    return pid ;
  }

  TString Name() ; 
  virtual void Paint(Option_t * option="");
  virtual void Print(const char * opt) ; 
  void SetPrimary(Int_t index) { // sets the primary particle index
    fPrimary = index ; 
  }
  
  void SetType(Int_t type) { 
    // sets the particle type 
    fType = type ; 
  }	    
 
  void SetIndexInList(Int_t val) { 
    // sets the value of the index in the list 
    fIndexInList = val ; 
  } 

  enum EParticleType { kUNDEFINED=-1,  
		       kCHARGEDHASLOW, kNEUTRALHASLOW, kCHARGEDHAFAST, kNEUTRALHAFAST,
		       kCHARGEDEMSLOW, kNEUTRALEMSLOW, kCHARGEDEMFAST, kNEUTRALEMFAST} ; 

  typedef TClonesArray  FastRecParticlesList ; 

 protected:

  Int_t fIndexInList ; // the index of this RecParticle in the list stored in TreeR (to be set by analysis)
  Int_t fPrimary ;     //  primary particle index 
  Int_t fType ;        // particle type obtained by "virtual" reconstruction

 private:


  ClassDef(AliPHOSFastRecParticle,1)  // Reconstructed Particle produced by the fast simulation 

};

#endif // AliPHOSFASTRECPARTICLE_H
