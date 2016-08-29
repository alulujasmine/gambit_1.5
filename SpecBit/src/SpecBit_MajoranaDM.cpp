//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module SpecBit
///
///  SpecBit module functions related to the
///  MajoranaDM model (file format is based on
///  the SpecBit_SingletDM.cpp)
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ankit Beniwal
///          (ankit.beniwal@adelaide.edu.au)
///    \date 2016 Aug
///
///  *********************************************

#include <string>
#include <sstream>

#include "gambit/Elements/gambit_module_headers.hpp"

#include "gambit/Elements/spectrum.hpp"
#include "gambit/Utils/stream_overloads.hpp" // Just for more convenient output to logger
#include "gambit/Utils/util_macros.hpp"

#include "gambit/SpecBit/SpecBit_rollcall.hpp"
#include "gambit/SpecBit/SpecBit_helpers.hpp"
#include "gambit/SpecBit/QedQcdWrapper.hpp"
#include "gambit/Models/SimpleSpectra/SMHiggsSimpleSpec.hpp"
#include "gambit/Models/SimpleSpectra/MajoranaDMSimpleSpec.hpp"
#include "gambit/SpecBit/model_files_and_boxes.hpp"

#include "gambit/SpecBit/MajoranaDMSpec.hpp"

//#include "gambit/SpecBit/SMskeleton.hpp"

// Flexible SUSY stuff (should not be needed by the rest of gambit)
// #include "flexiblesusy/src/ew_input.hpp"
// #include "flexiblesusy/src/lowe.h" // From softsusy; used by flexiblesusy
// #include "flexiblesusy/src/numerics2.hpp"
// #include "flexiblesusy/src/two_loop_corrections.hpp"
// Switch for debug mode
//#define SPECBIT_DEBUG

namespace Gambit
{

  namespace SpecBit
  {
    using namespace LogTags;
    using namespace flexiblesusy;

    /// Get a (simple) Spectrum object wrapper for the MajoranaDM model
    void get_MajoranaDM_spectrum(Spectrum& result)
    {
      namespace myPipe = Pipes::get_MajoranaDM_spectrum;
      const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;

      // Initialise an object to carry the Majorana plus Higgs sector information
      Models::MajoranaDMModel majoranamodel;
      
      // quantities needed to fill container spectrum, intermediate calculations
      double alpha_em = 1.0 / sminputs.alphainv;
      double mz2 = pow(sminputs.mZ,2);
      double GF = sminputs.GF;
      double sinW2cosW2 = Pi * alpha_em / (pow(2,0.5) * mz2 * GF ) ;
      double e = pow( 4*Pi*( alpha_em ),0.5) ;

      double sin2W = pow(2 * sinW2cosW2, 0.5);
      double tW = 0.5* asin( sin2W );
      double sinW2 = pow( sin (tW) , 2);
      double cosW2 = pow( cos (tW) , 2);
      
      // Higgs sector
      double mh   = *myPipe::Param.at("mH");
      majoranamodel.HiggsPoleMass   = mh;
      
      double vev        = 1. / sqrt(sqrt(2.)*GF);
      majoranamodel.HiggsVEV        = vev;
      // majoranamodel.LambdaH   = GF*pow(mh,2)/pow(2,0.5) ;
      
      // MajoranaDM sector    
      majoranamodel.MajoranaPoleMass = *myPipe::Param.at("mX");
      majoranamodel.MajoranaLambda   = *myPipe::Param.at("lX");    
      majoranamodel.MajoranacosXI    = *myPipe::Param.at("cosXI");    
      
      // Check if lX >= 4pi/2mX (i.e., where EFT approach breaks down)
      if (majoranamodel.MajoranaLambda >= (4*M_PI)/(2*majoranamodel.MajoranaPoleMass))
      {
       std::ostringstream msg;
       msg << "Model points mX = " << majoranamodel.MajoranaPoleMass
           << " GeV and lambdaX = " << majoranamodel.MajoranaLambda
           << " GeV are invalid within the EFT approach!";
       invalid_point().raise(msg.str());
      }

      // Standard model
      majoranamodel.sinW2 = sinW2;
      
      // gauge couplings
      majoranamodel.g1 = e / sinW2;
      majoranamodel.g2 = e / cosW2;
      majoranamodel.g3   = pow( 4*Pi*( sminputs.alphaS ),0.5) ;
      
      double sqrt2v = pow(2.0,0.5)/vev;

      // Yukawas
      majoranamodel.Yu[0] = sqrt2v * sminputs.mU;
      majoranamodel.Yu[1] = sqrt2v * sminputs.mCmC;
      majoranamodel.Yu[2] = sqrt2v * sminputs.mT;
      
      majoranamodel.Ye[0] = sqrt2v * sminputs.mE;
      majoranamodel.Ye[1] = sqrt2v * sminputs.mMu;
      majoranamodel.Ye[2] = sqrt2v * sminputs.mTau;
      
      majoranamodel.Yd[0] = sqrt2v * sminputs.mD;
      majoranamodel.Yd[1] = sqrt2v * sminputs.mS;
      majoranamodel.Yd[2] = sqrt2v * sminputs.mBmB;
            
      // Create a SubSpectrum object to wrap the EW sector information
      Models::MajoranaDMSimpleSpec majoranaspec(majoranamodel);

      // We don't supply a LE subspectrum here; an SMSimpleSpec will therefore be automatically created from 'sminputs'
      result = Spectrum(majoranaspec,sminputs,&myPipe::Param);
    }
    
    
    // print spectrum out, stripped down copy from MSSM version with variable names changed
    void fill_map_from_MajoranaDMspectrum(std::map<std::string,double>&, const Spectrum&);
   
