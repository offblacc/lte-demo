/*
=== Gates ===
0: eNodeB

=== Message kinds ===
0: initial message to eNodeB, attach request to the BS
1: Attach complete
*/

#include <omnetpp.h>
#include <string.h>

#include <iostream>

using namespace omnetpp;

class UE : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

   private:
    int id = 657;  // the unique ID, for this simulation purposes it is hardcoded, just a random number
                   // it just matters its some unique identifyer
};

// The module class needs to be registered with OMNeT++
Define_Module(UE);

void UE::initialize() {
    cMessage *msg = new cMessage("Attach request to the BS", 0);
    msg->addPar("id");
    msg->par("id").setLongValue(id);
    EV << "UE: Sending initial message to eNodeB, attach request to the BS" << endl;
    send(msg, "out");
}

void UE::handleMessage(cMessage *msg) {
    // if kind == 0
    switch (msg->getKind()) {
        case 0:
            EV << "UE: Reconfigured radio bearer done" << endl;
            // The UE sends a "Direct Transfer" message, which includes the
            //"Attach Complete" signal, to the eNodeB.
            EV << "UE: Sending attach complete to eNodeB" << endl;
            cMessage *msg1 = new cMessage("Attach Complete", 1);
            send(msg1, "out");
            break;
    }
}
