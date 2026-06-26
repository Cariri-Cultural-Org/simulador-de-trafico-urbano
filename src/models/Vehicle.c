#include "Vehicle.h"

void vehicle_init(Vehicle *vehicle, int id, int velocity, Road *road, int road_cell_index, void *city_map)
{
    vehicle->id = id;
    vehicle->velocity = velocity;
    vehicle->current_road = road;
    vehicle->road_cell_index = road_cell_index;
    vehicle->row = 0;
    vehicle->column = 0;
    vehicle->active = 1;
    vehicle->city_map = city_map;
}
