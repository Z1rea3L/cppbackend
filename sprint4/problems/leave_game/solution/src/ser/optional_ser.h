/**
 * @file std_optional_serialization.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Realization for std::optional as <boost/serialization/optional.hpp>
 * @version 0.1
 * @date 2023-01-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __STD_OPTIONAL_SERIALIZATION_H__
#define __STD_OPTIONAL_SERIALIZATION_H__
/**/
#include <boost/serialization/split_free.hpp>

#include <optional>

namespace boost::serialization {

template <typename Archive, typename T>
void save(Archive& ar, const std::optional<T>& opt, unsigned int version) {
  const bool has_value = opt.has_value();
  ar << opt.has_value();
  if (has_value) {
    ar << *opt;
  }
}

template <typename Archive, typename T>
void load(Archive& ar, std::optional<T>& opt, unsigned int version) {
  bool has_value{false};
  ar >> has_value;
  if (has_value) {
    opt = T();
    ar >> *opt;
  } else {
    opt.reset();
  }
}

template <class Archive, class T>
void serialize(Archive& ar, std::optional<T>& t, const unsigned int version) {
  boost::serialization::split_free(ar, t, version);
}

}  // namespace boost::serialization
/**/

/** /
#include <boost/config.hpp>

#include <optional>

#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/library_version_type.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/serialization/detail/is_default_constructible.hpp>

#include <boost/serialization/optional.hpp>
// function specializations must be defined in the appropriate
// namespace - boost::serialization
namespace boost {
namespace serialization {

template<class Archive, class T>
void save(
    Archive & ar,
    const std::optional< T > & t,
    const unsigned int
){
    const bool tflag = t.has_value();
    ar << boost::serialization::make_nvp("initialized", tflag);
    if (tflag){
        ar << boost::serialization::make_nvp("value", *t);
    }
}

template<class Archive, class T>
void load(
    Archive & ar,
    std::optional< T > & t,
    const unsigned int version
){
    bool tflag;
    ar >> boost::serialization::make_nvp("initialized", tflag);
    if(! tflag){
        t.reset();
        return;
    }

    if(0 == version){
        boost::serialization::item_version_type item_version(0);
        boost::serialization::library_version_type library_version(
            ar.get_library_version()
        );
        if(boost::serialization::library_version_type(3) < library_version){
            ar >> BOOST_SERIALIZATION_NVP(item_version);
        }
    }
    if(! t.has_value())
        t = T();
    ar >> boost::serialization::make_nvp("value", *t);
}

template<class Archive, class T>
void serialize(
    Archive & ar,
    std::optional< T > & t,
    const unsigned int version
){
    boost::serialization::split_free(ar, t, version);
}

template<class T>
struct version<std::optional<T> > {
    BOOST_STATIC_CONSTANT(int, value = 1);
};


} // serialization
} // boost

/**/

#endif  // __STD_OPTIONAL_SERIALIZATION_H__