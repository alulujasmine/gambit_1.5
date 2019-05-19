//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Flexiblesusy model header includes for
///  SpecBit.
///
///  As new models are used in SpecBit, the new
///  required headers should be added here.
///
///  The definitions of the MODELbox structs are
///  also here.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (ben.farmer@gmail.com)
///    \date 2014 Dec, 2015 Jan
///
///  *********************************************

#ifndef MODELFILES_HPP
#define MODELFILES_HPP

#include "SLHAea/slhaea.h"
#include "flexiblesusy/src/lowe.h" // From softsusy; used by flexiblesusy

// Abstraction of algorithm name. For now everything uses 'Two_scale'.
#define ALGORITHM1 Two_scale
// Need a lowercase version for certain file names
#define ALGORITHM1l two_scale


// Make flexiblesusy interface struct for MSSMSpec template class
// We need this so that we can match spectrum objects with different
// routines for translating them into SLHAea objects, while still
// creating the appropriate overloads for the Spectrum class
// 'getSLHAea' virtual function.
//
// The interface class here is specific to flexiblesusy, but in order to work, it just
// need to typedef a Model object with all the right MSSMSpec getter names (like before).
// If extra objects are needed to do this (as in the
// flexiblesusy case), they will need to be carried around by an instance of this interface
// struct, which should be used to construct the MSSMSpec.

#define MAKE_INTERFACE                                                                 \
  namespace Gambit {                                                                   \
    namespace SpecBit {                                                                \
      using namespace flexiblesusy;                                                    \
                                                                                       \
      template <class algorithm_type>                                                  \
      class CAT_3(MODELNAME,_,interface)                                               \
      {                                                                                \
        public:                                                                        \
          /* Typedefs for accessing FlexibleSUSY classes in a generic way */           \
          typedef CAT_3(MODELNAME,_,input_parameters)                   InputParameters;   \
          typedef CAT_3(MODELNAME,_,spectrum_generator<algorithm_type>) SpectrumGenerator; \
          typedef MODELNAME<algorithm_type>                             BaseModel;         \
          typedef CAT_3(MODELNAME,_,slha<BaseModel>)                    Model;             \
          typedef CAT_3(MODELNAME,_,physical)                           Physical;          \
          typedef CAT_3(MODELNAME,_,slha_io)                            SlhaIo;            \
          typedef CAT_3(MODELNAME,_,scales)                             Scales;            \
          static const unsigned number_of_particles = CAT_3(MODELNAME,_,info)::NUMBER_OF_PARTICLES; \
          typedef flexiblesusy::Spectrum_generator_problems             Problems;      \
                                                                                       \
          /* Of the above, only the Model typedef is required by the MSSMSpec
             template. The rest are for internal details, and for use in SpecBit */    \
          /* Generic name for internal model class */                                  \
                                                                                       \
          /* Internal data members for keeping track of objects needed/created by FlexibleSUSY */ \
          Model model;     /* FlexibleSUSY model */                                    \
          softsusy::QedQcd oneset;   /* SoftSUSY format Standard Model parameters */ \
          InputParameters input;    /* Parameters needed to compute points of Model */ \
          Problems problems; /* FlexibleSUSY problems report manager */                \
          Scales scales;   /*scales at shich coundary conditions are applied*/         \
          static const int index_offset = -1; /* indices start from 0 (1 less than Gambit convention) */ \
                                                                                       \
          /* The rest is also internal data only. For now anyway, might need some of it
             accessible at some point, like the input parameters, could add more
             virtual functions for that */                                             \
                                                                                       \
          /* Constructor */                                                            \
          CAT_3(MODELNAME,_,interface) (const SpectrumGenerator& spectrum_generator, const softsusy::QedQcd& onesetIN, const InputParameters& inputIN) \
          : model(spectrum_generator.get_model())                                      \
          , oneset(onesetIN)                                                           \
          , input(inputIN)                                                             \
          , problems()                                                                 \
          , scales()                                                                   \
          {                                                                            \
            scales.HighScale = spectrum_generator.get_high_scale();                    \
            scales.SUSYScale = spectrum_generator.get_susy_scale();                    \
            scales.LowScale  = spectrum_generator.get_low_scale();                     \
          }                                                                            \
                                                                                       \
          /* Constructor 2 (mostly for testing; leave input with default values) */    \
          CAT_3(MODELNAME,_,interface) (const Model& modelIN)                          \
          : model(modelIN)                                                             \
          , oneset()                                                                   \
          , input()                                                                    \
          , problems(CAT_3(MODELNAME,_,info)::particle_names)                          \
          , scales()                                                                   \
          {}                                                                           \
                                                                                       \
          /* Make sure the slhaio object is up to date in preparation for writing
             output */                                                                 \
          void fill_slhaio(SlhaIo& slhaio) const                                      \
          {                                                                            \
             slhaio.set_spinfo(problems);                                             \
             slhaio.set_sminputs(oneset);                                             \
             slhaio.set_minpar(input);                                                \
             slhaio.set_extpar(input);                                                \
             if (!problems.have_problem()) {                                           \
                slhaio.set_spectrum(model);                                           \
                slhaio.set_extra(model,scales);                                       \
             }                                                                         \
          }                                                                            \
                                                                                       \
      };                                                                               \
      /* I think that there is a need to ensure that these templates actually get
         instantiated; was getting weird segfaults accessing the (virtual) getters
         via a Spectrum*.

         Should add instantiations for each algorithm we want to use
         (just one for now) */                                                         \
      /* Actually maybe we don't need these (and they cause duplicate symbol errors in clang */ \
      /*template class CAT_3(MODELNAME,_,interface)<ALGORITHM1>; */                        \
    }                                                                                  \
  }


