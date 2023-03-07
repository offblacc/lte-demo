/*
=== Gates ===
0: MME

=== Message kinds ===
0: Auth request response with user data if authenticated
*/

#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;

class HSS : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

   private:
    virtual void updateLocation(int &id, int &location);
    virtual bool authenticate(int &id);
    std::map<int, int> userLocations = {
        {657, 333},
        {422, 544},
        {123, 123},
        {456, 132},
        {789, 646},
        {321, 321},
        {654, 654},
        {987, 987},
        {111, 111},
        {222, 222},
        {333, 333},
        {444, 444},
        {555, 555},
        {666, 666},
        {777, 777},
        {888, 888},
        {999, 999},
    };  // some initial, imaginary previous location values
};

// The module class needs to be registered with OMNeT++
Define_Module(HSS);

void HSS::initialize() {}

void HSS::handleMessage(cMessage *msg) {
    int id = msg->par("id").longValue();
    int location = msg->par("baseId").longValue();
    delete msg;
    cMessage *retMsg = new cMessage("Auth request response", 0);
    retMsg->addPar("id");
    retMsg->par("id").setLongValue(id);
    bool auth = true;
    retMsg->addPar("auth");

    if (!authenticate(id)) {
        EV << "HSS: UE with ID " << id << " authentication failed" << endl;
        auth = false;
    }
    retMsg->par("auth").setBoolValue(auth);

    if (auth) {
        updateLocation(id, location);
        retMsg->addPar("userData");
        retMsg->par("userData").setStringValue("Just some user subscription data in string representation");
    }
    if (auth)
        EV << "HSS: Sending positive auth request response with user data to MME" << endl;
    else
        EV << "HSS: Sending negative auth request response" << endl;
    send(retMsg, "out");
}

/**
 * @brief Update the location of a user
 *
 * This function updates the location of a user with a given ID in the system.
 *
 * @param id The ID of the user to update the location of
 * @param location The new location of the user
 */
void HSS::updateLocation(int &id, int &location) {
    EV << "HSS: Updating location of UE with ID " << id << " from " << userLocations[id] << " to " << location << endl;
    userLocations[id] = location;
}

/**
 * @brief Authenticate a user
 *
 * This function authenticates a user with a given ID in the system.
 * Implemented to do nothing, just log, but any authentication logic can be implemented here.
 *
 * @param id The ID of the user to authenticate
 * @return true if the user is authenticated, false otherwise
 */
bool HSS::authenticate(int &id) {
    EV << "HSS: Authenticating UE with ID " << id << endl;
    EV << "HSS: UE with ID " << id << " authenticated" << endl;
    return true;
}
