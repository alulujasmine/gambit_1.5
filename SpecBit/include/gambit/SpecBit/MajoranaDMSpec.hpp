//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  MajoranaDM derived version of SubSpectrum class
///  (file format is based on SingletDMSpec.hpp).
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ankit Beniwal  
///          (ankit.beniwal@adelaide.edu.au)
///  \date 2016, Aug
///
///  *********************************************

#ifndef MAJORANADMSPEC_H
#define MAJORANADMSPEC_H

#include <memory>

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Elements/subspectrum.hpp"
#include "gambit/Elements/slhaea_helpers.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/SpecBit/MajoranaDMSpec_head.hpp"   // "Header" declarations for MajoranaDMSpec class

// Flexible SUSY stuff (should not be needed by the rest of gambit)
#include "flexiblesusy/config/config.h"


namespace Gambit
{

   namespace SpecBit
   {
      template <class MI>
      const int MajoranaDMSpec<MI>::_index_offset = MI::index_offset;

      // NOTE!! mi is COPIED into the object, so when we get the reference to the
      // actual Model object to store in 'model', we need to use the copy inside
      // the object. So also need to make sure 'model_interface' is initialised first
      // (i.e. it should be declared first)
      template <class MI>
      MajoranaDMSpec<MI>::MajoranaDMSpec(MI mi, str be_name, str be_version)
         : backend_name(be_name)
         , backend_version(be_version)
         , model_interface(mi)
      {}
  
  
      template <class MI>
      MajoranaDMSpec<MI>::MajoranaDMSpec()
      {}
     
      template <class MI>
      MajoranaDMSpec<MI>::~MajoranaDMSpec()
      {}
     
      template <class MI>
      void MajoranaDMSpec<MI>::RunToScaleOverride(double scale)
      {
        model_interface.model.run_to(scale);
      }
      template <class MI>
      double MajoranaDMSpec<MI>::GetScale() const
      {
        return model_interface.model.get_scale();
      }
      template <class MI>
      void MajoranaDMSpec<MI>::SetScale(double scale)
      {
        model_interface.model.set_scale(scale);
      }
      
      template <class MI>
      std::string MajoranaDMSpec<MI>::AccessError(std::string state) const
      {
        std::string errormsg;
        errormsg = "Error accessing "+ state + " element is out of bounds";
        return errormsg;
      }
     
      template <class Model>
      double get_sinthW2_MSbar(const Model& model)
      {
       double sthW2 = Utils::sqr(model.get_g1()) * 0.6 / 
                      (0.6 * Utils::sqr(model.get_g1()) + 
                      Utils::sqr(model.get_g2()));
       return sthW2;
      }
      
//
//      
//      template <class Model>
//      void set_Mhh_pole_slha(Model& model,double mass)
//      {
//        model.get_physical_slha().Mhh = mass;
//      }
//
//
//      template <class Model>
//      void set_Mss_pole_slha(Model& model, double mass)
//      {
//        model.get_physical_slha().Mss = mass;
//      }
//
//      
//
//      template <class Model>
//      void set_neutral_goldstone_pole_slha(Model& model, double mass)
//      {
//        model.get_physical_slha().MAh = mass;
//      }
//     
//           template <class Model>
//      void set_MAh_pole_slha(Model& model, double mass)
//      {
//        model.get_physical_slha().MAh = mass;
//      }
//     
//     
//
//     //PA:  setting MZ and MW is necessary because we may have them as ouptuts
//     template <class Model>
//     void set_MZ_pole_slha(Model& model, double mass)
//     {
//        model.get_physical_slha().MVZ = mass;
//     }
//
//     template <class Model>
//     void set_MW_pole_slha(Model& model, double mass)
//     {
//        model.get_physical_slha().MVWp = mass;
//     }
//
//     
//     template <class Model>
//     void set_MGluon(Model& model, double mass)
//     {
//        model.get_physical().MVG = mass;
//     }
//     
//     template <class Model>
//     void set_MPhoton(Model& model, double mass)
//     {
//        model.get_physical().MVP = mass;
//     }
//
//
//      template <class Model>
//      double get_MAh_pole_slha(const Model& model)
//      {
//        return model.get_MAh_pole_slha();
//      }
//     
//      template <class Model>
//      double get_Mss_pole_slha(const Model& model)
//      {
//        return model.get_Mss_pole_slha();
//      }



