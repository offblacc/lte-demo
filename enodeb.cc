/*
=== Gates ===
0: ue
1: mme
2: sgw

=== Message kinds ===
0: Forwarded attach request from UE
1: Initial Context reponse message to MME
2: Forwarded attach complete from UE
*/

#include <omnetpp.h>

#include <string>

using namespace omnetpp;

class ENodeB : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

   private:
    int baseId = 996;  // the unique ID, for this simulation purposes it is hardcoded, just a random number
                       // it just matters its some unique identifyer
};

// The module class needs to be registered with OMNeT++
Define_Module(ENodeB);

void ENodeB::initialize() {}

void ENodeB::handleMessage(cMessage *msg) {
    // find out source
    cGate *source = msg->getArrivalGate();
    if (strcmp(source->getFullName(), "in[0]") == 0) {
        switch (msg->getKind()) {
            case 0:  // got attach request, forwarding to MME
                msg->addPar("baseId");
                msg->par("baseId").setLongValue(baseId);
                EV << "eNodeB: Forwarding attach request from UE to MME" << endl;
                send(msg, "out", 1);
                break;

            case 1:
                EV << "eNodeB: Got back attach complete from UE, forwarding to MME" << endl;
                msg->setKind(2);
                send(msg, "out", 1);
                break;

            default:
                EV << "eNodeB: Received unknown message from UE, ignoring" << endl;
                break;
        }
    } else if (strcmp(source->getFullName(), "in[1]") == 0) {
        switch (msg->getKind()) {
            case 2:
                EV << "eNodeB: Got back attach request response from MME" << endl;
                EV << "[global] Reconfiguring radio bearer between eNodeB and UE based on parameters received from MME" << endl;
                // send "initial context" reponse message to MME
                cMessage *msg2 = new cMessage("Initial Context reponse message to MME", 1);
                send(msg2, "out", 1);
                // send to UE to reconfigure radio bearer
                cMessage *msg1 = new cMessage("Reconfigured radio bearer between eNodeB and UE", 0);
                send(msg1, "out", 0);
                break;
        }
    }
}
