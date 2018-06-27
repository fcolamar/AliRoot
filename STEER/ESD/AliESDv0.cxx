/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//-------------------------------------------------------------------------
//               Implementation of the ESD V0 vertex class
//            This class is part of the Event Data Summary
//            set of classes and contains information about
//            V0 kind vertexes generated by a neutral particle
//     Origin: Iouri Belikov, IReS, Strasbourg, Jouri.Belikov@cern.ch
//     Modified by: Marian Ivanov,  CERN, Marian.Ivanov@cern.ch
//            and  Boris Hippolyte,IPHC, hippolyt@in2p3.fr 
//-------------------------------------------------------------------------

#include <TMath.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TVector3.h>
#include <TMatrixD.h>

#include "AliLog.h"
#include "AliESDv0.h"
#include "AliESDV0Params.h"
#include "AliKFParticle.h"
#include "AliKFVertex.h"
#include "AliESDVertex.h"

ClassImp(AliESDv0)

const AliESDV0Params  AliESDv0::fgkParams;

AliESDv0::AliESDv0() :
  AliVParticle(),
  fParamN(),
  fParamP(),
  fEffMass(TDatabasePDG::Instance()->GetParticle(kK0Short)->Mass()),
  fDcaV0Daughters(0),
  fChi2V0(0.),
  fRr(0),
  fDistSigma(0),
  fChi2Before(0),
  fChi2After(0),
  fPointAngleFi(0),
  fPointAngleTh(0),
  fPointAngle(0),
  fPdgCode(kK0Short),
  fNidx(0),
  fPidx(0),
  fStatus(0),
  fNBefore(0),
  fNAfter(0),
  fOnFlyStatus(kFALSE)
{
  //--------------------------------------------------------------------
  // Default constructor  (K0s)
  //--------------------------------------------------------------------

  for (Int_t i=0; i<3; i++) {
    fPos[i] = 0.;
    fNmom[i] = 0.;
    fPmom[i] = 0.;
  }

  for (Int_t i=0; i<6; i++) {
    fPosCov[i]= 0.;
  }

  for (Int_t i=0;i<6;i++){fClusters[0][i]=0; fClusters[1][i]=0;}
  fNormDCAPrim[0]=fNormDCAPrim[1]=0;
  for (Int_t i=0;i<3;i++){fAngle[i]=0;}
  for (Int_t i=0;i<4;i++){fCausality[i]=0;}
}

AliESDv0::AliESDv0(const AliESDv0& v0) :
  AliVParticle(v0),
  fParamN(v0.fParamN),
  fParamP(v0.fParamP),
  fEffMass(v0.fEffMass),
  fDcaV0Daughters(v0.fDcaV0Daughters),
  fChi2V0(v0.fChi2V0),
  fRr(v0.fRr),
  fDistSigma(v0.fDistSigma),
  fChi2Before(v0.fChi2Before),
  fChi2After(v0.fChi2After),
  fPointAngleFi(v0.fPointAngleFi),
  fPointAngleTh(v0.fPointAngleTh),
  fPointAngle(v0.fPointAngle),
  fPdgCode(v0.fPdgCode),
  fNidx(v0.fNidx),
  fPidx(v0.fPidx),
  fStatus(v0.fStatus),
  fNBefore(v0.fNBefore),
  fNAfter(v0.fNAfter),
  fOnFlyStatus(v0.fOnFlyStatus)
{
  //--------------------------------------------------------------------
  // The copy constructor
  //--------------------------------------------------------------------
  
  Short_t cN=fParamN.Charge(), cP=fParamP.Charge();
  Bool_t swp = cN>0 && (cN != cP);
  if (swp) {
    fParamN = v0.fParamP;
    fParamP = v0.fParamN;
    fNidx=v0.fPidx;
    fPidx=v0.fNidx;
  }
  
  for (int i=0; i<3; i++) {
    fPos[i]  = v0.fPos[i];
    fNmom[i] = v0.fNmom[i];
    fPmom[i] = v0.fPmom[i];
  }
  for (int i=0; i<6; i++) {
    fPosCov[i]  = v0.fPosCov[i];
  }

  for (Int_t i=0; i<2; i++) {
    fNormDCAPrim[i]=v0.fNormDCAPrim[i];
  }
  if (swp) {
    for (Int_t i=0;i<6;i++){
      fClusters[0][i]=v0.fClusters[1][i]; 
      fClusters[1][i]=v0.fClusters[0][i];
    }
  }
  else {
    for (Int_t i=0;i<6;i++){
      fClusters[0][i]=v0.fClusters[0][i]; 
      fClusters[1][i]=v0.fClusters[1][i];
    }
  }
  for (Int_t i=0;i<3;i++){
      fAngle[i]=v0.fAngle[i];
  }
  for (Int_t i=0;i<4;i++){fCausality[i]=v0.fCausality[i];}
}


