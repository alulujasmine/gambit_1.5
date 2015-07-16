//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  declaration for scanner module
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date Feb 2014
///
///  *********************************************

#ifndef PLUGIN_PRIOR_HPP
#define PLUGIN_PRIOR_HPP

#include "gambit/ScannerBit/priors.hpp"
#include "gambit/ScannerBit/plugin_interface.hpp"

namespace Gambit
{
        namespace Priors
        {
                namespace plugin_prior_utils
                {
                        inline std::string inputName(const Options& options)
                        {
                                if (options.hasKey("plugin"))
                                {
                                        return options.getValue<std::string>("plugin");
                                }
                                else
                                {
                                        scan_err << "Plugin prior:  need to specify plugin with \"plugin\" tag." << scan_end;
                                        return "";
                                }
                        }
                }
                
                class Plugin : public BasePrior, public Scanner::Plugins::Plugin_Interface<double (const std::vector<double> &), void (const std::vector<double> &, std::unordered_map<std::string,double> &)>
                {
                private:
                        std::vector<std::string> param_names;
                        
                public:
                        Plugin(const std::vector<std::string>& params, const Options& options) : 
                                Scanner::Plugins::Plugin_Interface<double (const std::vector<double>&), void (const std::vector<double> &, std::unordered_map<std::string,double> &)>("objective", plugin_prior_utils::inputName(options), params, sizeRef())
                        {
                        }
                        
                        void transform(const std::vector<double> &unitpars, std::unordered_map<std::string,double> &outputMap) const
                        {
                                return const_cast<Plugin *>(this)->Scanner::Plugins::Plugin_Interface<double (const std::vector<double>&), void (const std::vector<double> &, std::unordered_map<std::string,double> &)>::operator()(unitpars, outputMap);
                        }
                        
                        double operator()(const std::vector<double>& vec) const
                        {
                                return const_cast<Plugin *>(this)->Scanner::Plugins::Plugin_Interface<double (const std::vector<double>&), void (const std::vector<double> &, std::unordered_map<std::string,double> &)>::operator()(vec);
                        }
                };
                
                LOAD_PRIOR(plugin, Plugin)
        }
}

#endif
