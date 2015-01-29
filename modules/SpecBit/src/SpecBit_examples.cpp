//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Example retrieval of data from Spectrum 
///  object.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (ben.farmer@gmail.com)
///    \date 2014 Dec
///  
///  *********************************************

#include <string>
#include <sstream>
#include <cmath>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>
#include "gambit_module_headers.hpp"
#include "SpecBit_rollcall.hpp"

// includes that are already done by the above headers, 
// just for clarity
#include "Spectrum.hpp"
#include <slhaea.h> 

// For the demo output
#include "stream_overloads.hpp"

namespace Gambit
{

  namespace SpecBit
  {

    using namespace LogTags;

    // Example showing reading of SLHA information from Spectrum object, using
    // both SLHAea objects and direct Spectrum getter functions.
    void exampleRead (bool &result)
    {

      // Retreive pointer to Spectrum object, delivered by dependency resolver
      // Module function asks for Spectrum* with capability MSSM_spectrum.
      // i.e. has DEPENDENCY(MSSM_spectrum, Spectrum*) 
      namespace Pipe = Pipes::exampleRead;
      Spectrum* spec = *Pipe::Dep::MSSM_spectrum;

      // Extract SLHAea object
      // This copies the data out. Could possible change it to pass out a
      // reference instead, or have another function to do that.
      SLHAea::Coll slhaea = spec->getSLHAea();

      // If this is a valid model point, return true and dump information, else false

      // SLHAea objects behave mostly like maps, but with special kinds of keys. For
      // "at" and "operator[]", it does automatic conversion, but for "find" it does
      // not, so we have to manually do it.   
      SLHAea::Block spinfo = slhaea.at("SPINFO");
      std::vector<std::string> k3(1, "3");
      std::vector<std::string> k4(1, "4");

      // See if error code entries exist
      if(spinfo.find(k3) == spinfo.end() and spinfo.find(k4) == spinfo.end())
      {
         std::cout << "Good spectrum found! Inspecting contents..." << std::endl;
         std::cout << std::endl << slhaea << std::endl;
         
         // Write to file so we can check it
         std::string filename(Pipe::runOptions->getValue<std::string>("output_slha_filename"));
         spec->dump2slha(filename);

         // ---------------------------------------------------------
         // BEGIN DEMO OF SPECTRUM OBJECT AND PARTICLE DATABASE
         // ---------------------------------------------------------

         //#define ECHO(COMMAND) cout << "  " << STRINGIFY(COMMAND) << " = " << COMMAND << endl;
         //Replacing with a version that deals with SLHAea access errors            
         #define ECHO(COMMAND)                                \
         {                                                    \
             std::ostringstream msg;                          \
             try { msg << COMMAND; }                          \
             catch (const std::out_of_range& oor)             \
             { msg << "Out of Range error: " << oor.what(); } \
             cout << "  " << STRINGIFY(COMMAND) << " = " << msg.str() << endl; \
         }
         
         /* ----------Test particle database access ---------------- */
         #define PDB Models::ParticleDB()        

         // First check out what is actually in the database
         PDB.check_contents();

         // Demo a couple of particle name retrievals
         cout<<endl;
         cout<<"Demo retrieval of lightest neutralino info from particle database"<<endl;
         cout<<"-----------------------------------------------------------------"<<endl;
         ECHO( PDB.pdg_pair("~chi0_1")                   ) // Input long name, retrieve PDG code + context integer
         ECHO( PDB.pdg_pair("~chi0",1)                   ) // Input short name + index, retrieve PDG code + context integer
         ECHO( PDB.long_name("~chi0",1)                  ) // Input short name + index, retrieve long name
         ECHO( PDB.long_name(std::make_pair(1000022,0))  ) // Input PDG code + context integer, retrieve long name
         ECHO( PDB.long_name(1000022,0)                  ) // Input PDG code + context integer, retrieve long name
         ECHO( PDB.short_name_pair("~chi0_1")            ) // Input long name, retrieve short name + index
         ECHO( PDB.short_name_pair(std::make_pair(1000022,0)) ) // Input PDG code plus context integer, retrieve short name + index 
         ECHO( PDB.short_name_pair(1000022,0)            ) // Input PDG code plus context integer, retrieve short name + index 

         cout<<endl;
         cout<<"Demo retrieval when no short name exists"<<endl;
         cout<<"-----------------------------------------------------------------"<<endl;
         ECHO( PDB.pdg_pair("H+")                        )
         //ECHO( PDB.pdg_pair("H+",1)                      ) // Error!
         //ECHO( PDB.long_name("H+",1)                     ) // Error!
         ECHO( PDB.long_name(std::make_pair(37,0))       )
         ECHO( PDB.long_name(37,0)                       )
         //ECHO( PDB.short_name_pair("H+")                 ) // Error!
         //ECHO( PDB.short_name_pair(std::make_pair(37,0)) ) // Error!
         //ECHO( PDB.short_name_pair(37,0)                 ) // Error!
         //ECHO( PDB.short_name_pair(37)                   ) // Error!
         cout<<endl;
         /* ----------------- Pole masses --------------------------- */
     
         cout<<"Begin demo retrievals from Spectrum object"<<endl;
         cout<<"-----------------------------------------------------------------"<<endl;
         cout<<endl;
         cout<<"First, general methods for accessing different sorts of information."<<endl;
         cout<<endl;
         // At the moment it is only pole masses which have getters overloaded to use
         // the particle database information. It is only the MASS block in the 
         // spectrum generator output SLHA files which use PDG numbers anyway, so I
         // think this makes sense.
         cout<<"Lighest neutral Higgs boson pole mass:"<<endl;
         ECHO(  spec->phys.get_Pole_Mass( PDB.short_name_pair(25,0) )   )
         ECHO(  spec->phys.get_Pole_Mass( PDB.long_name(25,0) )         )
         ECHO(  spec->phys.get_Pole_Mass(25,0)                          )
         ECHO(  spec->phys.get_Pole_Mass( PDB.pdg_pair("h0",1) )        )
         ECHO(  spec->phys.get_Pole_Mass("h0",1)                        )
         ECHO(  spec->phys.get_Pole_Mass("h0_1")                        )

         cout<<endl;
         cout<<"Retrieval of Spectrum object contents, with"<<endl;
         cout<<"correspondence to SLHAea object entries"<<endl;
         cout<<"-----------------------------------------------------------------"<<endl;
    
         // MZ was a bad first example; it is empty unless you switch on the SM pole mass
         // calculator for flexiblesusy. We do not yet pass any input value of MZ though
         // Similar issues with other gauge boson masses. So don't use these yet or
         // you'll get zero for all these masses.
         cout<<endl;
         cout<<"Gauge boson pole masses:"<<endl;
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("Z0")       )
         ECHO(  slhaea.at("SMINPUTS").at(4).at(1)    )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("gamma")    )
         cout<<"  ***Not in slha***"<<endl;
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("W+")       )
         ECHO(  slhaea.at("SMINPUTS").at(9).at(1)    )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("g")        )
         cout<<"  ***Not in slha***"<<endl;
         cout<<endl;
         cout<<"Quark pole masses (actually the slha entries are MSbar except the top mass):"<<endl;
         cout<<endl;
         // I'm a little unclear on what the pole masses for the lighter quarks mean, since I thought
         // that non-perturbative effects made definining them difficult... well anyway will have
         // to ask Peter what is being computed here.

         ECHO(  spec->phys.get_Pole_Mass("u",1)      )  // i.e. up (mass eigenstate)
         ECHO(  slhaea.at("SMINPUTS").at(22).at(1)   )  // mu(2 GeV)^MS-bar, not pole mass
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("u",2)      )  // i.e. charm
         ECHO(  slhaea.at("SMINPUTS").at(24).at(1)   )  // mc(mc)^MS-bar, not pole mass
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("u",3)      )  // i.e. top
         ECHO(  slhaea.at("SMINPUTS").at(6).at(1)    )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("d",1)      )  // i.e. down
         ECHO(  slhaea.at("SMINPUTS").at(21).at(1)   )  // md(2 GeV)^MS-bar, not pole mass
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("d",2)      )  // i.e. strange
         ECHO(  slhaea.at("SMINPUTS").at(23).at(1)   )  // ms(2 GeV)^MS-bar, not pole mass
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("d",3)      )  // i.e. bottom
         ECHO(  slhaea.at("SMINPUTS").at(5).at(1)    )  //  mb(mb)^MS-bar, not pole mass.
         cout<<endl;
         cout<<"Charged fermions pole masses:"<<endl;
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("e-",1)     )  // i.e. electron
         ECHO(  slhaea.at("SMINPUTS").at(11).at(1)   )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("e-",2)     )  // i.e. muon
         ECHO(  slhaea.at("SMINPUTS").at(13).at(1)   )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("e-",3)     )  // i.e. tau
         ECHO(  slhaea.at("SMINPUTS").at(7).at(1)    )
         cout<<endl;
         cout<<"Neutrinos pole masses:"<<endl;
         cout<<endl;
         // These will produce errors because currently no neutrino mass getters are hooked up
         ECHO(  spec->phys.get_Pole_Mass("nu",1)     )  // Just mass ordered (if there is mixing)
         ECHO(  slhaea.at("SMINPUTS").at(12).at(1)   )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("nu",2)     )
         ECHO(  slhaea.at("SMINPUTS").at(14).at(1)   )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("nu",3)     )
         ECHO(  slhaea.at("SMINPUTS").at(8).at(1)    )
         cout<<endl;

         // Now for SUSY particles
         cout<<endl;
         cout<<"MSSM Higgs sector pole masses:"<<endl;
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("h0",1)     )  // Lightest neutral Higgs boson
         ECHO(  slhaea.at("MASS").at(25).at(1)       )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("h0",2)     )  // Heavy neutral Higgs boson
         ECHO(  slhaea.at("MASS").at(35).at(1)       )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("H+")       )  // Charged Higgs
         ECHO(  slhaea.at("MASS").at(37).at(1)       )
         cout<<endl;
         ECHO(  spec->phys.get_Pole_Mass("A0")       )  // Pseudoscalar neutral Higgs
         ECHO(  slhaea.at("MASS").at(36).at(1)       )
         cout<<endl;

         // I'm going to use these macros to save lots of typing for the rest. If you run this function
         // (e.g. using MSSM25_test.yaml) you will see how it all expands. But it is just the
         // same as the examples above, except that the PDG codes are retrieved from the particle database.
         // The PDG code - string name correspondences are defined in 'Models/src/particle_database.cpp'

         #define GET_POLEMASS(LONGNAME)                                         \
         {                                                                      \
            std::ostringstream echo1;                                           \
            std::ostringstream echo2;                                           \
            echo1 <<     "  spec->phys.get_Pole_Mass("<<LONGNAME<<") = ";       \
            double value1 = spec->phys.get_Pole_Mass(LONGNAME);                 \
            echo2 <<  "  slhaea.at(\"MASS\").at("<<PDB.pdg_pair(LONGNAME).first<<").at(1) = "; \
            str value2 = slhaea.at("MASS").at( PDB.pdg_pair(LONGNAME).first ).at(1); \
            cout << echo1.str() << value1 << endl;                              \
            cout << echo2.str() << value2 << endl;                              \
            cout<<endl;                                                         \
         } 

         #define GET_POLEMASS_I(r, DATA, ELEM)                                  \
         {                                                                      \
            std::ostringstream echo1;                                           \
            std::ostringstream echo2;                                           \
            echo1 <<     "  spec->phys.get_Pole_Mass("<<DATA<<","<<ELEM<<") = "; \
            double value1 = spec->phys.get_Pole_Mass(DATA,ELEM);                \
            echo2 <<  "  slhaea.at(\"MASS\").at("<<PDB.pdg_pair(DATA,ELEM).first<<").at(1) = "; \
            str value2 = slhaea.at("MASS").at( PDB.pdg_pair(DATA,ELEM).first ).at(1); \
            cout << echo1.str() << value1 << endl;                              \
            cout << echo2.str() << value2 << endl;                              \
            cout<<endl;                                                         \
         }

         #define GET_POLEMASSES(NAME, __INDICES) BOOST_PP_SEQ_FOR_EACH(GET_POLEMASS_I, NAME, BOOST_PP_TUPLE_TO_SEQ(__INDICES))

         cout<<endl<<"Gaugino pole masses:"<<endl<<endl;
         GET_POLEMASS("~g")
         GET_POLEMASSES("~chi+",(1,2)) 
         GET_POLEMASSES("~chi0",(1,2,3,4)) 
         cout<<endl<<"Squark pole masses:"<<endl<<endl;
         GET_POLEMASSES("~d",(1,2,3,4,5,6)) 
         GET_POLEMASSES("~u",(1,2,3,4,5,6)) 
         cout<<endl<<"Slepton pole masses:"<<endl<<endl;
         GET_POLEMASSES("~e-",(1,2,3,4,5,6)) 
         GET_POLEMASSES("~nu",(1,2,3)) 

         cout << endl << "Mixing matrices:" << endl << endl;

         // Note, currently we are not using a matrix object or any such thing, so you have to
         // extract the elements of each matrix one at a time. It would probably be handy to
         // add such a return type though.

         #define GET_MIX_MATRIX_EL(r, PRODUCT)                                   \
         {                                                                       \
            str label = BOOST_PP_SEQ_ELEM(0, PRODUCT);                           \
            str block = BOOST_PP_SEQ_ELEM(1, PRODUCT);                           \
            int i = BOOST_PP_SEQ_ELEM(2, PRODUCT);                               \
            int j = BOOST_PP_SEQ_ELEM(3, PRODUCT);                               \
            std::ostringstream echo1;                                            \
            std::ostringstream echo2;                                            \
            echo1 <<     "  spec->phys.get_Pole_Mixing("<<label<<","<<i<<","<<j<<") = "; \
            double value1 = spec->phys.get_Pole_Mixing(label,i,j); \
            echo2 <<     "  SLHAea::to<double>( slhaea.at("<<block<<").at("<<i<<","<<j<<").at(2) ) = "; \
            double value2 = SLHAea::to<double>( slhaea.at(block).at(i,j).at(2) ); \
            cout << echo1.str() << value1 <<endl;                                \
            cout << echo2.str() << value2 <<endl;                                \
            cout << endl;                                                        \
         }

         #define GET_MIX_MATRIX(NAME,BLOCK,__IND1,__IND2) BOOST_PP_SEQ_FOR_EACH_PRODUCT(GET_MIX_MATRIX_EL, ((NAME))((BLOCK))(BOOST_PP_TUPLE_TO_SEQ(__IND1))(BOOST_PP_TUPLE_TO_SEQ(__IND2)))

         // The names here could perhaps be improved. They are not so immediately obvious to me.

         GET_MIX_MATRIX("UM","UMIX",(1,2),(1,2)) cout<<endl;
         GET_MIX_MATRIX("UP","VMIX",(1,2),(1,2)) cout<<endl;
         GET_MIX_MATRIX("ZA","PSEUDOSCALARMIX",(1,2),(1,2)) cout<<endl;
         GET_MIX_MATRIX("ZD","DSQMIX",(1,2,3,4,5,6),(1,2,3,4,5,6)) cout<<endl;
         GET_MIX_MATRIX("ZE","SELMIX",(1,2,3,4,5,6),(1,2,3,4,5,6)) cout<<endl;
         GET_MIX_MATRIX("ZH","SCALARMIX",(1,2),(1,2)) cout<<endl;
         GET_MIX_MATRIX("ZN","NMIX",(1,2,3,4),(1,2,3,4)) cout<<endl;
         GET_MIX_MATRIX("ZHPM","CHARGEMIX",(1,2),(1,2)) cout<<endl;
         GET_MIX_MATRIX("ZU","USQMIX",(1,2,3,4,5,6),(1,2,3,4,5,6)) cout<<endl;
         GET_MIX_MATRIX("ZV","SNUMIX",(1,2,3),(1,2,3)) cout<<endl;

         cout<<endl;
         cout << "Next up: running parameters" << endl;
         cout << "These are all given in the DRbar scheme, at least when running FlexibleSUSY or SoftSUSY. ";
         cout << "There may be some switching or converting once other spectrum generator are added." << endl;
         cout<<endl;
         cout << "Spectrum object running parameters are currently defined at scale Q=" 
              << spec->runningpars.GetScale() << " [GeV]" << endl << endl;
         cout<<endl; 
         cout << "-- Dimensionless parameters --" <<endl;          
         cout << endl << "Gauge couplings:" << endl << endl;
         ECHO(  spec->runningpars.get_dimensionless_parameter("g1")  )  // U_Y(1) gauge coupling in SU(5) normalisation  
         ECHO(  slhaea.at("GAUGE").at(1).at(1)  ) // This is in the Standard Model normalisation as per SLHA conventions
         cout << "Note: " << spec->runningpars.get_dimensionless_parameter("g1") << " * sqrt(3/5) = "
                          << spec->runningpars.get_dimensionless_parameter("g1")*sqrt(3./5.) << endl;
         cout<<endl;
         ECHO(  spec->runningpars.get_dimensionless_parameter("g2")  )  // SU(2) gauge coupling
         ECHO(  slhaea.at("GAUGE").at(2).at(1)  )
         cout<<endl;
         ECHO(  spec->runningpars.get_dimensionless_parameter("g3")  )  // SU(3) gauge coupling
         ECHO(  slhaea.at("GAUGE").at(3).at(1)  )
         cout<<endl;
 
         cout << endl << "Yukawa matrices:" << endl << endl;

         // Note, currently we are not using a matrix object or any such thing, so you have to
         // extract the elements of the matrix one at a time. It would probably be handy to
         // add such a return type though.

         #define GET_MATRIX_EL(r, PRODUCT)                                       \
         {                                                                       \
            str label = BOOST_PP_SEQ_ELEM(0, PRODUCT);                           \
            str block = BOOST_PP_SEQ_ELEM(1, PRODUCT);                           \
            int i = BOOST_PP_SEQ_ELEM(2, PRODUCT);                               \
            int j = BOOST_PP_SEQ_ELEM(3, PRODUCT);                               \
            std::ostringstream echo1;                                            \
            std::ostringstream echo2;                                            \
            echo1 <<     "  spec->runningpars.get_dimensionless_parameter("<<label<<","<<i<<","<<j<<") = "; \
            double value1 = spec->runningpars.get_dimensionless_parameter(label,i,j); \
            echo2 <<     "  SLHAea::to<double>( slhaea.at("<<block<<").at("<<i<<","<<j<<").at(2) ) = "; \
            double value2 = SLHAea::to<double>( slhaea.at(block).at(i,j).at(2) ); \
            cout << echo1.str() << value1 <<endl;                                \
            cout << echo2.str() << value2 <<endl;                                \
            cout << endl;                                                        \
         }

         #define GET_MATRIX(NAME,BLOCK,__IND1,__IND2) BOOST_PP_SEQ_FOR_EACH_PRODUCT(GET_MATRIX_EL, ((NAME))((BLOCK))(BOOST_PP_TUPLE_TO_SEQ(__IND1))(BOOST_PP_TUPLE_TO_SEQ(__IND2)))

         GET_MATRIX("Yu","YU",(1,2,3),(1,2,3)) cout << endl; 
         GET_MATRIX("Yd","YD",(1,2,3),(1,2,3)) cout << endl; 
         GET_MATRIX("Ye","YE",(1,2,3),(1,2,3)) cout << endl; 

         // Mass dimension 1 parameters
         
         cout<<endl;
         cout<<"MSSM mass dimension 1 running parameters"<<endl;
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("M1")    )  // Gaugino mass parameter "MassB"
         ECHO(  slhaea.at("MSOFT").at(1).at(1)                )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("M2")    )  // Gaugino mass parameter "MassWB"
         ECHO(  slhaea.at("MSOFT").at(2).at(1)                )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("M3")    )  // Gaugino mass parameter "MassG"
         ECHO(  slhaea.at("MSOFT").at(3).at(1)                )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("Mu")    )  // Superpotential mu parameter
         ECHO(  slhaea.at("HMIX").at(1).at(1)                 )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("vd")    )  // Down-type Higgs vev
         ECHO(  slhaea.at("HMIX").at(102).at(1)               )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass_parameter("vu")    )  // Up-type Higgs vev
         ECHO(  slhaea.at("HMIX").at(103).at(1)               )
         cout<<endl;

         // Matrices

         #define GET_M1_MATRIX_EL(r, PRODUCT)                                    \
         {                                                                       \
            str label = BOOST_PP_SEQ_ELEM(0, PRODUCT);                           \
            str block = BOOST_PP_SEQ_ELEM(1, PRODUCT);                           \
            int i = BOOST_PP_SEQ_ELEM(2, PRODUCT);                               \
            int j = BOOST_PP_SEQ_ELEM(3, PRODUCT);                               \
            std::ostringstream echo1;                                            \
            std::ostringstream echo2;                                            \
            echo1 <<     "  spec->runningpars.get_mass_parameter("<<label<<","<<i<<","<<j<<") = "; \
            double value1 = spec->runningpars.get_mass_parameter(label,i,j); \
            echo2 <<     "  SLHAea::to<double>( slhaea.at("<<block<<").at("<<i<<","<<j<<").at(2) ) = "; \
            double value2 = SLHAea::to<double>( slhaea.at(block).at(i,j).at(2) ); \
            cout << echo1.str() << value1 <<endl;                                \
            cout << echo2.str() << value2 <<endl;                                \
            cout << endl;                                                        \
         }

         #define GET_M1_MATRIX(NAME,BLOCK,__IND1,__IND2) BOOST_PP_SEQ_FOR_EACH_PRODUCT(GET_M1_MATRIX_EL, ((NAME))((BLOCK))(BOOST_PP_TUPLE_TO_SEQ(__IND1))(BOOST_PP_TUPLE_TO_SEQ(__IND2)))

         cout << endl << "Triliner coupling matrices? SLHA says these blocks should be called AU,AD,AE, not TU,TD,TE though, so I'm not sure. Need to check with Peter." << endl << endl;

         // Seem to be the trilinears, and TYu and au etc. seem to be equal. Ask Peter...

         GET_M1_MATRIX("TYu","TU",(1,2,3),(1,2,3)) cout << endl; 
         GET_M1_MATRIX("TYd","TD",(1,2,3),(1,2,3)) cout << endl; 
         GET_M1_MATRIX("TYe","TE",(1,2,3),(1,2,3)) cout << endl; 
         cout << endl; 
         GET_M1_MATRIX("au","TU",(1,2,3),(1,2,3)) cout << endl; 
         GET_M1_MATRIX("ad","TD",(1,2,3),(1,2,3)) cout << endl; 
         GET_M1_MATRIX("ae","TE",(1,2,3),(1,2,3)) cout << endl; 
 
         // Mass dimension 2 parameters

         cout<<endl;
         cout<<"MSSM mass dimension 2 running parameters"<<endl;
         cout<<endl;
         ECHO(  spec->runningpars.get_mass2_parameter("mHd2")  )  // Down-type Higgs soft mass
         ECHO(  slhaea.at("MSOFT").at(21).at(1)               )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass2_parameter("mHu2")  )  // Up-type Higgs soft mass
         ECHO(  slhaea.at("MSOFT").at(22).at(1)               )
         cout<<endl;
         ECHO(  spec->runningpars.get_mass2_parameter("BMu")   )  // Higgs bilinear soft parameter
         ECHO(  slhaea.at("HMIX").at(101).at(1)               )
         cout<<endl;

         // Matrices
           
         #define GET_M2_MATRIX_EL(r, PRODUCT)                                    \
         {                                                                       \
            str label = BOOST_PP_SEQ_ELEM(0, PRODUCT);                           \
            str block = BOOST_PP_SEQ_ELEM(1, PRODUCT);                           \
            int i = BOOST_PP_SEQ_ELEM(2, PRODUCT);                               \
            int j = BOOST_PP_SEQ_ELEM(3, PRODUCT);                               \
            std::ostringstream echo1;                                            \
            std::ostringstream echo2;                                            \
            echo1 <<     "  spec->runningpars.get_mass2_parameter("<<label<<","<<i<<","<<j<<") = "; \
            double value1 = spec->runningpars.get_mass2_parameter(label,i,j); \
            echo2 <<     "  SLHAea::to<double>( slhaea.at("<<block<<").at("<<i<<","<<j<<").at(2) ) = "; \
            double value2 = SLHAea::to<double>( slhaea.at(block).at(i,j).at(2) ); \
            cout << echo1.str() << value1 <<endl;                                \
            cout << echo2.str() << value2 <<endl;                                \
            cout << endl;                                                        \
         }

         #define GET_M2_MATRIX(NAME,BLOCK,__IND1,__IND2) BOOST_PP_SEQ_FOR_EACH_PRODUCT(GET_M2_MATRIX_EL, ((NAME))((BLOCK))(BOOST_PP_TUPLE_TO_SEQ(__IND1))(BOOST_PP_TUPLE_TO_SEQ(__IND2)))

         cout << endl << "Mass matrices:" << endl << endl;

         GET_M2_MATRIX("mq2","MSQ2",(1,2,3),(1,2,3)) cout << endl; 
         GET_M2_MATRIX("mu2","MSU2",(1,2,3),(1,2,3)) cout << endl; 
         GET_M2_MATRIX("md2","MSD2",(1,2,3),(1,2,3)) cout << endl; 
         GET_M2_MATRIX("me2","MSE2",(1,2,3),(1,2,3)) cout << endl; 
         GET_M2_MATRIX("ml2","MSL2",(1,2,3),(1,2,3)) cout << endl; 

         cout << endl; 

         // Things left to add to demo:
         // - Tree-level masses
         // - Standard Model mixings (need to be added to MSSMSpec as well)
         // - some extra stuff to do regarding organising Standard Model input
         //   parameters (and computing/transferring the pole masses)
         // - Need to extend a couple of things in Spectrum.hpp so that we can
         //   compute and return auxilliary things like TanBeta. Of course the
         //   user can currently compute it themselves from the vevs.

         SpecBit_warning().raise(LOCAL_INFO,"Stopped to examine spectrum contents");  
         result = 0;
      }
      else if(spinfo.find(k3) != spinfo.end())
      {
         std::cout << "Bad spectrum: " << spinfo.at(3) << std::endl;
         result = 1;
      }
      else if(spinfo.find(k4) != spinfo.end())
      {
         std::cout << "Bad spectrum: " << spinfo.at(4) << std::endl;
         result = 1;
      }
      else
      {
         SpecBit_error().raise(LOCAL_INFO,"Bug in exampleRead 'if' logic");  
      }
   }

  }      
}