AliESDv0::AliESDv0(const AliExternalTrackParam &t1, Int_t i1,
                   const AliExternalTrackParam &t2, Int_t i2) :
  AliVParticle(),
  fParamN(t1),
  fParamP(t2),
  fEffMass(TDatabasePDG::Instance()->GetParticle(kK0Short)->Mass()),
  fDcaV0Daughters(0),
  fChi2V0(0.),
  fRr(0),
  fDistSigma(0),
  fChi2Before(0),
  fChi2After(0),
  fPointAngleFi(0),
  fPointAngleTh(0),
  fPointAngle(0),
  fPdgCode(kK0Short),
  fNidx(i1),
  fPidx(i2),
  fStatus(0),
  fNBefore(0),
  fNAfter(0),
  fOnFlyStatus(kFALSE)
{
  //--------------------------------------------------------------------
  // Main constructor  (K0s)
  //--------------------------------------------------------------------

  //Make sure the daughters are ordered (needed for the on-the-fly V0s)
  Short_t cN=t1.Charge(), cP=t2.Charge();
  Bool_t swp = cN>0 && (cN != cP);
  if (swp) {
    fParamN = t2;
    fParamP = t1;
    fNidx = i2;
    fPidx = i1;
  }
  
  for (Int_t i=0; i<6; i++) {
    fPosCov[i]= 0.;
  }

  //Trivial estimation of the vertex parameters
  Double_t alpha=fParamN.GetAlpha(), cs=TMath::Cos(alpha), sn=TMath::Sin(alpha);
  Double_t tmp[3];
  fParamN.GetPxPyPz(tmp);
  Double_t px1=tmp[0], py1=tmp[1], pz1=tmp[2];
  fParamN.GetXYZ(tmp);
  Double_t  x1=tmp[0],  y1=tmp[1],  z1=tmp[2];
  const Double_t ss=0.0005*0.0005;//a kind of a residual misalignment precision
  Double_t sx1=sn*sn*fParamN.GetSigmaY2()+ss, sy1=cs*cs*fParamN.GetSigmaY2()+ss; 


  alpha=fParamP.GetAlpha(); cs=TMath::Cos(alpha); sn=TMath::Sin(alpha);
  fParamP.GetPxPyPz(tmp);
  Double_t px2=tmp[0], py2=tmp[1], pz2=tmp[2];
  fParamP.GetXYZ(tmp);
  Double_t  x2=tmp[0],  y2=tmp[1],  z2=tmp[2];
  Double_t sx2=sn*sn*fParamP.GetSigmaY2()+ss, sy2=cs*cs*fParamP.GetSigmaY2()+ss; 
    
  Double_t sz1=fParamN.GetSigmaZ2(), sz2=fParamP.GetSigmaZ2();
  Double_t wx1=sx2/(sx1+sx2), wx2=1.- wx1;
  Double_t wy1=sy2/(sy1+sy2), wy2=1.- wy1;
  Double_t wz1=sz2/(sz1+sz2), wz2=1.- wz1;
  fPos[0]=wx1*x1 + wx2*x2; fPos[1]=wy1*y1 + wy2*y2; fPos[2]=wz1*z1 + wz2*z2;

  //fPos[0]=0.5*(x1+x2); fPos[1]=0.5*(y1+y2); fPos[2]=0.5*(z1+z2);
  fNmom[0]=px1; fNmom[1]=py1; fNmom[2]=pz1; 
  fPmom[0]=px2; fPmom[1]=py2; fPmom[2]=pz2;

  for (Int_t i=0;i<6;i++){fClusters[0][i]=0; fClusters[1][i]=0;}
  fNormDCAPrim[0]=fNormDCAPrim[1]=0;
  for (Int_t i=0;i<3;i++){fAngle[i]=0;}
  for (Int_t i=0;i<4;i++){fCausality[i]=0;}
}

static Bool_t GetWeight(TMatrixD &w, const AliExternalTrackParam &t) {
  //
  // Returns the global weight matrix w = Transpose[G2P]*Inverse[Cpar]*G2P ,
  // where the matrix Cpar is the transverse part of the t covariance
  // in "parallel" system (i.e. the system with X axis parallel to momentum).
  // The matrix G2P performs the transformation global -> "parallel". 
  //
  Double_t phi=t.GetAlpha() + TMath::ASin(t.GetSnp());
  Double_t sp=TMath::Sin(phi);
  Double_t cp=TMath::Cos(phi);

  Double_t tgl=t.GetTgl();
  Double_t cl=1/TMath::Sqrt(1.+ tgl*tgl);
  Double_t sl=tgl*cl;

  TMatrixD g2p(3,3); //global --> parallel
  g2p(0,0)= cp*cl; g2p(0,1)= sp*cl; g2p(0,2)=sl;
  g2p(1,0)=-sp;    g2p(1,1)= cp;    g2p(1,2)=0.;
  g2p(2,0)=-sl*cp; g2p(2,1)=-sl*sp; g2p(2,2)=cl;

  Double_t alpha=t.GetAlpha();
  Double_t c=TMath::Cos(alpha), s=TMath::Sin(alpha);
  TMatrixD l2g(3,3); //local --> global
  l2g(0,0)= c; l2g(0,1)=-s; l2g(0,2)= 0;
  l2g(1,0)= s; l2g(1,1)= c; l2g(1,2)= 0;
  l2g(2,0)= 0; l2g(2,1)= 0; l2g(2,2)= 1;

  Double_t sy2=t.GetSigmaY2(), syz=t.GetSigmaZY(), sz2=t.GetSigmaZ2();
  TMatrixD cvl(3,3); //local covariance
  cvl(0,0)=0; cvl(0,1)=0;   cvl(0,2)=0;
  cvl(1,0)=0; cvl(1,1)=sy2; cvl(1,2)=syz;
  cvl(2,0)=0; cvl(2,1)=syz; cvl(2,2)=sz2;

  TMatrixD l2p(g2p, TMatrixD::kMult, l2g);
  TMatrixD cvp(3,3); //parallel covariance
  cvp=l2p*cvl*TMatrixD(TMatrixD::kTransposed,l2p);

  Double_t det=cvp(1,1)*cvp(2,2) - cvp(1,2)*cvp(2,1);
  if (TMath::Abs(det)<kAlmost0) return kFALSE;

  const Double_t m=100*100; //A large uncertainty in the momentum direction
  const Double_t eps=1/m;
  TMatrixD u(3,3);  //Inverse of the transverse part of the parallel covariance 
  u(0,0)=eps; u(0,1)=0;              u(0,2)=0;
  u(1,0)=0;   u(1,1)= cvp(2,2)/det;  u(1,2)=-cvp(2,1)/det;
  u(2,0)=0;   u(2,1)=-cvp(1,2)/det;  u(2,2)= cvp(1,1)/det;
  
  w=TMatrixD(TMatrixD::kTransposed,g2p)*u*g2p;

  return kTRUE;
}

