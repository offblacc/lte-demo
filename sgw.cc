/*
=== Gates ===
0: eNodeB
1: mme
2: pcrf
3: pgw

=== Message kinds ===
0: Forwarding attach request from MME
1: Modify bearer reponse
*/
#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;

class SGW : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(SGW);

void SGW::initialize() {}

void SGW::handleMessage(cMessage *msg) {
    // if source is mme
    cGate *source = msg->getArrivalGate();

    if (strcmp(source->getFullName(), "in[1]") == 0) {
        switch (msg->getKind()) {
            case 1:  // got attach request, forwarding to PGW
                EV << "SGW: Forwarding attach request from MME to PGW" << endl;
                msg->setKind(0);
                send(msg, "out", 3);
                break;
            case 3: {
                EV << "SGW: Got back attach complete from MME, responding w Modify Bearer response message" << endl;
                cMessage *response = new cMessage("Modify Bearer Response", 1);
                send(response, "out", 1);
                break;
            }
            default:
                EV << "SGW: Received unknown message from MME, ignoring" << endl;
                break;
        }
    } else if (strcmp(source->getFullName(), "in[3]") == 0) {
        switch (msg->getKind()) {
            case 0:  // got attach request, forwarding to PGW
                EV << "SGW: Got back attach request response from PGW, returning to originating MME" << endl;
                msg->setKind(0);
                send(msg, "out", 1);

            default:
                EV << "SGW: Received unknown message from PGW, ignoring" << endl;
                break;
        }
    }
}
