//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        *
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Sergey Gorbunov <sergey.gorbunov@cern.ch>             *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/** @file   AliHLTTPCSpline2D3D.cxx
    @author Sergey Gorbubnov
    @date   
    @brief 
*/

#include "AliHLTTPCSpline2D3D.h"
#include "AliHLTTPCSpline2D3DObject.h"
#include "Vc/Vc"
  
#include <iostream>
#include <iomanip>

using namespace std;

void AliHLTTPCSpline2D3D::Init(Float_t minA,Float_t  maxA, Int_t  nBinsA, Float_t  minB,Float_t  maxB, Int_t  nBinsB)
{
  //
  // Initialisation
  //

  if( maxA<= minA ) maxA = minA+1;
  if( maxB<= minB ) maxB = minB+1;
  if( nBinsA <4 ) nBinsA = 4;
  if( nBinsB <4 ) nBinsB = 4;

  fNA = nBinsA;
  fNB = nBinsB;
  fN = fNA*fNB;
  fMinA = minA;
  fMinB = minB;

  fStepA = (maxA-minA)/(nBinsA-1);
  fStepB = (maxB-minB)/(nBinsB-1);
  fScaleA = 1./fStepA;
  fScaleB = 1./fStepB;

  Vc::free( fXYZ );
  fXYZ = Vc::malloc< float, Vc::AlignOnCacheline>( 4*fN );
  memset ( fXYZ, 0, fN*4*sizeof(Float_t) );
}

AliHLTTPCSpline2D3D::~AliHLTTPCSpline2D3D()
{
  Vc::free( fXYZ );
}

Int_t AliHLTTPCSpline2D3D::WriteToBuffer( char *buf, size_t &size ) const 
{
  size_t maxSize = size;
  size = 0;
  if( EstimateBufferSize() > maxSize ) return -1;
  Int_t *pInt = reinterpret_cast<Int_t*> (buf);
  pInt[0] = Version();
  pInt[1] = fNA;
  pInt[2] = fNB;
  Float_t *pFloat = reinterpret_cast<Float_t*> (pInt+3);
  pFloat[0] = fMinA;
  pFloat[1] = fMinB;
  pFloat[2] = fStepA;
  pFloat[3] = fStepB;
  pFloat+=4;
  for( int i=0; i<fN; i+=4, pFloat+=3 ){ 
    pFloat[0] = fXYZ[i+0];
    pFloat[1] = fXYZ[i+1];
    pFloat[2] = fXYZ[i+2];
  }
  size = EstimateBufferSize();
  return 0;
}
  
Int_t AliHLTTPCSpline2D3D::ReadFromBuffer( const char*buf, size_t &size ) 
{
  size_t maxSize = size;
  size = 0;
  if( maxSize < sizeof(Int_t)*3 + sizeof(Float_t)*(4) ) return -1;
  const Int_t *pInt = reinterpret_cast< const Int_t*> (buf);
  const Float_t *pFloat = reinterpret_cast< const Float_t*> (pInt+3);  
  if( pInt[0]!= Version() ) return -1;
  Int_t nA = pInt[1];
  Int_t nB = pInt[2];
  Int_t n = nA*nB;
  if( maxSize < sizeof(Int_t)*3 + sizeof(Float_t)*(4+3*n) ) return -2;

  fNA = nA;
  fNB = nB;
  fN = n;
  fMinA = pFloat[0];
  fMinB = pFloat[1];
  fStepA = pFloat[2];
  fStepB = pFloat[3];
  int err = 0;
  if( fabs( fStepA ) <1.e-8 ) { fStepA = 1.; err=-3; }
  if( fabs( fStepB ) <1.e-8 ) { fStepB = 1.; err=-3; }

  fScaleA = 1./fStepA;
  fScaleB = 1./fStepB;

  pFloat+=4;
  Vc::free( fXYZ );
  fXYZ = Vc::malloc< float, Vc::AlignOnCacheline>( 4*fN );
  for( int i=0; i<4*fN; i+=4, pFloat+=3 ){ 
    fXYZ[i+0] = pFloat[0];
    fXYZ[i+1] = pFloat[1];
    fXYZ[i+2] = pFloat[2];
    fXYZ[i+3] = 0;    
  }
  size = sizeof(Int_t)*3 + sizeof(Float_t)*(4+3*n);
  return err;
}

void AliHLTTPCSpline2D3D::WriteToObject( AliHLTTPCSpline2D3DObject &obj )
{
  //
  // write spline to ROOT object to store it in database
  //  
  obj.SetMinA( fMinA );
  obj.SetMinB( fMinB );
  obj.SetStepA( fStepA );
  obj.SetStepB( fStepB );
  obj.InitGrid( fNA, fNB );
  for( int i=0; i<fN; i++ ) obj.SetGridValue( i, fXYZ + i*4);
}
 