Int_t AliESDv0::Refit() {
  //--------------------------------------------------------------------
  // Refit this vertex 
  //--------------------------------------------------------------------
  fStatus=0;

  //Save the daughters' momenta
  fParamN.GetPxPyPz(fNmom);
  fParamP.GetPxPyPz(fPmom);

  //Trivial estimation of the V0 vertex parameters 
  Double_t r1[3]; fParamN.GetXYZ(r1);
  Double_t r2[3]; fParamP.GetXYZ(r2);
  for (Int_t i=0; i<3; i++) fPos[i]=0.5*(r1[i]+r2[i]);
  fPosCov[1]=fPosCov[3]=fPosCov[4]=0.;
  fPosCov[0]=(fPos[0]-r1[0])*(fPos[0]-r1[0])/12.;
  fPosCov[2]=(fPos[1]-r1[1])*(fPos[1]-r1[1])/12.;
  fPosCov[5]=(fPos[2]-r1[2])*(fPos[2]-r1[2])/12.;
  fChi2V0=12.;


  //Try to improve the V0 vertex parameters
  TMatrixD w1(3,3);
  if (!GetWeight(w1,fParamN)) return fStatus;
  TMatrixD w2(3,3);
  if (!GetWeight(w2,fParamP)) return fStatus;
  TMatrixD cv(w1); cv+=w2;
  cv.Invert();
  if (!cv.IsValid()) return fStatus;

  //Covariance of the V0 vertex
  fPosCov[0]=cv(0,0);
  fPosCov[1]=cv(1,0); fPosCov[2]=cv(1,1);
  fPosCov[3]=cv(2,0); fPosCov[4]=cv(2,1); fPosCov[5]=cv(2,2);
  
  //Position of the V0 vertex
  TMatrixD cw1(cv,TMatrixD::kMult,w1);
  for (Int_t i=0; i<3; i++) {
      fPos[i]=r2[i];
      for (Int_t j=0; j<3; j++) fPos[i] += cw1(i,j)*(r1[j] - r2[j]);
  }

  //Chi2 of the V0 vertex
  fChi2V0=0.;
  Double_t res1[3]={r1[0]-fPos[0],r1[1]-fPos[1],r1[2]-fPos[2]};
  Double_t res2[3]={r2[0]-fPos[0],r2[1]-fPos[1],r2[2]-fPos[2]};
  for (Int_t i=0; i<3; i++)
      for (Int_t j=0; j<3; j++) 
          fChi2V0 += res1[i]*res1[j]*w1(i,j) + res2[i]*res2[j]*w2(i,j);

  //Successful fit
  fStatus=1;

  return fStatus;
}

AliESDv0& AliESDv0::operator=(const AliESDv0 &v0)
{
  //--------------------------------------------------------------------
  // The assignment operator
  //--------------------------------------------------------------------

  if(this==&v0)return *this;
  AliVParticle::operator=(v0);
  fParamN  = v0.fParamN;
  fParamP  = v0.fParamP;
  fEffMass = v0.fEffMass;
  fDcaV0Daughters = v0.fDcaV0Daughters;
  fChi2V0 = v0.fChi2V0;
  fRr = v0.fRr;
  fDistSigma    = v0.fDistSigma;
  fChi2Before   = v0.fChi2Before;
  fChi2After    = v0.fChi2After;
  fPointAngleFi = v0.fPointAngleFi;
  fPointAngleTh = v0.fPointAngleTh;
  fPointAngle   = v0.fPointAngle;
  fPdgCode      = v0.fPdgCode;
  fNidx         = v0.fNidx;
  fPidx         = v0.fPidx;
  fStatus       = v0.fStatus;
  fNBefore      = v0.fNBefore;
  fNAfter       = v0.fNAfter;
  fOnFlyStatus  = v0.fOnFlyStatus;

  for (int i=0; i<3; i++) {
    fPos[i]  = v0.fPos[i];
    fNmom[i] = v0.fNmom[i];
    fPmom[i] = v0.fPmom[i];
  }
  for (int i=0; i<6; i++) {
    fPosCov[i]  = v0.fPosCov[i];
  }
  for (Int_t i=0; i<2; i++) {
    fNormDCAPrim[i]=v0.fNormDCAPrim[i];
  }
  for (Int_t i=0;i<6;i++){
      fClusters[0][i]=v0.fClusters[0][i]; 
      fClusters[1][i]=v0.fClusters[1][i];
  }
  for (Int_t i=0;i<3;i++){
      fAngle[i]=v0.fAngle[i];
  }
  for (Int_t i=0;i<4;i++){fCausality[i]=v0.fCausality[i];}

  return *this;
}

void AliESDv0::Copy(TObject& obj) const {

  // this overwrites the virtual TOBject::Copy()
  // to allow run time copying without casting
  // in AliESDEvent

  if(this==&obj)return;
  AliESDv0 *robj = dynamic_cast<AliESDv0*>(&obj);
  if(!robj)return; // not an aliesesv0
  *robj = *this;
}

AliESDv0::~AliESDv0(){
  //--------------------------------------------------------------------
  // Empty destructor
  //--------------------------------------------------------------------
}

// Start with AliVParticle functions
Double_t AliESDv0::E() const {
  //--------------------------------------------------------------------
  // This gives the energy assuming the ChangeMassHypothesis was called
  //--------------------------------------------------------------------
  return E(fPdgCode);
}

Double_t AliESDv0::Y() const {
  //--------------------------------------------------------------------
  // This gives the energy assuming the ChangeMassHypothesis was called
  //--------------------------------------------------------------------
  return Y(fPdgCode);
}

// Then extend AliVParticle functions
Double_t AliESDv0::E(Int_t pdg) const {
  //--------------------------------------------------------------------
  // This gives the energy with the particle hypothesis as argument 
  //--------------------------------------------------------------------
  Double_t mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();
  return TMath::Sqrt(mass*mass+P()*P());
}

Double_t AliESDv0::Y(Int_t pdg) const {
  //--------------------------------------------------------------------
  // This gives the rapidity with the particle hypothesis as argument 
  //--------------------------------------------------------------------
  return 0.5*TMath::Log((E(pdg)+Pz())/(E(pdg)-Pz()+1.e-13));
}