      template <class MI>
      typename MajoranaDMSpec<MI>::GetterMaps MajoranaDMSpec<MI>::fill_getter_maps()
      {
         typename MajoranaDMSpec<MI>::GetterMaps map_collection;
         typedef typename MI::Model Model;

         typedef typename MTget::FInfo1 FInfo1;
         typedef typename MTget::FInfo2 FInfo2;

         // Can't use c++11 initialise lists, se have to initialise the index sets like this.
         static const int i01v[] = {0,1};
         static const std::set<int> i01(i01v, Utils::endA(i01v));

         static const int i012v[] = {0,1,2};
         static const std::set<int> i012(i012v, Utils::endA(i012v));

         static const int i0123v[] = {0,1,2,3};
         static const std::set<int> i0123(i0123v, Utils::endA(i0123v));

         static const int i012345v[] = {0,1,2,3,4,5};
         static const std::set<int> i012345(i012345v, Utils::endA(i012345v));
        

        {
            typename MTget::fmap0 tmp_map;
            tmp_map["mu2"] = &Model::get_muH;
            map_collection[Par::mass2].map0 = tmp_map;
         }
        


         //dimensionless - mass dimension 0 parameters

         {
            typename MTget::fmap0 tmp_map;
            tmp_map["g1"]= &Model::get_g1;
            tmp_map["g2"]= &Model::get_g2;
            tmp_map["g3"]= &Model::get_g3;
            tmp_map["lambda_h"]= &Model::get_LamH;
            tmp_map["lX"]= &Model::get_lX;

            map_collection[Par::dimensionless].map0 = tmp_map;
         }
        
        
          {
            typename MTget::fmap0_extraM tmp_map;
            tmp_map["sinW2"] = &get_sinthW2_MSbar<Model>;
            map_collection[Par::dimensionless].map0_extraM = tmp_map;
         }
        


         {
            typename MTget::fmap0 tmp_map;
            tmp_map["vev"] = &Model::get_v;
            tmp_map["lX"]= &Model::get_lX;  // ??? not sure why it wants it here
            map_collection[Par::mass1].map0 = tmp_map;
         }

         // Functions utilising the "extraM" function signature
         // (Zero index, model object as argument)

   
         // Functions utilising the two-index "plain-vanilla" function signature
         // (Two-index member functions of model object)
         {
            typename MTget::fmap2 tmp_map;

            tmp_map["Yd"]= FInfo2( &Model::get_Yd, i012, i012);
            tmp_map["Yu"]= FInfo2( &Model::get_Yu, i012, i012);
            tmp_map["Ye"]= FInfo2( &Model::get_Ye, i012, i012);

            map_collection[Par::dimensionless].map2 = tmp_map;
         }
 
         // Functions utilising the one-index "plain-vanilla" function signature
         // (One-index member functions of model object)
         {
            typename MTget::fmap1 tmp_map;

            ////    tmp_map["S"] = FInfo1( &Model::get_Mss, i012345 );
           ////     tmp_map["h0"] = FInfo1( &Model::get_Mhh, i01 );
            //Here we may access the goldstone boson
            // and higgs. maybe too dangerous to keep?

            //Here we may access the goldstone boson
            //and higgs. maybe too dangerous to keep?
            tmp_map["d"] =    FInfo1( &Model::get_MFd, i012 );
            tmp_map["u"] =    FInfo1( &Model::get_MFu, i012 );
            tmp_map["e-"] =   FInfo1( &Model::get_MFe, i012 );
            tmp_map["e"] =    FInfo1( &Model::get_MFe, i012 );
            tmp_map["dbar"] = FInfo1( &Model::get_MFd, i012 );
            tmp_map["ubar"] = FInfo1( &Model::get_MFu, i012 );
            tmp_map["e+"] =   FInfo1( &Model::get_MFe, i012 );

            map_collection[Par::mass_eigenstate].map1 = tmp_map;
         }
        
        
         {  
            typename MTget::fmap0 tmp_map;
                   
            // ***REMOVED THESE! Leave them to the QedQcdWrapper.***
            // reinstating the Z and W getters as otherwise there is no
            // point in having the setters!
            tmp_map["Z0"] = &Model::get_MVZ_pole_slha;
            //// //tmp_map["g"] = &Model::get_MGluon_pole_slha;
             tmp_map["g"] = &Model::get_MVG_pole_slha;


            map_collection[Par::Pole_Mass].map0 = tmp_map;
         } 

         // Functions utilising the "extraM" function signature
         // (Zero index, model object as argument)
//         {
//            typename MTget::fmap0_extraM tmp_map;
//        
//            // Using wrapper functions defined above
//            tmp_map["A0"] = &get_MAh_pole_slha<Model>;
//
//      
//            map_collection[Par::Pole_Mass].map0_extraM = tmp_map;
//         }

         // Functions utilising the one-index "plain-vanilla" function signature
         // (One-index member functions of model object)
         {  
            typename MTget::fmap0 tmp_map;

            tmp_map["X"] =  &Model::get_Mxx_pole_slha;
            tmp_map["Majorana"] =  &Model::get_Mxx_pole_slha; // alternative naming convention as in MajoranaDM container
            tmp_map["h0"] = &Model::get_Mhh_pole_slha;
            tmp_map["h0_1"] = &Model::get_Mhh_pole_slha; //added to match SM Higgs container naming

            tmp_map["A0"] = &Model::get_MAh_pole_slha;


            map_collection[Par::Pole_Mass].map0 = tmp_map;
         }


         return map_collection;
      } 

