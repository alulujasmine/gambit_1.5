//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functor template class definitions.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (patscott@physics.mcgill.ca)
///  \date 2013 Apr-July, Dec
///  \date 2014 Jan, Mar-May, Sep
///  \date 2015 Apr
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///   \date 2013 Apr, Nov
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 May, June, July
///
///  \author Ben Farmer
///          (benjamin.farmer@monash.edu.au)
///  \date 2013 July, Sep
///  \date 2014 Jan
///
///  \author Lars A. Dal
///          (l.a.dal@fys.uio.no)
///  \date 2015 Jan
///
///  *********************************************

#include <chrono>

#include "gambit/Elements/functors.hpp"
#include "gambit/Utils/standalone_error_handlers.hpp"
#include "gambit/Utils/signal_handling.hpp"
#include "gambit/Models/models.hpp"
#include "gambit/Logs/log.hpp"
#include "gambit/Printers/baseprinter.hpp"

#include <boost/preprocessor/seq/for_each.hpp>


namespace Gambit
{
  using namespace LogTags;

    /// Class methods for actual module functors for TYPE != void

    template <typename TYPE>
    module_functor<TYPE>::module_functor(void (*inputFunction)(TYPE &),
                                   str func_name,
                                   str func_capability,
                                   str result_type,
                                   str origin_name,
                                   Models::ModelFunctorClaw &claw)
    : module_functor_common(func_name, func_capability, result_type, origin_name, claw),
      myFunction        (inputFunction),
      myValue           (NULL),
      myPrintFlag       (false),
      myTimingPrintFlag (false)
    {}

    /// Destructor
    template <typename TYPE>
    module_functor<TYPE>::~module_functor()
    {
      if (myValue != NULL) delete [] myValue;
    }

    /// Setter for indicating if the wrapped function's result should be printed
    template <typename TYPE>
    void module_functor<TYPE>::setPrintRequirement(bool flag) { if (this == NULL) failBigTime("setPrintRequirement"); myPrintFlag = flag;}
    /// Setter for indicating if the timing data for this function's execution should be printed
    template <typename TYPE>
    void module_functor<TYPE>::setTimingPrintRequirement(bool flag) { if (this == NULL) failBigTime("setTimingPrintRequirement"); myTimingPrintFlag = flag;}

    /// Getter indicating if the wrapped function's result should be printed
    template <typename TYPE>
    bool module_functor<TYPE>::requiresPrinting() const { if (this == NULL) failBigTime("requiresPrinting"); return myPrintFlag; }

    /// Getter indicating if the timing data for this function's execution should be printed
    template <typename TYPE>
    bool module_functor<TYPE>::requiresTimingPrinting() const { if (this == NULL) failBigTime("requiresTimingPrinting"); return myTimingPrintFlag; }

    /// Calculate method
    /// (no loop-manager stuff here because only void specialisation can manage loops)
    template <typename TYPE>
    void module_functor<TYPE>::calculate()
    {
      if(not signaldata().shutdown_begun())          // If shutdown signal has been received, skip everything
      {
        int thread_num = omp_get_thread_num();
        init_memory();                               // Init memory if this is the first run through.
        if (needs_recalculating[thread_num])         // Do the actual calculation if required.
        {
          logger().entering_module(myLogTag);
          this->startTiming(thread_num);             //Begin timing function evaluation
          try
          {
            this->myFunction(myValue[thread_num]);   //Run and place result in the appropriate slot in myValue
          }
          catch (invalid_point_exception& e)
          {
            if (not point_exception_raised) acknowledgeInvalidation(e);
            if (omp_get_level()==0)                  // If not in an OpenMP parallel block, throw onwards
            {
              this->finishTiming(thread_num);        //Stop timing function evaluation
              throw(e);
            } 
          }
          this->finishTiming(thread_num);            //Stop timing function evaluation
          logger().leaving_module();
        }
        check_for_shutdown_signal();
      }
      else
      {
        logger() << "Shutdown in progress! Skipping evaluation of functor " << myName << EOM;
      }
    }