// Now the functions for analysis consistency
Double_t AliESDv0::RapK0Short() const {
  //--------------------------------------------------------------------
  // This gives the pseudorapidity assuming a K0s particle
  //--------------------------------------------------------------------
  return Y(kK0Short);
}

Double_t AliESDv0::RapLambda() const {
  //--------------------------------------------------------------------
  // This gives the pseudorapidity assuming a (Anti) Lambda particle
  //--------------------------------------------------------------------
  return Y(kLambda0);
}

Double_t AliESDv0::AlphaV0() const {
  //--------------------------------------------------------------------
  // This gives the Armenteros-Podolanski alpha
  //--------------------------------------------------------------------
  TVector3 momNeg(fNmom[0],fNmom[1],fNmom[2]);
  TVector3 momPos(fPmom[0],fPmom[1],fPmom[2]);
  TVector3 momTot(Px(),Py(),Pz());

  Double_t lQlNeg = momNeg.Dot(momTot)/momTot.Mag();
  Double_t lQlPos = momPos.Dot(momTot)/momTot.Mag();

  //return 1.-2./(1.+lQlNeg/lQlPos);
  return (lQlPos - lQlNeg)/(lQlPos + lQlNeg);
}

Double_t AliESDv0::PtArmV0() const {
  //--------------------------------------------------------------------
  // This gives the Armenteros-Podolanski ptarm
  //--------------------------------------------------------------------
  TVector3 momNeg(fNmom[0],fNmom[1],fNmom[2]);
  TVector3 momTot(Px(),Py(),Pz());

  return momNeg.Perp(momTot);
}

// Eventually the older functions
Double_t AliESDv0::ChangeMassHypothesis(Int_t code) {
  //--------------------------------------------------------------------
  // This function changes the mass hypothesis for this V0
  // and returns the "kinematical quality" of this hypothesis 
  //--------------------------------------------------------------------
  static
  Double_t piMass=TDatabasePDG::Instance()->GetParticle(kPiPlus)->Mass();
  static
  Double_t prMass=TDatabasePDG::Instance()->GetParticle(kProton)->Mass();
  static
  Double_t k0Mass=TDatabasePDG::Instance()->GetParticle(kK0Short)->Mass();
  static
  Double_t l0Mass=TDatabasePDG::Instance()->GetParticle(kLambda0)->Mass();

  Double_t nmass=piMass, pmass=piMass, mass=k0Mass, ps=0.206;

  fPdgCode=code;

  switch (code) {
  case kLambda0:
    nmass=piMass; pmass=prMass; mass=l0Mass; ps=0.101; break;
  case kLambda0Bar:
    pmass=piMass; nmass=prMass; mass=l0Mass; ps=0.101; break;
  case kK0Short: 
    break;
  default:
    AliError("invalide PDG code ! Assuming K0s...");
    fPdgCode=kK0Short;
    break;
  }

  Double_t pxn=fNmom[0], pyn=fNmom[1], pzn=fNmom[2]; 
  Double_t pxp=fPmom[0], pyp=fPmom[1], pzp=fPmom[2];

  Double_t en=TMath::Sqrt(nmass*nmass + pxn*pxn + pyn*pyn + pzn*pzn);
  Double_t ep=TMath::Sqrt(pmass*pmass + pxp*pxp + pyp*pyp + pzp*pzp);
  Double_t pxl=pxn+pxp, pyl=pyn+pyp, pzl=pzn+pzp;
  Double_t pl=TMath::Sqrt(pxl*pxl + pyl*pyl + pzl*pzl);

  fEffMass=TMath::Sqrt((en+ep)*(en+ep)-pl*pl);

  Double_t beta=pl/(en+ep);
  Double_t pln=(pxn*pxl + pyn*pyl + pzn*pzl)/pl;
  Double_t plp=(pxp*pxl + pyp*pyl + pzp*pzl)/pl;

  Double_t pt2=pxp*pxp + pyp*pyp + pzp*pzp - plp*plp;

  Double_t a=(plp-pln)/(plp+pln);
  a -= (pmass*pmass-nmass*nmass)/(mass*mass);
  a = 0.25*beta*beta*mass*mass*a*a + pt2;

  return (a - ps*ps);
  
}

void AliESDv0::GetPxPyPz(Double_t &px, Double_t &py, Double_t &pz) const {
  //--------------------------------------------------------------------
  // This function returns V0's momentum (global)
  //--------------------------------------------------------------------
  px=fNmom[0]+fPmom[0]; 
  py=fNmom[1]+fPmom[1]; 
  pz=fNmom[2]+fPmom[2]; 
}

void AliESDv0::GetXYZ(Double_t &x, Double_t &y, Double_t &z) const {
  //--------------------------------------------------------------------
  // This function returns V0's position (global)
  //--------------------------------------------------------------------
  x=fPos[0]; 
  y=fPos[1]; 
  z=fPos[2]; 
}

Float_t AliESDv0::GetD(Double_t x0, Double_t y0) const {
  //--------------------------------------------------------------------
  // This function returns V0's impact parameter calculated in 2D in XY plane
  //--------------------------------------------------------------------
  Double_t x=fPos[0],y=fPos[1];
  Double_t px=fNmom[0]+fPmom[0];
  Double_t py=fNmom[1]+fPmom[1];

  Double_t dz=(x0-x)*py - (y0-y)*px;
  Double_t d=TMath::Sqrt(dz*dz/(px*px+py*py));
  return d;
}