      // Filler function for setter function pointer maps extractable from "runningpars" container
      template <class MI>
      typename MajoranaDMSpec<MI>::SetterMaps MajoranaDMSpec<MI>::fill_setter_maps()
      {
         typename MajoranaDMSpec<MI>::SetterMaps map_collection;
         typedef typename MI::Model Model;

         typedef typename MTset::FInfo1 FInfo1;
         typedef typename MTset::FInfo2 FInfo2;

         // Can't use c++11 initialise lists, se have to initialise the index sets like this.
         static const int i01v[] = {0,1};
         static const std::set<int> i01(i01v, Utils::endA(i01v));

         static const int i012v[] = {0,1,2};
         static const std::set<int> i012(i012v, Utils::endA(i012v));

         static const int i0123v[] = {0,1,2,3};
         static const std::set<int> i0123(i0123v, Utils::endA(i0123v));

         static const int i012345v[] = {0,1,2,3,4,5};
         static const std::set<int> i012345(i012345v, Utils::endA(i012345v));
         
         /// @{ mass2 - mass dimension 2 parameters
         //
         // Functions utilising the "plain-vanilla" function signature
         // (Zero index member functions of model object)
         {
            typename MTset::fmap0 tmp_map;

            tmp_map["mS2"] = &Model::set_muS;
            tmp_map["mu2"] = &Model::set_muH;

            map_collection[Par::mass2].map0 = tmp_map;
         }
 
         //
         // Functions utilising the "plain-vanilla" function signature
         // (Zero index member functions of model object)
         {
            typename MTset::fmap0 tmp_map;
            tmp_map["vev"] = &Model::set_v;

            map_collection[Par::mass1].map0 = tmp_map;
         }

         //
         // Functions utilising the "plain-vanilla" function signature
         // (Zero index member functions of model object)
         {
            typename MTset::fmap0 tmp_map;
            tmp_map["g1"]= &Model::set_g1;
            tmp_map["g2"]= &Model::set_g2;
            tmp_map["g3"]= &Model::set_g3;
            tmp_map["lambda_h"]= &Model::set_LamH;
            tmp_map["lX"]= &Model::set_lX;

            map_collection[Par::dimensionless].map0 = tmp_map;
         }

         // Functions utilising the two-index "plain-vanilla" function signature
         // (Two-index member functions of model object)
         {
            typename MTset::fmap2 tmp_map;

            tmp_map["Yd"]= FInfo2( &Model::set_Yd, i012, i012);
            tmp_map["Yu"]= FInfo2( &Model::set_Yu, i012, i012);
            tmp_map["Ye"]= FInfo2( &Model::set_Ye, i012, i012);

            map_collection[Par::dimensionless].map2 = tmp_map;
         }

//        {  
//          typename MTset::fmap0_extraM tmp_map;
//          tmp_map["A0"] = &set_MAh_pole_slha<Model>;
//          tmp_map["Goldstone0"] = &set_neutral_goldstone_pole_slha<Model>;
//   
//          /// the getters for these were removed but Pat last meeting
//          /// we agreed to add setters here unless I misunderstood.
//          /// need to discuss this
//          tmp_map["W+"] = &set_MW_pole_slha<Model>;
//          tmp_map["W-"] = &set_MW_pole_slha<Model>;
//          tmp_map["Z0"] = &set_MZ_pole_slha<Model>;
//       
//          map_collection[Par::Pole_Mass].map0_extraM = tmp_map;
//        }

//        {  
//          typename MTset::fmap0_extraM tmp_map;
//
//          tmp_map["h0"] = &Model::set_Mhh_pole_slha;// &set_Mhh_pole_slha<Model>;
//          tmp_map["s0"] = &Model::set_Mss_pole_slha;//&set_Mss_pole_slha<Model>;
//          
//          map_collection[Par::Pole_Mass].map0_extraM = tmp_map;
//        }


         return map_collection;
      } 

      /// @}
  
   } // end SpecBit namespace 
} // end Gambit namespace

#endif
