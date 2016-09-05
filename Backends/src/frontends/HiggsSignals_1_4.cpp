//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file HiggsSignals.cpp
///
///  Frontend source for the HiggsSignals backend.
///
///  Actual implementation of HiggsSignals ini function.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Christopher S. Rogan
///          (crogan@cern.ch)
///  \date 2015 Sept
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/backend_singleton.hpp"
#include "gambit/Backends/frontends/HiggsSignals_1_4.hpp"
#include "gambit/Utils/file_lock.hpp"

void ConvertToFortran(char* fstring, std::size_t fstring_len,
                      const char* cstring)
{
    std::size_t inlen = std::strlen(cstring);
    std::size_t cpylen = std::min(inlen, fstring_len);

    if (inlen > fstring_len)
    {
        // TODO: truncation error or warning
    }

    std::copy(cstring, cstring + cpylen, fstring);
    std::fill(fstring + cpylen, fstring + fstring_len, ' ');
}



BE_INI_FUNCTION
{

  static bool scan_level = true;
  if(scan_level)
  {
    int nHneut = 3; // number of neutral higgses
    int nHplus = 1; // number of charged higgses
    int pdf = 2;    // choose which pdf style to use for Higgs lineshape; 2 = Gaussian
    // Initialize HiggsSignals. Do this one-by-one for each MPI process with
    // locks, as HS calls HB, which writes files then reads them back in later (crazy).
    // Note that this is the Higgs*Bounds* lock, as in the HiggsBounds ini function,
    // because we need to make sure that neither this function nor the HB ini function
    // run at the same time, beccause both of them cause the HiggsBounds files to be
    // written.

    // Find all the versions of HiggsBounds that have been successfully loaded, and get
    // their locks.
    std::vector<str> hbversions = Backends::backendInfo().working_safe_versions("HiggsBounds");    
    std::vector<Utils::FileLock> mylocks;
    for (auto it = hbversions.begin(); it != hbversions.end(); ++it)
    {
      mylocks.emplace_back("HiggsBounds_" + *it + "_init");
      mylocks.back().get_lock();
    }
    { 
      // initialize HiggsSignals with the latest results and set pdf shape
      std::string data_path = runOptions->getValueOrDef<std::string>("latestresults", "HS_data");
      cout << "using data path = " << data_path << endl;
      const char * exp_data = strdup(data_path.c_str());//strdup(data_path.c_str());
      cout << "exp_data = " << exp_data << endl;
      char cstring[2][4] = { "abc", "xyz" };
      char string[2][4];

      ConvertToFortran(string[0], sizeof string[0], cstring[0]);
      ConvertToFortran(string[1], sizeof string[1], cstring[1]);

      std::cout << "c++: string[0] = '" << cstring[0] << "'" << std::endl;
      std::cout << "c++: string[1] = '" << cstring[1] << "'" << std::endl;

      
      
      initialize_HiggsSignals(nHneut,nHplus,string);
      setup_pdf(pdf);
    }
    for (auto it = mylocks.begin(); it != mylocks.end(); ++it)
    {
      it->release_lock();    
    }
    scan_level = false;
  }

}
END_BE_INI_FUNCTION