Float_t AliESDv0::GetD(Double_t x0, Double_t y0, Double_t z0) const {
  //--------------------------------------------------------------------
  // This function returns V0's impact parameter calculated in 3D
  //--------------------------------------------------------------------
  Double_t x=fPos[0],y=fPos[1],z=fPos[2];
  Double_t px=fNmom[0]+fPmom[0];
  Double_t py=fNmom[1]+fPmom[1];
  Double_t pz=fNmom[2]+fPmom[2];

  Double_t dx=(y0-y)*pz - (z0-z)*py; 
  Double_t dy=(x0-x)*pz - (z0-z)*px;
  Double_t dz=(x0-x)*py - (y0-y)*px;
  Double_t d=TMath::Sqrt((dx*dx+dy*dy+dz*dz)/(px*px+py*py+pz*pz));
  return d;
}

Float_t AliESDv0::GetV0CosineOfPointingAngle(Double_t refPointX, Double_t refPointY, Double_t refPointZ) const {
  // calculates the pointing angle of the V0 wrt a reference point

  Double_t momV0[3]; //momentum of the V0
  GetPxPyPz(momV0[0],momV0[1],momV0[2]);

  Double_t deltaPos[3]; //vector between the reference point and the V0 vertex
  deltaPos[0] = fPos[0] - refPointX;
  deltaPos[1] = fPos[1] - refPointY;
  deltaPos[2] = fPos[2] - refPointZ;

  Double_t momV02    = momV0[0]*momV0[0] + momV0[1]*momV0[1] + momV0[2]*momV0[2];
  Double_t deltaPos2 = deltaPos[0]*deltaPos[0] + deltaPos[1]*deltaPos[1] + deltaPos[2]*deltaPos[2];

  Double_t cosinePointingAngle = (deltaPos[0]*momV0[0] +
				  deltaPos[1]*momV0[1] +
				  deltaPos[2]*momV0[2] ) /
    TMath::Sqrt(momV02 * deltaPos2);
  
  return cosinePointingAngle;
}


// **** The following functions need to be revised

void AliESDv0::GetPosCov(Double_t cov[6]) const {

  for (Int_t i=0; i<6; ++i) cov[i] = fPosCov[i];

}

Double_t AliESDv0::GetSigmaY(){
  //
  // return sigmay in y  at vertex position  using covariance matrix 
  //
  const Double_t * cp  = fParamP.GetCovariance();
  const Double_t * cm  = fParamN.GetCovariance();
  Double_t sigmay = cp[0]+cm[0]+ cp[5]*(fParamP.GetX()-fRr)*(fParamP.GetX()-fRr)+ cm[5]*(fParamN.GetX()-fRr)*(fParamN.GetX()-fRr);
  return (sigmay>0) ? TMath::Sqrt(sigmay):100;
}

Double_t AliESDv0::GetSigmaZ(){
  //
  // return sigmay in y  at vertex position  using covariance matrix 
  //
  const Double_t * cp  = fParamP.GetCovariance();
  const Double_t * cm  = fParamN.GetCovariance();
  Double_t sigmaz = cp[2]+cm[2]+ cp[9]*(fParamP.GetX()-fRr)*(fParamP.GetX()-fRr)+ cm[9]*(fParamN.GetX()-fRr)*(fParamN.GetX()-fRr);
  return (sigmaz>0) ? TMath::Sqrt(sigmaz):100;
}

Double_t AliESDv0::GetSigmaD0(){
  //
  // Sigma parameterization using covariance matrix
  //
  // sigma of distance between two tracks in vertex position 
  // sigma of DCA is proportianal to sigmaD0
  // factor 2 difference is explained by the fact that the DCA is calculated at the position 
  // where the tracks as closest together ( not exact position of the vertex)
  //
  const Double_t * cp      = fParamP.GetCovariance();
  const Double_t * cm      = fParamN.GetCovariance();
  Double_t sigmaD0   = cp[0]+cm[0]+cp[2]+cm[2]+fgkParams.fPSigmaOffsetD0*fgkParams.fPSigmaOffsetD0;
  sigmaD0           += ((fParamP.GetX()-fRr)*(fParamP.GetX()-fRr))*(cp[5]+cp[9]);
  sigmaD0           += ((fParamN.GetX()-fRr)*(fParamN.GetX()-fRr))*(cm[5]+cm[9]);
  return (sigmaD0>0)? TMath::Sqrt(sigmaD0):100;
}


Double_t AliESDv0::GetSigmaAP0(){
  //
  //Sigma parameterization using covariance matrices
  //
  Double_t prec  = TMath::Sqrt((fNmom[0]+fPmom[0])*(fNmom[0]+fPmom[0])
			      +(fNmom[1]+fPmom[1])*(fNmom[1]+fPmom[1])
			      +(fNmom[2]+fPmom[2])*(fNmom[2]+fPmom[2]));
  Double_t normp = TMath::Sqrt(fPmom[0]*fPmom[0]+fPmom[1]*fPmom[1]+fPmom[2]*fPmom[2])/prec;  // fraction of the momenta
  Double_t normm = TMath::Sqrt(fNmom[0]*fNmom[0]+fNmom[1]*fNmom[1]+fNmom[2]*fNmom[2])/prec;  
  const Double_t * cp      = fParamP.GetCovariance();
  const Double_t * cm      = fParamN.GetCovariance();
  Double_t sigmaAP0 = fgkParams.fPSigmaOffsetAP0*fgkParams.fPSigmaOffsetAP0;                           // minimal part
  sigmaAP0 +=  (cp[5]+cp[9])*(normp*normp)+(cm[5]+cm[9])*(normm*normm);          // angular resolution part
  Double_t sigmaAP1 = GetSigmaD0()/(TMath::Abs(fRr)+0.01);                       // vertex position part
  sigmaAP0 +=  0.5*sigmaAP1*sigmaAP1;                              
  return (sigmaAP0>0)? TMath::Sqrt(sigmaAP0):100;
}

