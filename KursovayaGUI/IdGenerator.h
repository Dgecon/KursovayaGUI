#ifndef IDGENERATOR_H
#define IDGENERATOR_H

class IdGenerator {
private:
    static int nextClientId;
    static int nextRoomId;
    static int nextBookingId;

public:
    static int generateClientId();
    static int generateRoomId();
    static int generateBookingId();

    // Getters for current next ids (needed for persistence)
    static int getNextClientId() { return nextClientId; }
    static int getNextRoomId() { return nextRoomId; }
    static int getNextBookingId() { return nextBookingId; }

    // Setter to initialize next ids from loaded data
    static void setNextIds(int clientId, int roomId, int bookingId) {
        nextClientId = clientId;
        nextRoomId = roomId;
        nextBookingId = bookingId;
    }
};

#endif