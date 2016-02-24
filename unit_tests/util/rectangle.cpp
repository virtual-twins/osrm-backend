#include "util/rectangle.hpp"
#include "util/typedefs.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

BOOST_AUTO_TEST_SUITE(rectangle_test)

using namespace osrm;
using namespace osrm::util;

// Verify that the bearing-bounds checking function behaves as expected
BOOST_AUTO_TEST_CASE(get_min_dist_test)
{
    //           ^
    //           |
    //           +- 80
    //           |
    //           |
    //           +- 10
    //           |
    //--|-----|--+--|-----|-->
    // -100  -10 |  10    100
    //           +- -10
    //           |
    //           |
    //           +- -80
    //           |
    RectangleInt2D nw {FloatLongitude(10), FloatLongitude(100), FloatLatitude(10), FloatLatitude(80)};
    //RectangleInt2D ne {FloatLongitude(-100), FloatLongitude(-10), FloatLatitude(10), FloatLatitude(80)};
    //RectangleInt2D sw {FloatLongitude(10), FloatLongitude(100), FloatLatitude(-80), FloatLatitude(-10)};
    RectangleInt2D se {FloatLongitude(-100), FloatLongitude(-10), FloatLatitude(-80), FloatLatitude(-10)};

    Coordinate nw_sw {FloatLongitude(9.9), FloatLatitude(9.9)};
    Coordinate nw_se {FloatLongitude(100.1), FloatLatitude(9.9)};
    Coordinate nw_ne {FloatLongitude(100.1), FloatLatitude(80.1)};
    Coordinate nw_nw {FloatLongitude(9.9), FloatLatitude(80.1)};
    Coordinate nw_s {FloatLongitude(55), FloatLatitude(9.9)};
    Coordinate nw_e {FloatLongitude(100.1), FloatLatitude(45.0)};
    Coordinate nw_w {FloatLongitude(9.9), FloatLatitude(45.0)};
    Coordinate nw_n {FloatLongitude(55), FloatLatitude(80.1)};
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_sw),15611.9, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_se),15611.9, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_ne),11287.4, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_nw),11287.4, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_s), 11122.6, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_e), 7864.89, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_w), 7864.89, 0.1);
    BOOST_CHECK_CLOSE(nw.GetMinDist(nw_n), 11122.6, 0.1);

    Coordinate se_ne {FloatLongitude(-9.9), FloatLatitude(-9.9)};
    Coordinate se_nw {FloatLongitude(-100.1), FloatLatitude(-9.9)};
    Coordinate se_sw {FloatLongitude(-100.1), FloatLatitude(-80.1)};
    Coordinate se_se {FloatLongitude(-9.9), FloatLatitude(-80.1)};
    Coordinate se_n  {FloatLongitude(-55), FloatLatitude(-9.9)};
    Coordinate se_w  {FloatLongitude(-100.1), FloatLatitude(-45.0)};
    Coordinate se_e  {FloatLongitude(-9.9), FloatLatitude(-45.0)};
    Coordinate se_s  {FloatLongitude(-55), FloatLatitude(-80.1)};
    BOOST_CHECK_CLOSE(se.GetMinDist(se_sw),11287.4, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_se),11287.4, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_ne),15611.9, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_nw),15611.9, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_s), 11122.6, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_e), 7864.89, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_w), 7864.89, 0.1);
    BOOST_CHECK_CLOSE(se.GetMinDist(se_n), 11122.6, 0.1);
}

BOOST_AUTO_TEST_CASE(bb_distance_test)
{
    RectangleInt2D rect(FloatLongitude(-124.217428),FloatLongitude(-0.68555),FloatLatitude(42.22385),FloatLatitude(71.747749));

    Coordinate segment_start {FloatLongitude(-124.217428), FloatLatitude(42.22385)};
    Coordinate segment_end {FloatLongitude(-0.68555), FloatLatitude(71.747749)};

    Coordinate query {FloatLongitude{76.4239}, FloatLatitude{70.2184}};
    auto rect_dist = rect.GetMinDist(query);
    BOOST_CHECK(rect.Contains(segment_start));
    BOOST_CHECK(rect.Contains(segment_end));
    BOOST_CHECK_LE(rect_dist, coordinate_calculation::perpendicularDistance(segment_start, segment_end, query));
    BOOST_CHECK_LE(rect_dist, coordinate_calculation::euclideanDistance(segment_start, query));
    BOOST_CHECK_LE(rect_dist, coordinate_calculation::euclideanDistance(segment_end, query));
}

void TestRectangle(double width, double height, double center_lat, double center_lon)
{
    Coordinate center{FloatLongitude(center_lon), FloatLatitude(center_lat)};

    RectangleInt2D rect;
    rect.min_lat = center.lat - FixedLatitude(height / 2.0 * COORDINATE_PRECISION);
    rect.max_lat = center.lat + FixedLatitude(height / 2.0 * COORDINATE_PRECISION);
    rect.min_lon = center.lon - FixedLongitude(width / 2.0 * COORDINATE_PRECISION);
    rect.max_lon = center.lon + FixedLongitude(width / 2.0 * COORDINATE_PRECISION);

    const FixedLongitude lon_offset(5. * COORDINATE_PRECISION);
    const FixedLatitude lat_offset(5. * COORDINATE_PRECISION);
    Coordinate north(center.lon, rect.max_lat + lat_offset);
    Coordinate south(center.lon, rect.min_lat - lat_offset);
    Coordinate west(rect.min_lon - lon_offset, center.lat);
    Coordinate east(rect.max_lon + lon_offset, center.lat);
    Coordinate north_east(rect.max_lon + lon_offset, rect.max_lat + lat_offset);
    Coordinate north_west(rect.min_lon - lon_offset, rect.max_lat + lat_offset);
    Coordinate south_east(rect.max_lon + lon_offset, rect.min_lat - lat_offset);
    Coordinate south_west(rect.min_lon - lon_offset, rect.min_lat - lat_offset);

    /* Distance to line segments of rectangle */
    BOOST_CHECK_EQUAL(rect.GetMinDist(north), coordinate_calculation::euclideanDistance(north, Coordinate(north.lon, rect.max_lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(south), coordinate_calculation::euclideanDistance(south, Coordinate(south.lon, rect.min_lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(west), coordinate_calculation::euclideanDistance(west,   Coordinate(rect.min_lon, west.lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(east), coordinate_calculation::euclideanDistance(east,   Coordinate(rect.max_lon, east.lat)));

    /* Distance to corner points */
    BOOST_CHECK_EQUAL(rect.GetMinDist(north_east), coordinate_calculation::euclideanDistance(north_east, Coordinate(rect.max_lon, rect.max_lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(north_west), coordinate_calculation::euclideanDistance(north_west, Coordinate(rect.min_lon, rect.max_lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(south_east), coordinate_calculation::euclideanDistance(south_east, Coordinate(rect.max_lon, rect.min_lat)));
    BOOST_CHECK_EQUAL(rect.GetMinDist(south_west), coordinate_calculation::euclideanDistance(south_west, Coordinate(rect.min_lon, rect.min_lat)));
}

BOOST_AUTO_TEST_CASE(rectangle_test)
{
    TestRectangle(10, 10, 5, 5);
    TestRectangle(10, 10, -5, 5);
    TestRectangle(10, 10, 5, -5);
    TestRectangle(10, 10, -5, -5);
    TestRectangle(10, 10, 0, 0);
}

BOOST_AUTO_TEST_SUITE_END()