void  AliHLTTPCSpline2D3D::ReadFromObject( const AliHLTTPCSpline2D3DObject &obj )
{
  //
  // read spline from ROOT object stored in database
  //

  fMinA = obj.GetMinA();
  fStepA = obj.GetStepA();
  fNA = obj.GetNPointsA();

  fMinB = obj.GetMinB();
  fStepB = obj.GetStepB();
  fNB = obj.GetNPointsB();

  fN = fNA*fNB;

  fScaleA = fabs(fStepA) >1.e-8 ?1./fStepA :1.;
  fScaleB = fabs(fStepB) >1.e-8 ?1./fStepB :1.;

  Vc::free( fXYZ );
  fXYZ = Vc::malloc< float, Vc::AlignOnCacheline>( 4*fN );
  for( int i=0; i<fN; i++ ){
    obj.GetGridValue( i, fXYZ + i*4 );
    fXYZ[i*4+3] = 0.;
  }
}

void AliHLTTPCSpline2D3D::Consolidate()
{
  //
  // Consolidate the map
  //
  
  Float_t *m = fXYZ;
  for( int iXYZ=0; iXYZ<3; iXYZ++ ){
    for( int iA=0; iA<fNA; iA++){
      {
	int i0 = 4*iA*fNB + iXYZ;
	int i1 = i0+4;
	int i2 = i0+4*2;
	int i3 = i0+4*3;
	m[i0] = 0.5*( m[i3]+m[i0]+3*(m[i1]-m[i2]) );      
      }
      {
	int i0 = 4*(iA*fNB+fNB-4) + iXYZ;
	int i1 = i0+4;
	int i2 = i0+4*2;
	int i3 = i0+4*3;
	m[i3] = 0.5*( m[i0]+m[i3]+3*(m[i2]-m[i1]) );
      }
    }
    for( int iB=0; iB<fNB; iB++){
      {
	int i0 = 4*(0*fNB +iB) +iXYZ;
	int i1 = i0+4*fNB;
	int i2 = i1+4*fNB;
	int i3 = i2+4*fNB;
	m[i0] = 0.5*( m[i3]+m[i0]+3*(m[i1]-m[i2]) );
      }
      {
	int i0 = 4*((fNA-4)*fNB +iB) +iXYZ;
	int i1 = i0+4*fNB;
	int i2 = i1+4*fNB;
	int i3 = i2+4*fNB;
	m[i3] = 0.5*( m[i0]+m[i3]+3*(m[i2]-m[i1]) );
      }
    }
  }
 }



inline Vc::float_v GetSpline3Vc(Vc::float_v v0, Vc::float_v v1, Vc::float_v v2, Vc::float_v v3, 
				Vc::float_v x, Vc::float_v x2)
{
  Vc::float_v dv = v2-v1;  
  Vc::float_v z0 = 0.5f*(v2-v0);
  Vc::float_v z1 = 0.5f*(v3-v1);
  return x2*( (z1-dv + z0-dv)*(x-1) - (z0-dv) ) + z0*x + v1; 
}

void AliHLTTPCSpline2D3D::GetValue(Float_t A, Float_t B, Float_t XYZ[] ) const
{
  //
  //  Get Interpolated value at A,B 
  //

  Float_t lA = (A-fMinA)*fScaleA-1.f;
  Int_t iA = (int) lA;
  if( lA<0 ) iA=0;
  else if( iA>fNA-4 ) iA = fNA-4;

  Float_t lB = (B-fMinB)*fScaleB -1.f;
  Int_t iB = (int) lB;
  if( lB<0 ) iB=0;
  else if( iB>fNB-4 ) iB = fNB-4;  

  if( Vc::float_v::Size==4 ){
    Vc::float_v da = lA-iA;
    Vc::float_v db = lB-iB;
    Vc::float_v db2=db*db;
    
    Vc::float_v v[4];
    Int_t ind = iA*fNB  + iB;
    const Vc::float_v *m = reinterpret_cast< const Vc::float_v *>(fXYZ);

    for( Int_t i=0; i<4; i++ ){    
      v[i] = GetSpline3Vc(m[ind+0],m[ind+1],m[ind+2],m[ind+3],db,db2); 
      ind+=fNB;
    }
    Vc::float_v res=GetSpline3Vc(v[0],v[1],v[2],v[3],da,da*da);
    XYZ[0] =  res[0];
    XYZ[1] =  res[1];
    XYZ[2] =  res[2];
  } else {
    Float_t da = lA-iA;
    Float_t db = lB-iB;
  
    Float_t vx[4];
    Float_t vy[4];
    Float_t vz[4];
    Int_t ind = iA*fNB  + iB;
    for( Int_t i=0; i<4; i++ ){
      int ind4 = ind*4;
      vx[i] = GetSpline3(fXYZ[ind4+0],fXYZ[ind4+4],fXYZ[ind4 +8],fXYZ[ind4+12],db); 
      vy[i] = GetSpline3(fXYZ[ind4+1],fXYZ[ind4+5],fXYZ[ind4 +9],fXYZ[ind4+13],db); 
      vz[i] = GetSpline3(fXYZ[ind4+2],fXYZ[ind4+6],fXYZ[ind4+10],fXYZ[ind4+14],db); 
      ind+=fNB;
    }
    XYZ[0] =  GetSpline3(vx,da);
    XYZ[1] =  GetSpline3(vy,da);
    XYZ[2] =  GetSpline3(vz,da);
  }
}