Double_t AliESDv0::GetEffectiveSigmaD0(){
  //
  // minimax - effective Sigma parameterization 
  // p12 effective curvature and v0 radius postion used as parameters  
  //  
  Double_t p12 = TMath::Sqrt(fParamP.GetParameter()[4]*fParamP.GetParameter()[4]+
			     fParamN.GetParameter()[4]*fParamN.GetParameter()[4]);
  Double_t sigmaED0= TMath::Max(TMath::Sqrt(fRr)-fgkParams.fPSigmaRminDE,0.0)*fgkParams.fPSigmaCoefDE*p12*p12;
  sigmaED0*= sigmaED0;
  sigmaED0*= sigmaED0;
  sigmaED0 = TMath::Sqrt(sigmaED0+fgkParams.fPSigmaOffsetDE*fgkParams.fPSigmaOffsetDE);
  return (sigmaED0<fgkParams.fPSigmaMaxDE) ? sigmaED0: fgkParams.fPSigmaMaxDE;
}


Double_t AliESDv0::GetEffectiveSigmaAP0(){
  //
  // effective Sigma parameterization of point angle resolution 
  //
  Double_t p12 = TMath::Sqrt(fParamP.GetParameter()[4]*fParamP.GetParameter()[4]+
			     fParamN.GetParameter()[4]*fParamN.GetParameter()[4]);
  Double_t sigmaAPE= fgkParams.fPSigmaBase0APE;
  sigmaAPE+= fgkParams.fPSigmaR0APE/(fgkParams.fPSigmaR1APE+fRr);
  sigmaAPE*= (fgkParams.fPSigmaP0APE+fgkParams.fPSigmaP1APE*p12);
  sigmaAPE = TMath::Min(sigmaAPE,fgkParams.fPSigmaMaxAPE);
  return sigmaAPE;
}


Double_t  AliESDv0::GetMinimaxSigmaAP0(){
  //
  // calculate mini-max effective sigma of point angle resolution
  //
  //compv0->fTree->SetAlias("SigmaAP2","max(min((SigmaAP0+SigmaAPE0)*0.5,1.5*SigmaAPE0),0.5*SigmaAPE0+0.003)");
  Double_t    effectiveSigma = GetEffectiveSigmaAP0();
  Double_t    sigmaMMAP = 0.5*(GetSigmaAP0()+effectiveSigma);
  sigmaMMAP  = TMath::Min(sigmaMMAP, fgkParams.fPMaxFractionAP0*effectiveSigma);
  sigmaMMAP  = TMath::Max(sigmaMMAP, fgkParams.fPMinFractionAP0*effectiveSigma+fgkParams.fPMinAP0);
  return sigmaMMAP;
}
Double_t  AliESDv0::GetMinimaxSigmaD0(){
  //
  // calculate mini-max sigma of dca resolution
  // 
  //compv0->fTree->SetAlias("SigmaD2","max(min((SigmaD0+SigmaDE0)*0.5,1.5*SigmaDE0),0.5*SigmaDE0)");
  Double_t    effectiveSigma = GetEffectiveSigmaD0();
  Double_t    sigmaMMD0 = 0.5*(GetSigmaD0()+effectiveSigma);
  sigmaMMD0  = TMath::Min(sigmaMMD0, fgkParams.fPMaxFractionD0*effectiveSigma);
  sigmaMMD0  = TMath::Max(sigmaMMD0, fgkParams.fPMinFractionD0*effectiveSigma+fgkParams.fPMinD0);
  return sigmaMMD0;
}


Double_t AliESDv0::GetLikelihoodAP(Int_t mode0, Int_t mode1){
  //
  // get likelihood for point angle
  //
  Double_t sigmaAP = 0.007;            //default sigma
  switch (mode0){
  case 0:
    sigmaAP = GetSigmaAP0();           // mode 0  - covariance matrix estimates used 
    break;
  case 1:
    sigmaAP = GetEffectiveSigmaAP0();  // mode 1 - effective sigma used
    break;
  case 2:
    sigmaAP = GetMinimaxSigmaAP0();    // mode 2 - minimax sigma
    break;
  }
  Double_t apNorm = TMath::Min(TMath::ACos(fPointAngle)/sigmaAP,50.);  
  //normalized point angle, restricted - because of overflow problems in Exp
  Double_t likelihood = 0;
  switch(mode1){
  case 0:
    likelihood = TMath::Exp(-0.5*apNorm*apNorm);   
    // one component
    break;
  case 1:
    likelihood = (TMath::Exp(-0.5*apNorm*apNorm)+0.5* TMath::Exp(-0.25*apNorm*apNorm))/1.5;
    // two components
    break;
  case 2:
    likelihood = (TMath::Exp(-0.5*apNorm*apNorm)+0.5* TMath::Exp(-0.25*apNorm*apNorm)+0.25*TMath::Exp(-0.125*apNorm*apNorm))/1.75;
    // three components
    break;
  }
  return likelihood;
}

Double_t AliESDv0::GetLikelihoodD(Int_t mode0, Int_t mode1){
  //
  // get likelihood for DCA
  //
  Double_t sigmaD = 0.03;            //default sigma
  switch (mode0){
  case 0:
    sigmaD = GetSigmaD0();           // mode 0  - covariance matrix estimates used 
    break;
  case 1:
    sigmaD = GetEffectiveSigmaD0();  // mode 1 - effective sigma used
    break;
  case 2:
    sigmaD = GetMinimaxSigmaD0();    // mode 2 - minimax sigma
    break;
  }

  //Bo:  Double_t dNorm = TMath::Min(fDist2/sigmaD,50.);
  Double_t dNorm = TMath::Min(fDcaV0Daughters/sigmaD,50.);//Bo:
  //normalized point angle, restricted - because of overflow problems in Exp
  Double_t likelihood = 0;
  switch(mode1){
  case 0:
    likelihood = TMath::Exp(-2.*dNorm);   
    // one component
    break;
  case 1:
    likelihood = (TMath::Exp(-2.*dNorm)+0.5* TMath::Exp(-dNorm))/1.5;
    // two components
    break;
  case 2:
    likelihood = (TMath::Exp(-2.*dNorm)+0.5* TMath::Exp(-dNorm)+0.25*TMath::Exp(-0.5*dNorm))/1.75;
    // three components
    break;
  }
  return likelihood;

}

