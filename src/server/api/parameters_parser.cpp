#include "server/api/parameters_parser.hpp"

#include "server/api/isochrone_parameter_grammar.hpp"
#include "server/api/isodistance_parameter_grammar.hpp"

#include <type_traits>

namespace osrm
{
namespace server
{
namespace api
{

namespace detail
{
template <typename T>
using is_grammar_t =
    std::integral_constant<bool,
                           std::is_same<IsochroneParametersGrammar<>, T>::value ||
                               std::is_same<IsodistanceParametersGrammar<>, T>::value>;

template <typename ParameterT,
          typename GrammarT,
          typename std::enable_if<detail::is_parameter_t<ParameterT>::value, int>::type = 0,
          typename std::enable_if<detail::is_grammar_t<GrammarT>::value, int>::type = 0>
boost::optional<ParameterT> parseParameters(std::string::iterator &iter,
                                            const std::string::iterator end)
{
    using It = std::decay<decltype(iter)>::type;

    static const GrammarT grammar;

    try
    {
        ParameterT parameters;
        const auto ok =
            boost::spirit::qi::parse(iter, end, grammar(boost::phoenix::ref(parameters)));

        // return move(a.b) is needed to move b out of a and then return the rvalue by implicit move
        if (ok && iter == end)
            return std::move(parameters);
    }
    catch (const qi::expectation_failure<It> &failure)
    {
        // The grammar above using expectation parsers ">" does not automatically increment the
        // iterator to the failing position. Extract the position from the exception ourselves.
        iter = failure.first;
    }

    return boost::none;
}
} // ns detail

template <>
boost::optional<engine::api::IsochroneParameters> parseParameters(std::string::iterator &iter,
                                                                  const std::string::iterator end)
{
    return detail::parseParameters<engine::api::IsochroneParameters, IsochroneParametersGrammar<>>(
        iter, end);
}
template <>
boost::optional<engine::api::IsodistanceParameters> parseParameters(std::string::iterator &iter,
                                                  const std::string::iterator end)
{
    return detail::parseParameters<engine::api::IsodistanceParameters, IsodistanceParametersGrammar<>>(
        iter, end);
}

} // ns api
} // ns server
} // ns osrm
