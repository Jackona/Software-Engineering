
#include <boost/test/unit_test.hpp>

#include "../../headers/types.h"


#include "../../headers/waypoints.h"
#include "../../headers/analysis/analysis-track.h"
#include "../../headers/gridworld/gridworld-track.h"

/*
#include "types.h"
#include "waypoints.h"
#include "analysis-track.h"
#include "gridworld-track.h"
*/


/* The mean duration of the travelling periods within the track.
    * (A travelling period is the sum of all adjacent non-rests.)
    * Throws a std::domain_error if the track contains no travelling periods.
    * Throws a std::domain_error if the time elapsed between any two adjacent points during a
    * travelling period is negative (but negative time while resting will not trigger an exception).
*/

/*
    Testing the averageTravellingPeriod() function 
    This function should return the the mean/average duration of the travelling period within the track
    The function should also check for two errors 
        - when track contains no travelling period
        - when elapsed time between two adjacent points during a travelling period is negative
    
    modify the timeStamps vector
    - As a minor test case, I would also test for cases when the sum of all adjacent non-rest have
    - a sum of 0 or a very high number
    - In order to compute the mean duration of the travelling period, the occurance of travelling period 
    - and the total travelling time is required. Therefore, a test case where one of/both variables are
    - 0 would also be an edge case
    - include some typical cases just as appropriate amount of travelling time and an appropriate 
    - number of travlling occured
*/

/*
    CLASSES AND STRUCKS
    TimeStamp(std::chrono::system_clock::time_point)
    RoutePoint(position:Position and name:String)
    Position(longitude:degrees, latitude:degrees, elevation:metres)
    TrackPoint(position:Position, name:String, dateTime:tm)
*/

using namespace GPS;

BOOST_AUTO_TEST_SUITE( Track_averageTravellingPeriod )

const GridWorld::Model gridworld; //using default M location(Pontianak) and gridUnit(10000)

BOOST_AUTO_TEST_CASE( NoRestingCase )
{
    const std::vector<TrackPoint> trackPoints = GridWorld::Track("A3F3G1L5M").toTrackPoints();
    const metres restingRange = 10000 * 0.01; //100
    const Analysis::Track track(trackPoints, restingRange);

    const std::chrono::seconds expected_ATP{3};
    
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();

    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

//Very basic test case
BOOST_AUTO_TEST_CASE( TravelOnceCase )
{
    const std::vector<TrackPoint> trackPoints = GridWorld::Track("A9Y").toTrackPoints();
    const Analysis::Track track { trackPoints, 10000 }; //restingRange = 10000
    const std::chrono::seconds expected_ATP {9}; // 9/1
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

//Travelling the corners(AYEU)
BOOST_AUTO_TEST_CASE( TravelCornersCase )
{
    const Analysis::Track track { GridWorld::Track("A5Y3E2U").toTrackPoints(), 50000 };
    const std::chrono::seconds expected_ATP {5}; // (5+3+2)/2 = 5 only two travel(A to Y and E to U)
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( ComplexCase )
{
    const Analysis::Track track { GridWorld::Track("A1B1H2M1R1S4P6E2D").toTrackPoints(), 12000 }; 
    //travelling diagonal(EX: A to G) is a NOT a rest
    //travelling one sideways and one up/down is a rest(EX: A to F or D to C)
    const std::chrono::seconds expected_ATP {6};
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( SecondComplexCase )
{
    const Analysis::Track track { GridWorld::Track("A2C2M4E6Y1V").toTrackPoints(), 20001};
    const std::chrono::seconds expected_ATP {5};
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( ThirdComplexCase )
{
    const Analysis::Track track { GridWorld::Track("G4M6O1T7U2P4K8E").toTrackPoints(), 14000};
    const std::chrono::seconds expected_ATP {8};
    //TTRTRRT
    //4 6 1 7 2 4 8 = 32
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( FourthComplexCase )
{
    const Analysis::Track track { GridWorld::Track("P1V2Y5E3A3M1N1O").toTrackPoints(), 20001};
    const std::chrono::seconds expected_ATP {4};
    //R T T T T R R
    //1 2 5 3 3 1 1 = 16
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}


                    /**************** SPECIAL TEST CASES *****************/
BOOST_AUTO_TEST_CASE( StayAtSamePointCase )
{
    const Analysis::Track track { GridWorld::Track("A1B4B2N1O2O2E4E1E1D").toTrackPoints(), 14000};
    const std::chrono::seconds expected_ATP {9};
    //R R T R R T R R R
    //1 4 2 1 2 2 4 1 1 = 18
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( RestingTimeNegative )
{
    const Analysis::Track track { GridWorld::Track("A-2B2D1N1X-2W").toTrackPoints(), 14000};
    const std::chrono::seconds expected_ATP {0};
    // R T T T R
    //-2 2 1 1 -2
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( TravelTimeZero ) //Is it allowed to have travel time of zero?
{
    const Analysis::Track track { GridWorld::Track("A0B0D0N0X0W").toTrackPoints(), 14000};
    const std::chrono::seconds expected_ATP {0};
    // R T T T R
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

BOOST_AUTO_TEST_CASE( LotsOfEdgeCase )
{
    const Analysis::Track track { GridWorld::Track("A-4A-2B8V2Y-1T-1O6K8E").toTrackPoints(), 14000};
    const std::chrono::seconds expected_ATP {4};
    // R R T T R R T T
    //-4 -2 8 2 -1 -1 6 8 = 16
    std::chrono::seconds calculated_ATP = track.averageTravellingPeriod();
    BOOST_CHECK_EQUAL(calculated_ATP.count(), expected_ATP.count());
}

/*

BOOST_AUTO_TEST_CASE( ErrorResultCaseOne ) //No travel, all rest
{
    const Analysis::Track track { GridWorld::Track("A1B1G1H1I1J").toTrackPoints(), 14000}; //no travel
    //R R R R R
    //1 1 1 1 1 = 5
    BOOST_CHECK_EQUAL(std::domain_error("Cannot find travelling periods in an empty track."), track.averageTravellingPeriod());
}

BOOST_AUTO_TEST_CASE( ErrorResultCaseTwo ) //Travel time is negative
{
    const Analysis::Track track { GridWorld::Track("A1B-1G-2H1I1J").toTrackPoints(), 1000}; 
    //R R R R R 
    //1 -1 -2 1 1 = 0
    BOOST_CHECK_EQUAL(std::domain_error("Cannot find travelling periods in an empty track."), track.averageTravellingPeriod());
}

*/

BOOST_AUTO_TEST_SUITE_END()