Double_t AliESDv0::GetLikelihoodC(Int_t mode0, Int_t /*mode1*/) const {
  //
  // get likelihood for Causality
  // !!!  Causality variables defined in AliITStrackerMI !!! 
  //      when more information was available
  //  
  Double_t likelihood = 0.5;
  Double_t minCausal  = TMath::Min(fCausality[0],fCausality[1]);
  Double_t maxCausal  = TMath::Max(fCausality[0],fCausality[1]);
  //  minCausal           = TMath::Max(minCausal,0.5*maxCausal);
  //compv0->fTree->SetAlias("LCausal","(1.05-(2*(0.8-exp(-max(RC.fV0rec.fCausality[0],RC.fV0rec.fCausality[1])))+2*(0.8-exp(-min(RC.fV0rec.fCausality[0],RC.fV0rec.fCausality[1]))))/2)**4");
  
  switch(mode0){
  case 0:
    //normalization 
    likelihood = TMath::Power((1.05-2*(0.8-TMath::Exp(-maxCausal))),4.);
    break;
  case 1:
    likelihood = TMath::Power(1.05-(2*(0.8-TMath::Exp(-maxCausal))+(2*(0.8-TMath::Exp(-minCausal))))*0.5,4.);
    break;
  }
  return likelihood;
  
}

void AliESDv0::SetCausality(Float_t pb0, Float_t pb1, Float_t pa0, Float_t pa1)
{
  //
  // set probabilities
  //
  fCausality[0] = pb0;     // probability - track 0 exist before vertex
  fCausality[1] = pb1;     // probability - track 1 exist before vertex
  fCausality[2] = pa0;     // probability - track 0 exist close after vertex
  fCausality[3] = pa1;     // probability - track 1 exist close after vertex
}
void  AliESDv0::SetClusters(const Int_t *clp, const Int_t *clm)
{
  //
  // Set its clusters indexes
  //
  for (Int_t i=0;i<6;i++) fClusters[0][i] = clp[i]; 
  for (Int_t i=0;i<6;i++) fClusters[1][i] = clm[i]; 
}

Double_t AliESDv0::GetEffMass(UInt_t p1, UInt_t p2) const{
  //
  // calculate effective mass
  //
  const Double_t kpmass[5] = {TDatabasePDG::Instance()->GetParticle(kElectron)->Mass(),
			     TDatabasePDG::Instance()->GetParticle(kMuonMinus)->Mass(),
			     TDatabasePDG::Instance()->GetParticle(kPiPlus)->Mass(),
			     TDatabasePDG::Instance()->GetParticle(kKPlus)->Mass(),
			     TDatabasePDG::Instance()->GetParticle(kProton)->Mass()};
  /*
  if (p1>4) return -1;
  if (p2>4) return -1;
  Double_t mass1 = kpmass[p1]; 
  Double_t mass2 = kpmass[p2];   
  const Double_t *m1 = fPmom;
  const Double_t *m2 = fNmom;
  //
  //if (fRP[p1]+fRM[p2]<fRP[p2]+fRM[p1]){
  //  m1 = fPM;
  //  m2 = fPP;
  //}
  //
  Double_t e1    = TMath::Sqrt(mass1*mass1+
                              m1[0]*m1[0]+
                              m1[1]*m1[1]+
                              m1[2]*m1[2]);
  Double_t e2    = TMath::Sqrt(mass2*mass2+
                              m2[0]*m2[0]+
                              m2[1]*m2[1]+
                              m2[2]*m2[2]);  
  Double_t mass =  
    (m2[0]+m1[0])*(m2[0]+m1[0])+
    (m2[1]+m1[1])*(m2[1]+m1[1])+
    (m2[2]+m1[2])*(m2[2]+m1[2]);
  
  mass = (e1+e2)*(e1+e2)-mass;
  if (mass < 0.) mass = 0.;
  return (TMath::Sqrt(mass));
  */
  if(p1>4 || p2>4) return -1;
  return GetEffMassExplicit(kpmass[p1],kpmass[p2]);			       
}

Double_t AliESDv0::GetEffMassExplicit(Double_t m1, Double_t m2) const {
  //
  // calculate effective mass with given masses of decay products
  //
  const AliExternalTrackParam *paramP = GetParamP();
  const AliExternalTrackParam *paramN = GetParamN();
  if (paramP->GetParameter()[4]<0){
    paramP=GetParamN();
    paramN=GetParamP();
  }
  Double_t pmom[3]={0}, nmom[3]={0};
  paramP->GetPxPyPz(pmom);
  paramN->GetPxPyPz(nmom);
  Double_t e12   = m1*m1+pmom[0]*pmom[0]+pmom[1]*pmom[1]+pmom[2]*pmom[2];
  Double_t e22   = m2*m2+nmom[0]*nmom[0]+nmom[1]*nmom[1]+nmom[2]*nmom[2];
  Double_t cmass = TMath::Sqrt(TMath::Max(m1*m1+m2*m2
					  +2.*(TMath::Sqrt(e12*e22)-pmom[0]*nmom[0]-pmom[1]*nmom[1]-pmom[2]*nmom[2]),0.));
  return cmass;
			       
}



Double_t AliESDv0::GetKFInfo(UInt_t p1, UInt_t p2, Int_t type) const{
  //
  // type:
  //   0 - return mass
  //   1 - return err mass
  //   2 - return chi2
  // 
  const Int_t spdg[5]={kPositron,kMuonPlus,kPiPlus, kKPlus, kProton};
  const AliExternalTrackParam *paramP = GetParamP();
  const AliExternalTrackParam *paramN = GetParamN();
  if (paramP->GetParameter()[4]<0){
    paramP=GetParamN();
    paramN=GetParamP();
  }
  AliKFParticle kfp1(  *(paramP), spdg[p1] *TMath::Sign(1,p1) );
  AliKFParticle kfp2( *(paramN), spdg[p2] *TMath::Sign(1,p2) );
  AliKFParticle v0KF;
  v0KF+=kfp1;
  v0KF+=kfp2;
  if (type==0) return v0KF.GetMass();
  if (type==1) return v0KF.GetErrMass();
  if (type==2) return v0KF.GetChi2();
  if (type==3) return v0KF.GetPt(); 

  return 0;
}

