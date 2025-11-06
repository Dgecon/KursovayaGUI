#include "IdGenerator.h"

int IdGenerator::nextClientId = 1;
int IdGenerator::nextRoomId = 1;
int IdGenerator::nextBookingId = 1;

int IdGenerator::generateClientId() {
    return nextClientId++;
}

int IdGenerator::generateRoomId() {
    return nextRoomId++;
}

int IdGenerator::generateBookingId() {
    return nextBookingId++;
}