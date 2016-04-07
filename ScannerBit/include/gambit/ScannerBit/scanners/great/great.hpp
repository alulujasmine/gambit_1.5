//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  ScannerBit interface to GreAT 1.0.0
///
///  Header file
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Antje Putze
///          (putze@lapth.cnrs.fr)
///  \date 2015 Nov
///
///  *********************************************

#ifndef __great_hpp__
#define __great_hpp__

#include "gambit/ScannerBit/scanner_plugin.hpp"

class TGreatPoint;

namespace Gambit
{
  namespace GreAT
  {
    /// Structure for passing likelihood and printer data through GreAT to the objective function.
    struct greatScanData
    {
      Scanner::like_ptr likelihood_function;
      Scanner::printer_interface* printer;
    };

    /// Function to be minimised by GreAT
    //double objective(double params[], const int param_dim, int &fcall, bool &quit, const bool validvector, void*& context);
    double LogLikelihoodFunction(TGreatPoint& point);
  }
}

#endif // #defined __great_hpp__