/// Calculate  numerical derivative of V0 properties
/// \param p1     -particle type (0-el,1-mu, 2-pi, 3-K, 4-Proton)
/// \param p2     - particle type (0-el,1-mu, 2-pi, 3-K, 4-Proton)
/// \param type   - return value type (0=mass, 1-err mass, 2-chi2, 3 - pt of V0
/// \param d1pt   - detla q/pt  (1/GeV)
/// \param s1pt   - momentum scale  (fraction)
/// \param eLoss  - delta of energy loss  (MIP==1) ///
/// \[aram flag   - apply derivative for track 1 (flag -1) track 2(flag 2) or both (flag=3)
/// \return       - value of given type
Double_t AliESDv0::GetKFInfoScale(UInt_t p1, UInt_t p2, Int_t type, Double_t d1pt, Double_t s1pt, Double_t eLoss, Int_t flag) const{
  // Used for fast numerical studies  of impact of residual miscalibration
  // Input:
  // p1,p2 - particle type (0-el,1-mu, 2-pi, 3-K, 4-Proton)
  // type
  //   0 - return mass
  //   1 - return err mass
  //   2 - return chi2
  //   3 - return Pt of the V0 ()
  //  distrotion 
  //   d1pt - 1/pt shift
  //   s1pt - scaling of 1/pt
  // Important function to benchmark the pt resolution, and to find out systematic distortion
  // Used for fast numerical emulation of impact of the resifual misscalibration
  //    
  /*
    Example analysis using filtered trees:
    // ratio of pt:  0.0015 1/GeV shift -> 0 mean shift for the K0 but ~ 1.1 % shift at 10 GeV  for the Lambda  
    treeLambda->Draw("v0.GetKFInfoScale(4,2,3,0.0015,0.0)/v0.GetKFInfoScale(4,2,3,0.0,0)-1:v0.Pt()>>ptratioLambda(50,0,20)","v0.Pt()<20","prof",100000);
    treeALambda->Draw("v0.GetKFInfoScale(2,4,3,0.0015,0.0)/v0.GetKFInfoScale(2,4,3,0.0,0)-1:v0.Pt()>>ptratioALambda(50,0,20)","v0.Pt()<20","profsame",100000);
    treeK0->Draw("v0.GetKFInfoScale(2,2,3,0.0015,0.0)/v0.GetKFInfoScale(2,2,3,0.0,0)-1:v0.Pt()>>ptratioK0(50,0,20)","v0.Pt()<20","profsame",100000);
    // pt spectra ratios:
    // ratio of spectra  - e.g q/pt shift ~ 0.0015 1/GeV -> Lambda - 10 % difference in pt yeald at 10 GeV  - K0 - unaffected
    treeLambda->Draw("v0.GetKFInfoScale(4,2,3,0.0015,0.0)>>hisLambdaShifted0015(50,0,20)","","",1000000);
    treeLambda->Draw("v0.GetKFInfoScale(4,2,3,0.00,0.0)>>hisLambdaShifted0(50,0,20)","","",1000000);
    TH1F ratioLambdaShifted0015 = *hisLambdaShifted0015;
    ratioLambdaShifted0015.Sumw2();
    ratioLambdaShifted0015.Divide(hisLambdaShifted0);
   */
  const Int_t spdg[5]={kPositron,kMuonPlus,kPiPlus, kKPlus, kProton};
  if (p1>4 || p2>4)return 0;
  AliExternalTrackParam paramP;
  AliExternalTrackParam paramN;
  if (GetParamP()->GetParameter()[4]<0){
    paramP=*(GetParamN());
    paramN=*(GetParamP());
  }else{
    paramP=*(GetParamP());
    paramN=*(GetParamN());
  }
  // calculate delta of energy loss
  Double_t bg1=paramP.P()  /TDatabasePDG::Instance()->GetParticle(spdg[p1])->Mass();
  Double_t bg2=paramN.P()  /TDatabasePDG::Instance()->GetParticle(spdg[p2])->Mass();
  Double_t dP1=AliExternalTrackParam::BetheBlochGeant(bg1)/AliExternalTrackParam::BetheBlochGeant(3.); ///relative energu loss
  Double_t dP2=AliExternalTrackParam::BetheBlochGeant(bg2)/AliExternalTrackParam::BetheBlochGeant(3.);

  Double_t *pparam1 = (Double_t*)paramP.GetParameter();
  Double_t *pparam2 = (Double_t*)paramN.GetParameter();
  if (flag&0x1) pparam1[4]+=d1pt;
  if (flag&0x2) pparam2[4]+=d1pt;
  if (flag&0x1) pparam1[4]*=(1+s1pt)*(1.+eLoss*dP1/paramP.P());   /// TODO   - use relative energy loss
  if (flag&0x2) pparam2[4]*=(1+s1pt)*(1.+eLoss*dP2/paramN.P());   ///

  //
  AliKFParticle kfp1( paramP, spdg[p1] *TMath::Sign(1,p1) );
  AliKFParticle kfp2( paramN, spdg[p2] *TMath::Sign(1,p2) );
  AliKFParticle *v0KF = new AliKFParticle;
  *(v0KF)+=kfp1;
  *(v0KF)+=kfp2;
  if (type==0) return v0KF->GetMass();
  if (type==1) return v0KF->GetErrMass();
  if (type==2) return v0KF->GetChi2();
  if (type==3) return v0KF->GetPt(); 
  return 0;
}


AliESDVertex AliESDv0::GetVertex() const{
  // Build an AliESDVertex from this AliESDv0
  AliESDVertex vtx(fPos,fPosCov,fChi2V0,2);
  return vtx;
}

