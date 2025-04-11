#ifndef TRAVELPLANNER_INCLUDED
#define TRAVELPLANNER_INCLUDED

#include "provided.h"
#include <limits>
#include <queue>
#include <unordered_set>
#include <functional>

class TravelPlanner : public TravelPlannerBase
{
public:
    TravelPlanner(const FlightManagerBase& flight_manager, const AirportDB& airport_db) : 
    TravelPlannerBase(flight_manager, airport_db) {}

    ~TravelPlanner() override = default;


    bool plan_travel(std::string source_airport, std::string destination_airport,
        int start_time, Itinerary& itinerary) const override {

        itinerary.source_airport = source_airport;
        itinerary.destination_airport = destination_airport;
        itinerary.flights.clear();
        itinerary.total_duration = std::numeric_limits<int>::max();

        struct PathNode {
            Itinerary itinerary;
            int last_arrival_time;
            int total_duration;
            std::unordered_set<std::string> visited;

        };

        auto compare = [](const PathNode& x, const PathNode& y) {
            return x.total_duration > y.total_duration;
        };


        std::priority_queue<PathNode, std::vector<PathNode>, decltype(compare)> q(compare);


        PathNode start_node;
        start_node.itinerary = itinerary;
        start_node.last_arrival_time = start_time;
        start_node.total_duration = 0;
        start_node.visited.insert(source_airport);


        q.push(start_node);

        std::unordered_map<std::string, int> best_time;
        best_time[source_airport] = start_time;

        while (!q.empty()) {
            PathNode current = q.top();
            q.pop();

            std::string current_airport = current.itinerary.flights.empty()
                ? source_airport
                : current.itinerary.flights.back().destination_airport;

            int earliest_depart = current.last_arrival_time;
            int latest_depart = current.last_arrival_time + get_max_layover();

            if (current.itinerary.flights.empty()) {
                earliest_depart = std::max(earliest_depart, (int)start_time);
                latest_depart = std::min(latest_depart, start_time + get_max_layover());
            }


            auto flights = get_flight_manager().find_flights(current_airport, earliest_depart, latest_depart);



            for (const auto& flight : flights) {
                
                if (!preferred_airlines.empty() && preferred_airlines.find(flight.airline) == preferred_airlines.end()) {
                    continue;
                }

                if (!current.itinerary.flights.empty()) {
                    if (flight.departure_time < current.last_arrival_time + get_min_connection_time()) {
                        continue;
                    }
                }


                int arrival_time = flight.departure_time + flight.duration_sec;

                if (arrival_time - start_time > get_max_duration()) {
                    continue;
                }

                if (!current.itinerary.flights.empty()) {
                    int layover_time = flight.departure_time - current.last_arrival_time;
                    if (layover_time > get_max_layover()) {
                        continue;
                    }
                }


                if (current.visited.count(flight.destination_airport) > 0) {
                    continue;
                }

                Itinerary new_itinerary = current.itinerary;
                new_itinerary.flights.push_back(flight);
                new_itinerary.total_duration = arrival_time - start_time;

                if (flight.destination_airport == destination_airport) {
                    if (new_itinerary.total_duration < itinerary.total_duration) {
                        itinerary = new_itinerary;
                    }
                    continue; 
                }

                PathNode next_node;
                next_node.itinerary = new_itinerary;
                next_node.last_arrival_time = arrival_time;
                next_node.total_duration = new_itinerary.total_duration;
                next_node.visited = current.visited;
                next_node.visited.insert(flight.destination_airport);

                auto it = best_time.find(flight.destination_airport);
                if (it == best_time.end() || it->second > arrival_time) {
                    best_time[flight.destination_airport] = arrival_time;
                    q.push(next_node);

                }

            }
        }

        return itinerary.total_duration != std::numeric_limits<int>::max();

    }
    void add_preferred_airline(std::string airline) override {
        preferred_airlines.insert(airline);

    }
private:
    std::unordered_set<std::string> preferred_airlines;
};

#endif