#ifndef OSRM_ISODISTANCE_PARAMETER_GRAMMAR_HPP
#define OSRM_ISODISTANCE_PARAMETER_GRAMMAR_HPP

#include "server/api/base_parameters_grammar.hpp"
#include "engine/api/isodistance_parameters.hpp"

#include <boost/spirit/include/qi.hpp>

namespace osrm
{
namespace server
{
namespace api
{

namespace
{
namespace ph = boost::phoenix;
namespace qi = boost::spirit::qi;
}

template <typename Iterator = std::string::iterator,
          typename Signature = void(engine::api::IsodistanceParameters &)>
struct IsodistanceParametersGrammar final : public BaseParametersGrammar<Iterator, Signature>
{
    using BaseGrammar = BaseParametersGrammar<Iterator, Signature>;

    IsodistanceParametersGrammar() : BaseGrammar(root_rule)
    {

        distance_rule =
            (qi::lit("distance=") >
             qi::double_)[ph::bind(&engine::api::IsodistanceParameters::distance, qi::_r1) = qi::_1];
        convexhull_rule =
            (qi::lit("convexhull=") >
             qi::bool_)[ph::bind(&engine::api::IsodistanceParameters::convexhull, qi::_r1) = qi::_1];
        concavehull_rule =
            (qi::lit("concavehull=") >
             qi::bool_)[ph::bind(&engine::api::IsodistanceParameters::concavehull, qi::_r1) = qi::_1];
        threshold_rule =
            (qi::lit("threshold=") >
             qi::double_)[ph::bind(&engine::api::IsodistanceParameters::threshold, qi::_r1) = qi::_1];

        root_rule =
            BaseGrammar::query_rule(qi::_r1) > -qi::lit(".json") >
            -('?' > (distance_rule(qi::_r1) | convexhull_rule(qi::_r1) |
                     concavehull_rule(qi::_r1) | threshold_rule(qi::_r1)) %
                        '&');
    }

  private:
    qi::rule<Iterator, Signature> root_rule;
    qi::rule<Iterator, Signature> distance_rule;
    qi::rule<Iterator, Signature> convexhull_rule;
    qi::rule<Iterator, Signature> concavehull_rule;
    qi::rule<Iterator, Signature> threshold_rule;
};
}
}
}

#endif // OSRM_ISODISTANCE_PARAMETER_GRAMMAR_HPP
