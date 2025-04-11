#ifndef FLIGHTMANAGER_INCLUDED
#define FLIGHTMANAGER_INCLUDED

#include "provided.h"
#include "bstset.h"
#include <map>
#include <fstream>
#include <sstream>

inline bool operator <(const FlightSegment& lhs, const FlightSegment& rhs) {
	if (lhs.departure_time < rhs.departure_time) return true;
	if (lhs.departure_time > rhs.departure_time) return false;

	if (lhs.duration_sec < rhs.duration_sec) return true;
	if (lhs.duration_sec > rhs.duration_sec) return false;

	if (lhs.airline < rhs.airline) return true;
	if (lhs.airline > rhs.airline) return false;

	return false;
}

inline bool operator>(const FlightSegment& lhs, const FlightSegment& rhs) {
	return rhs < lhs;
}

inline bool operator==(const FlightSegment& lhs, const FlightSegment& rhs)
{
	return lhs.departure_time == rhs.departure_time &&
		lhs.flight_no == rhs.flight_no &&
		lhs.airline == rhs.airline &&
		lhs.source_airport == rhs.source_airport &&
		lhs.destination_airport == rhs.destination_airport &&
		lhs.duration_sec == rhs.duration_sec;
}

inline bool operator >=(const FlightSegment& lhs, const FlightSegment& rhs) {
	return !(lhs < rhs);
}

inline bool operator<=(const FlightSegment& lhs, const FlightSegment& rhs) {
    return !(lhs > rhs);
}

class FlightManager : public FlightManagerBase
{
private:
	std::map<std::string, BSTSet<FlightSegment>> flightsAirport;

public:
	FlightManager() {}
	~FlightManager() {}

	bool load_flight_data(std::string filename) override {


		std::ifstream file(filename);
		if (!file) {
			return false;
		}

		std::string line;

		while (std::getline(file, line)) {
			std::istringstream ss(line);
			std::string airline, source, destination;
			int flightNum, departureTime, arrivalTime, duration;

			if (std::getline(ss, airline, ',') &&
				ss >> flightNum && ss.ignore() && 
				std::getline(ss, source, ',') &&
				std::getline(ss,destination, ',') &&
				ss >> departureTime && ss.ignore() &&
				ss >> arrivalTime && ss.ignore() &&
				ss >> duration){
				
				FlightSegment flight(airline, flightNum, source, destination, departureTime, duration);
				flightsAirport[source].insert(flight);
			
			}

		}
		return true;

	}

	std::vector<FlightSegment> find_flights(std::string source_airport, int start_time, int end_time) const override {
		std::vector<FlightSegment> result;
		auto it = flightsAirport.find(source_airport);
		if (it == flightsAirport.end()) {
			return result;
		}

		auto flight_it = it->second.find_first_not_smaller(FlightSegment("", 0, "", "", start_time, 0));
		const FlightSegment* p;
		while ((p = flight_it.get_and_advance()) != nullptr && p->departure_time < end_time) {
			result.push_back(*p);
		}


		return result;
	}

};


#endif 