/// All the includes and Model_interface definitions
/// Have now moved the MAKE_INTERFACE command inside the include automater, because that
/// has some automagic stuff to prevent it doing anything when the corresponding FlexibleSUSY
/// model has not been compiled.
#define MODELNAME CMSSM
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary CMSSM model headers
#undef MODELNAME

/// {@ Versions of MSSM spectrum generators that take
#define MODELNAME MSSM
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MSSM model headers etc.
#undef MODELNAME

#define MODELNAME MSSMatMGUT
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MSSMatMGUT model headers etc.
#undef MODELNAME
/// @}

/// {@ Versions of MSSM spectrum generators that take mA and mu as inputs
#define MODELNAME MSSM_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MSSMatMGUT model headers etc.
#undef MODELNAME

#define MODELNAME MSSMatMSUSY_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MSSMatMGUT model headers etc.
#undef MODELNAME

#define MODELNAME MSSMatMGUT_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MSSMatMGUT model headers etc.
#undef MODELNAME
/// @}

#define MODELNAME MSSMEFTHiggs
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp"// Automatically includes necessary MSSMEFTHiggs model headers etc.
#undef MODELNAME
/// @}

#define MODELNAME MSSMEFTHiggs_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp"// Automatically includes necessary MSSMEFTHiggs_mAmu model headers etc.
#undef MODELNAME
/// @}

#define MODELNAME MSSMatMSUSYEFTHiggs_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp"// Automatically includes necessary MSSMatMSUSYEFTHiggs_mAmu model headers etc.
#undef MODELNAME
/// @}

#define MODELNAME MSSMatMGUTEFTHiggs
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp"// Automatically includes necessary MSSMatMGUTEFTHiggs model headers etc.
#undef MODELNAME
/// @}
   
#define MODELNAME MSSMatMGUTEFTHiggs_mAmu
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp"// Automatically includes necessary MSSMatMGUTEFTHiggs_mAmu model headers etc.
#undef MODELNAME
/// @}

#define MODELNAME ScalarSingletDM_Z2
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary ScalarSingletDM_Z2 model headers etc.
#undef MODELNAME

#define MODELNAME ScalarSingletDM_Z3
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary ScalarSingletDM_Z3 model headers etc.
#undef MODELNAME

#define MODELNAME MDM
#include "gambit/SpecBit/flexiblesusy_include_automater.hpp" // Automatically includes necessary MDM model headers etc.
#undef MODELNAME

#endif
