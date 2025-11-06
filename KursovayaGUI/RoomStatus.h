#pragma once

#include <string>

enum class RoomStatus
{
	AVAILABLE,
	BOOKED,
	OCCUPIED,
	MAINTENANCE,
	CLEANING
};

inline std::string RoomStatusToString(RoomStatus status)
{
	switch (status)
	{
	case RoomStatus::AVAILABLE:    return u8"Доступна";
	case RoomStatus::BOOKED: 	   return u8"Забронирована";
	case RoomStatus::OCCUPIED:     return u8"Занята";
	case RoomStatus::MAINTENANCE:  return u8"На обслуживании";
	case RoomStatus::CLEANING:     return u8"Уборка";
	default:                       return u8"Неизвестно";
	}
}