    /// Initialise the memory of this functor.
    template <typename TYPE>
    void module_functor<TYPE>::init_memory()
    {
      this->module_functor_common::init_memory();
      if(myValue==NULL)
      {
        #pragma omp critical(module_functor_init_memory)
        {
          // Reserve enough space to hold as many results as there are slots (threads) allowed
          if(myValue==NULL) myValue = new TYPE[(iRunNested ? globlMaxThreads : 1)];
        }
      }
    }

    /// Operation (return value)
    template <typename TYPE>
    TYPE module_functor<TYPE>::operator()(int index)
    {
      if (this == NULL) functor::failBigTime("operator()");
      init_memory(); // Init memory if this is the first run through.
      return (iRunNested ? myValue[index] : myValue[0]);
    }

    /// Alternative to operation (returns a safe pointer to value)
    template <typename TYPE>
    safe_ptr<TYPE> module_functor<TYPE>::valuePtr()
    {
      if (this == NULL) functor::failBigTime("valuePtr");
      init_memory(); // Init memory if this is the first run through.
      return safe_ptr<TYPE>(myValue);
    }

    /// Printer function
    template <typename TYPE>
    void module_functor<TYPE>::print(Printers::BasePrinter* printer, const int pointID, int thread_num)
    {
      if(not signaldata().shutdown_begun()) // Don't print anything if we are shutting down,
      {                                     // since this calculation has been interrupted.
        // Only try to print if print flag set to true, and if this functor(+thread) hasn't already been printed
        // TODO: though actually the printer system will probably cark it if printing from multiple threads is
        // attempted, because it uses the VertexID to differentiate print streams, and this is shared among threads.
        // Can fix by requiring a VertexID+thread_num pair, but I am leaving this for later.
        init_memory();                 // Init memory if this is the first run through.
        if(myPrintFlag and not already_printed[thread_num] and type()!="void") // myPrintFlag should anyway not be true for void result types
        {
          if (not iRunNested) thread_num = 0; // Force printing of thread_num=0 if this functor cannot run nested.
          int rank = printer->getRank();      // This is "first pass" printing, so use the actual rank of this process.
                                              // In the auxilliary printing system we may tell the printer to overwrite
                                              // the output of other ranks.
          printer->print(myValue[thread_num],myLabel,myVertexID,rank,pointID);
          already_printed[thread_num] = true;
        }

<<<<<<< HEAD
        // Print timing info if requested (independent of whether printing actual result)
        if(myTimingPrintFlag and not already_printed_timing[thread_num])
        {
          if (not iRunNested) thread_num = 0; // Force printing of thread_num=0 if this functor cannot run nested.
          int rank = printer->getRank();
          std::chrono::duration<double> runtime = end[thread_num] - start[thread_num]; 
          printer->print(runtime.count(),myTimingLabel,myTimingVertexID,rank,pointID);
          already_printed_timing[thread_num] = true;
        }
=======
      // Print timing info if requested (independent of whether printing actual result)
      if(myTimingPrintFlag and not already_printed_timing[thread_num])
      {
        if (not iRunNested) thread_num = 0; // Force printing of thread_num=0 if this functor cannot run nested.
        int rank = printer->getRank();
        std::chrono::duration<double> runtime = end[thread_num] - start[thread_num];
        printer->print(runtime.count(),myTimingLabel,myTimingVertexID,rank,pointID);
        already_printed_timing[thread_num] = true;
>>>>>>> master
      }
    }

    /// Printer function (no-thread-index short-circuit)
    template <typename TYPE>
    void module_functor<TYPE>::print(Printers::BasePrinter* printer, const int pointID) { print(printer,pointID,0); }

  /// Class methods for actual module functors for TYPE=void

    /// Constructor
    module_functor<void>::module_functor(void (*inputFunction)(),
                                         str func_name,
                                         str func_capability,
                                         str result_type,
                                         str origin_name,
                                         Models::ModelFunctorClaw &claw)
    : module_functor_common(func_name, func_capability, result_type, origin_name, claw),
      myFunction (inputFunction) {}

    /// Calculate method
    /// The "void" specialisation can potentially manage loops,
    /// so there are some extra switches in here to let the signal
    /// handler know that it needs to run in threadsafe mode during
    /// execution of this functor.
    void module_functor<void>::calculate()
    {
      if(not signaldata().shutdown_begun())          // If shutdown signal has been received, skip everything
      {
        int thread_num = omp_get_thread_num();
        init_memory();                               // Init memory if this is the first run through.
        if (needs_recalculating[thread_num])
        {
          entering_multithreaded_region();

          logger().entering_module(myLogTag);
          this->startTiming(thread_num);
          try
          {
<<<<<<< HEAD
            this->myFunction();
          }
          catch (invalid_point_exception& e)
          {
            if (not point_exception_raised) acknowledgeInvalidation(e);
            if (omp_get_level()==0)                  // If not in an OpenMP parallel block, throw onwards
            {
              this->finishTiming(thread_num);
              leaving_multithreaded_region();
              throw(e);
            } 
          }
          this->finishTiming(thread_num);
          logger().leaving_module();
         
          leaving_multithreaded_region();
=======
            this->finishTiming(thread_num);
            throw(e);
          }
>>>>>>> master
        }
        check_for_shutdown_signal();
      }
      else
      {
        logger() << "Shutdown in progress! Skipping evaluation of functor " << myName << EOM;
      }
    }

