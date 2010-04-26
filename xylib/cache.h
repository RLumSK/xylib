// Public API of xylib library.
// Licence: Lesser GNU Public License 2.1 (LGPL)
// $Id: xylib.h 462 2009-04-15 23:54:27Z wojdyr $

/// This header is new in 0.4 and may be changed in future.
/// Support for caching files read by xylib.
/// Usage is similar to load_file() from xylib.h:
///  shared_ptr<const xylib::DataSet> my_dataset = xylib::cached_load_file(...);
/// or
///  xylib::DataSet const& my_dataset = xylib::Cache::Get()->load_file(...);

#ifndef XYLIB_CACHE_H_
#define XYLIB_CACHE_H_

#ifndef __cplusplus
#error "xylib/cache.h is a C++ only header."
#endif

#include <ctime>
#include <string>
#include <vector>

// XYLIB_API is a mark for API classes and functions,
// used to decorate classes and functions for Win32 DLL linking.
#ifdef XYLIB_API
# undef XYLIB_API
#endif
#if defined(_WIN32) && defined(BUILDING_XYLIB) && \
        (defined(XYLIB_DLL) || defined(DLL_EXPORT))
# define XYLIB_API  __declspec(dllexport)
#elif defined(_WIN32) && defined(XYLIB_DLL)
# define XYLIB_API  __declspec(dllimport)
#else
# define XYLIB_API
#endif

// MSVC has shared_ptr in <memory>, but let's use boost
#ifndef _MSC_VER
// the value here (1 or 0) is set by xylib configure script
// (yes, the configure script can modify this file!)
#define XYLIB_USE_TR1_MEMORY 1
#endif

#if XYLIB_USE_TR1_MEMORY
# include <tr1/memory>
  using std::tr1::shared_ptr;
#else
# include <boost/shared_ptr.hpp>
  using boost::shared_ptr;
#endif

namespace xylib
{


class DataSet;

// singleton
class XYLIB_API Cache
{
public:
    // get instance
    static Cache* Get()
        { if (instance_ == NULL) instance_ = new Cache(); return instance_; }

    // Arguments are the same as in load_file() in xylib.h,
    // but a const ref is returned instead of pointer.
    shared_ptr<const DataSet> load_file(std::string const& path,
                                        std::string const& format_name="",
                                        std::vector<std::string> const& options
                                                = std::vector<std::string>());

    // set max. number of cached files, default=1
    void set_number_of_cached_files(size_t n);
    // get max. number of cached files
    inline size_t get_number_of_cached_files() const { return n_cached_files_; }

    // clear cache
    void clear_cache() { cache_.clear(); }

private:
    static Cache *instance_; // for singleton pattern

    struct CachedFile
    {
        std::string path_;
        std::string format_name_;
        std::vector<std::string> options_;
        std::time_t read_time_;
        shared_ptr<const DataSet> dataset_;

        CachedFile(std::string const& path,
                   std::string const& format_name,
                   std::vector<std::string> const& options,
                   shared_ptr<const DataSet> dataset)
            : path_(path), format_name_(format_name), options_(options),
              read_time_(std::time(NULL)), dataset_(dataset) {}
    };

    size_t n_cached_files_;
    std::vector<CachedFile> cache_;

    Cache() : n_cached_files_(1) {}
};

inline
shared_ptr<const DataSet> cached_load_file(std::string const& path,
                                        std::string const& format_name="",
                                        std::vector<std::string> const& options
                                                = std::vector<std::string>())
{
    return Cache::Get()->load_file(path, format_name, options);
}

// format is passed as the first option
inline
shared_ptr<const DataSet> cached_load_file(std::string const& path,
                                      std::vector<std::string> const& options)
{
    return options.empty() ? cached_load_file(path)
                           : cached_load_file(path, options[0],
                                   std::vector<std::string>(options.begin()+1,
                                                            options.end()));
}

} // namespace xylib

#endif // XYLIB_CACHE_H_