    void get_MajoranaDM_spectrum_as_map (std::map<std::string,double>& specmap)
    {
      namespace myPipe = Pipes::get_MajoranaDM_spectrum_as_map;
      const Spectrum& majoranadmspec(*myPipe::Dep::MajoranaDM_spectrum);
      fill_map_from_MajoranaDMspectrum(specmap, majoranadmspec);
    }
    
    void fill_map_from_MajoranaDMspectrum(std::map<std::string,double>& specmap, const Spectrum& majoranadmspec)
    {
      /// Add everything... use spectrum contents routines to automate task
      static const SpectrumContents::MajoranaDM contents;
      static const std::vector<SpectrumParameter> required_parameters = contents.all_parameters();
      
      for(std::vector<SpectrumParameter>::const_iterator it = required_parameters.begin();
           it != required_parameters.end(); ++it)
      {
         const Par::Tags        tag   = it->tag();
         const std::string      name  = it->name();
         const std::vector<int> shape = it->shape();

         /// Verification routine should have taken care of invalid shapes etc, so won't check for that here.

         // Check scalar case
         if(shape.size()==1 and shape[0]==1)
         {
           std::ostringstream label;
           label << name <<" "<< Par::toString.at(tag);
           specmap[label.str()] = majoranadmspec.get_HE().get(tag,name);
         }
         // Check vector case
         else if(shape.size()==1 and shape[0]>1)
         {
           for(int i = 1; i<=shape[0]; ++i) {
             std::ostringstream label;
             label << name <<"_"<<i<<" "<< Par::toString.at(tag);
             specmap[label.str()] = majoranadmspec.get_HE().get(tag,name,i);
           }
         }
         // Check matrix case
         else if(shape.size()==2)
         {
           for(int i = 1; i<=shape[0]; ++i) {
             for(int j = 1; j<=shape[0]; ++j) {
               std::ostringstream label;
               label << name <<"_("<<i<<","<<j<<") "<<Par::toString.at(tag);
               specmap[label.str()] = majoranadmspec.get_HE().get(tag,name,i,j);
             }  
           }
         }
         // Deal with all other cases
         else
         {
           // ERROR
           std::ostringstream errmsg;           
           errmsg << "Error, invalid parameter received while converting MajoranaDMspectrum to map of strings! This should no be possible if the spectrum content verification routines were working correctly; they must be buggy, please report this.";
           errmsg << "Problematic parameter was: "<< tag <<", " << name << ", shape="<< shape; 
           utils_error().forced_throw(LOCAL_INFO,errmsg.str());
         }
      }

    }
    
  } // end namespace SpecBit
} // end namespace Gambit