    /// Blank print methods
    void module_functor<void>::print(Printers::BasePrinter*, const int, int) {}
    void module_functor<void>::print(Printers::BasePrinter*, const int) {}


  // Backend_functor_common class method definitions

    /// Constructor
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    backend_functor_common<PTR_TYPE, TYPE, ARGS...>::backend_functor_common (funcPtrType inputFunction,
                                                                             str func_name,
                                                                             str func_capability,
                                                                             str result_type,
                                                                             str origin_name,
                                                                             str origin_version,
                                                                             str origin_safe_version,
                                                                             Models::ModelFunctorClaw &claw)
    : functor (func_name, func_capability, result_type, origin_name, claw),
      myFunction (inputFunction),
      myLogTag(-1),
      inUse(false)
    {
      myVersion = origin_version;
      mySafeVersion = origin_safe_version;
      // Determine LogTag number
      myLogTag = Logging::str2tag(myOrigin);
      // Or in the case where we prefer to include the version number in the LogTag too
      //myLogTag = Logging::str2tag(myOrigin+"v"+myVersion);
      // Check for failure
      if(myLogTag==-1)
      {
        std::ostringstream ss;
        ss << "Error retrieving LogTag number (in functors.hpp, constructor for "
              "backend_functor_common)! No match for backend name in tag2str map! "
              "This is supposed to be a backend functor, so this is a fatal error. "
              "(myOrigin=" << myOrigin << ", myName=" << myName << ")";
        utils_error().raise(LOCAL_INFO,ss.str());
      }
    }

    /// Update the internal function pointer wrapped by the functor
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    void backend_functor_common<PTR_TYPE, TYPE, ARGS...>::updatePointer(funcPtrType inputFunction)
    {
      myFunction = inputFunction;
    }

    /// Hand out the internal function pointer wrapped by the functor
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    typename backend_functor_common<PTR_TYPE, TYPE, ARGS...>::funcPtrType backend_functor_common<PTR_TYPE, TYPE, ARGS...>::handoutFunctionPointer()
    {
      return myFunction;
    }

    /// Getter for the 'safe' incarnation of the wrapped function's origin's version (module or backend)
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    str backend_functor_common<PTR_TYPE, TYPE, ARGS...>::safe_version() const { if (this == NULL) failBigTime("safe_version"); return mySafeVersion; }

    /// Set the inUse flag.
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    void backend_functor_common<PTR_TYPE, TYPE, ARGS...>::setInUse(bool flag) { inUse = flag; }

    /// Hand out a safe pointer to this backend functor's inUse flag.
    template <typename PTR_TYPE, typename TYPE, typename... ARGS>
    safe_ptr<bool> backend_functor_common<PTR_TYPE, TYPE, ARGS...>::inUsePtr()
    {
      if (this == NULL) functor::failBigTime("inUsePtr");
      return safe_ptr<bool>(&inUse);
    }


    // Actual non-variadic backend functor class method definitions for TYPE != void

