//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
//
//  Scalar singlet dark matter with running mass
//  and quartic coupling
//  *********************************************
//
//  Authors
//  =======
//
//  (add name and date if you modify)
//  Christoph Weniger
//  2014 January
//
//  James McKay
//  2015 September
//
//  *********************************************

#ifndef __SCDM_hpp__
#define __SCDM_hpp__

#define MODEL SCDM
  START_MODEL
  DEFINEPARS(mS2, lambda_hS,lambda_S,mu3)
#undef MODEL

#endif