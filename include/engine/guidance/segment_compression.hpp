#ifndef ENGINE_GUIDANCE_PROCESSING_SEGMENT_COMPRESSION_HPP_
#define ENGINE_GUIDANCE_PROCESSING_SEGMENT_COMPRESSION_HPP_

#include "engine/segment_information.hpp"
#include "extractor/turn_instructions.hpp"

#include <string>
#include <vector>

namespace osrm
{
namespace engine
{
namespace guidance
{

/*
  Simplify turn instructions
  Input :
    10. Turn left on B 36 for 20 km
    11. Continue on B 35; B 36 for 2 km
    12. Continue on B 36 for 13 km

  Output:
   10. Turn left on B 36 for 35 km
*/

template <typename DataFacadeT>
inline bool Omittable(const SegmentInformation &first,
                      const SegmentInformation &second,
                      const DataFacadeT *const facade)
{
    const std::string& first_name = facade->GetNameForId(first.name_id);
    const std::string& second_name = facade->GetNameForId(second.name_id);

    std::cout << "First: " << first_name
              << " Second: " << second_name << std::endl;
    return( first.travel_mode == second.travel_mode && first_name.size() > 1 && second_name.size() > 1 && ( first_name.find( second_name ) != std::string::npos
      || second_name.find( first_name ) != std::string::npos ));
}

template <typename DataFacadeT>
inline void CombineSimilarSegments(std::vector<SegmentInformation> &segments,
                                   const DataFacadeT *const facade)
{
    if( segments.empty() )
      return;

    std::stack<std::size_t> indices;
    indices.push(0);
    for (std::size_t i = 1; i < segments.size(); ++i){
      if( segments[i].turn_instruction == extractor::TurnInstruction::GoStraight ){
        if( Omittable( segments[indices.top()], segments[i], facade ) ){
            segments[i].turn_instruction =
                extractor::TurnInstruction::NoTurn; // TODO decide on combineable types
            const std::string &first = facade->GetNameForId(segments[indices.top()].name_id);
            const std::string &second = facade->GetNameForId(segments[i].name_id);
            std::cout << "Omitting " << second << std::endl;
            if( second.size() < first.size() ){
              std::cout << "Setting " << first << " to " << second << std::endl;
              segments[indices.top()].name_id = segments[i].name_id;
            }
        } else {
          indices.push(i);
        }
      }
    }
    // TODO: rework to check only end and start of string.
    //      stl string is way to expensive
    //    unsigned lastTurn = 0;
    //    for(unsigned i = 1; i < path_description.size(); ++i) {
    //        string1 = sEngine.GetEscapedNameForNameID(path_description[i].name_id);
    //        if(TurnInstruction::GoStraight == path_description[i].turn_instruction) {
    //            if(std::string::npos != string0.find(string1+";")
    //                  || std::string::npos != string0.find(";"+string1)
    //                  || std::string::npos != string0.find(string1+" ;")
    //                    || std::string::npos != string0.find("; "+string1)
    //                    ){
    //                SimpleLogger().Write() << "->next correct: " << string0 << " contains " <<
    //                string1;
    //                for(; lastTurn != i; ++lastTurn)
    //                    path_description[lastTurn].name_id = path_description[i].name_id;
    //                path_description[i].turn_instruction = TurnInstruction::NoTurn;
    //            } else if(std::string::npos != string1.find(string0+";")
    //                  || std::string::npos != string1.find(";"+string0)
    //                    || std::string::npos != string1.find(string0+" ;")
    //                    || std::string::npos != string1.find("; "+string0)
    //                    ){
    //                SimpleLogger().Write() << "->prev correct: " << string1 << " contains " <<
    //                string0;
    //                path_description[i].name_id = path_description[i-1].name_id;
    //                path_description[i].turn_instruction = TurnInstruction::NoTurn;
    //            }
    //        }
    //        if (TurnInstruction::NoTurn != path_description[i].turn_instruction) {
    //            lastTurn = i;
    //        }
    //        string0 = string1;
    //    }
    //
}
} // namespace guidance
} // namespace engine
} // namespace osrm

#endif // ENGINE_GUIDANCE_PROCESSING_SEGMENT_COMPRESSION_HPP_