    /// Constructor
    template <typename TYPE, typename... ARGS>
    backend_functor<TYPE(*)(ARGS...), TYPE, ARGS...>::backend_functor (TYPE (*inputFunction)(ARGS...),
                                                                       str func_name,
                                                                       str func_capability,
                                                                       str result_type,
                                                                       str origin_name,
                                                                       str origin_version,
                                                                       str safe_version,
                                                                       Models::ModelFunctorClaw &claw)
    : backend_functor_common<TYPE(*)(ARGS...), TYPE, ARGS...>(inputFunction, func_name,
      func_capability, result_type, origin_name, origin_version, safe_version, claw) {}

    /// Operation (execute function and return value)
    template <typename TYPE, typename... ARGS>
    TYPE backend_functor<TYPE(*)(ARGS...), TYPE, ARGS...>::operator()(ARGS&&... args)
    {
      if (this == NULL) functor::failBigTime("operator()");
      logger().entering_backend(this->myLogTag);
      TYPE tmp = this->myFunction(std::forward<ARGS>(args)...);
      logger().leaving_backend();
      return tmp;
    }


    // Actual non-variadic backend functor class method definitions for TYPE=void

    /// Constructor
    template <typename... ARGS>
    backend_functor<void(*)(ARGS...), void, ARGS...>::backend_functor (void (*inputFunction)(ARGS...),
                                                                       str func_name,
                                                                       str func_capability,
                                                                       str result_type,
                                                                       str origin_name,
                                                                       str origin_version,
                                                                       str safe_version,
                                                                       Models::ModelFunctorClaw &claw)
    : backend_functor_common<void(*)(ARGS...), void, ARGS...>(inputFunction, func_name,
      func_capability, result_type, origin_name, origin_version, safe_version, claw) {}

    /// Operation (execute function and return value)
    template <typename... ARGS>
    void backend_functor<void(*)(ARGS...), void, ARGS...>::operator()(ARGS&&... args)
    {
      if (this == NULL) functor::functor::failBigTime("operator()");
      logger().entering_backend(this->myLogTag);
      this->myFunction(std::forward<ARGS>(args)...);
      logger().leaving_backend();
    }

    // Actual variadic backend functor class method definitions for TYPE != void

    /// Constructor
    template <typename TYPE, typename... ARGS>
    backend_functor<typename variadic_ptr<TYPE,ARGS...>::type, TYPE, ARGS...>::backend_functor
     (typename variadic_ptr<TYPE,ARGS...>::type inputFunction, str func_name, str func_capability, str result_type,
     str origin_name, str origin_version, str safe_version, Models::ModelFunctorClaw &claw)
    : backend_functor_common<typename variadic_ptr<TYPE,ARGS...>::type, TYPE, ARGS...>(inputFunction, func_name,
      func_capability, result_type, origin_name, origin_version, safe_version, claw) {}

    // Actual variadic backend functor class method definitions for TYPE=void

    /// Constructor
    template <typename... ARGS>
    backend_functor<typename variadic_ptr<void,ARGS...>::type, void, ARGS...>::backend_functor
     (typename variadic_ptr<void,ARGS...>::type inputFunction, str func_name, str func_capability, str result_type,
     str origin_name, str origin_version, str safe_version, Models::ModelFunctorClaw &claw)
    : backend_functor_common<typename variadic_ptr<void,ARGS...>::type, void, ARGS...>(inputFunction, func_name,
      func_capability, result_type, origin_name, origin_version, safe_version, claw) {}

}

/// Instantiate a module functor template for a specific type
#define INSTANTIATE_MODULE_FUNCTOR_TEMPLATE(r,x,TYPE)             \
namespace Gambit { template class module_functor<TYPE>; }

/// Instantiate a backend functor template for a specific type
#define INSTANTIATE_BACKEND_FUNCTOR_TEMPLATE(r,x,TYPE_PACK)       \
namespace Gambit                                                  \
{                                                                 \
  template class backend_functor_common<STRIP_PARENS(TYPE_PACK)>; \
  template class backend_functor<STRIP_PARENS(TYPE_PACK)>;        \
}

