/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#ifndef  METAOBJECTBUILDER_HPP_
# define METAOBJECTBUILDER_HPP_

#include <qimessaging/api.hpp>
#include <string>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>

#include <boost/function.hpp>
#include <qimessaging/signature.hpp>
#include <qimessaging/metafunction.hpp>
#include <sstream>

namespace qi {

  class MetaObject;
  class MetaObjectBuilderPrivate;
  class QIMESSAGING_API MetaObjectBuilder {
  public:
    MetaObjectBuilder(qi::MetaObject *metaObject);
    ~MetaObjectBuilder();

    template <typename OBJECT_TYPE, typename METHOD_TYPE>
    inline unsigned int advertiseMethod(const std::string& name, OBJECT_TYPE object, METHOD_TYPE method);
    template <typename FUNCTION_TYPE>
    inline unsigned int advertiseMethod(const std::string& name, FUNCTION_TYPE function);
    template<typename T>
    inline unsigned int advertiseMethod(const std::string& name, boost::function<T> func);

    int xAdvertiseMethod(const std::string &retsig, const std::string& signature, qi::MetaFunction func);
    int xForgetMethod(const std::string &meth);

    template<typename FUNCTION_TYPE>
    inline unsigned int advertiseEvent(const std::string& eventName);
    int xAdvertiseEvent(const std::string& signature);


  public:
    MetaObjectBuilderPrivate *_p;
    QI_DISALLOW_COPY_AND_ASSIGN(MetaObjectBuilder);
  };


  namespace detail {
    struct signature_function_arg_apply {
      signature_function_arg_apply(std::ostream* val)
        : val(*val)
      {}

      template<typename T> void operator()(T *x) {
        val << metaTypeOf<T>()->signature();
      }

      std::ostream &val;
    };
  }


  template <typename FUNCTION_TYPE>
  inline unsigned int MetaObjectBuilder::advertiseMethod(const std::string& name, FUNCTION_TYPE function)
  {
    std::stringstream   signature;

    std::string         sigret;

    signature << name << "::(";
    typedef typename boost::function_types::parameter_types<FUNCTION_TYPE>::type ArgsType;
    boost::mpl::for_each<
      boost::mpl::transform_view<ArgsType,
        boost::add_pointer<
        boost::remove_const<
        boost::remove_reference<boost::mpl::_1> > > > > (qi::detail::signature_function_arg_apply(&signature));

    signature << ")";

    typedef typename boost::function_types::result_type<FUNCTION_TYPE>::type     ResultType;
    sigret = metaTypeOf<ResultType>()->signature();

    return xAdvertiseMethod(sigret, signature.str(), makeFunctor(function));
  }

  template<typename T>
  inline unsigned int MetaObjectBuilder::advertiseMethod(const std::string& name,
    boost::function<T> function)
  {
    std::stringstream   signature;
    std::string         sigret;
    signature << name << "::(";
    typedef typename boost::function_types::parameter_types<T>::type ArgsType;
    boost::mpl::for_each<
      boost::mpl::transform_view<ArgsType,
        boost::add_pointer<
        boost::remove_const<
        boost::remove_reference<boost::mpl::_1> > > > > (qi::detail::signature_function_arg_apply(&signature));

    signature << ")";

    typedef typename boost::function_types::result_type<T>::type ResultType;
    sigret = metaTypeOf<ResultType>()->signature();

    return xAdvertiseMethod(sigret, signature.str(), makeFunctor(function));
  }

  template<typename FUNCTION_TYPE>
  inline unsigned int MetaObjectBuilder::advertiseEvent(const std::string& eventName)
  {
    std::stringstream   signature;
    signature << eventName << "::(";
    typedef typename boost::function_types::parameter_types<FUNCTION_TYPE>::type ArgsType;
    boost::mpl::for_each<
      boost::mpl::transform_view<ArgsType,
        boost::add_pointer<
        boost::remove_const<
        boost::remove_reference<boost::mpl::_1> > > > > (qi::detail::signature_function_arg_apply(&signature));

    signature << ")";

    return xAdvertiseEvent(signature.str());
  }

  template <typename OBJECT_TYPE, typename METHOD_TYPE>
  inline unsigned int MetaObjectBuilder::advertiseMethod(const std::string& name, OBJECT_TYPE object, METHOD_TYPE method)
  {
    std::stringstream   signature;
    std::string         sigret;
    signature << name << "::(";
    typedef typename boost::function_types::parameter_types<METHOD_TYPE>::type MemArgsType;
    typedef typename boost::mpl::pop_front< MemArgsType >::type                ArgsType;

    boost::mpl::for_each<
      boost::mpl::transform_view<ArgsType,
        boost::add_pointer<
        boost::remove_const<
        boost::remove_reference<boost::mpl::_1> > > > > (qi::detail::signature_function_arg_apply(&signature));
    signature << ")";

    typedef typename boost::function_types::result_type<METHOD_TYPE>::type     ResultType;

    sigret = metaTypeOf<ResultType>()->signature();

    return xAdvertiseMethod(sigret, signature.str(), makeFunctor(object, method));
  }

}

#endif /* !METAOBJECTBUILDER_PP_ */
