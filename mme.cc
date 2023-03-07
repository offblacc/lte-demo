/*
=== Gates ===
0: eNodeB
1: sgw
2: hss

=== Message kinds ===
0: Request auth and subscription data from HSS
1: Request session from SGW
2: Attach accept/Initial context setup
3: Modify bearer request to SGW
*/

#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;

class MME : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

   private:
    // create a vector of IDs of UE that are awaiting authentication
    std::vector<int> awaiting_authentication;
    std::map<int, std::string> ue_subscriptions;
};

// The module class needs to be registered with OMNeT++
Define_Module(MME);

void MME::initialize() {
    awaiting_authentication = {};
}

void MME::handleMessage(cMessage *msg) {
    // find source
    cGate *source = msg->getArrivalGate();
    const char *sourceFullName = source->getFullName();
    if (strcmp(sourceFullName, "in[0]") == 0) {
        if (msg->getKind() == 0) {                  // got attach request, forwarding to HSS
            int ueId = msg->par("id").longValue();  // if this breaks, it is because the UE did not send the ID, should not happen
            awaiting_authentication.push_back(ueId);
            cMessage *newMsg = new cMessage("I need you to verify this device", 0);
            newMsg->addPar("id");
            newMsg->par("id").setLongValue(ueId);
            newMsg->addPar("baseId");
            newMsg->par("baseId").setLongValue(msg->par("baseId").longValue());
            EV << "MME: Asking HSS to verify UE and give me his subscription data" << endl;
            send(newMsg, "out", 2);
        } else if (msg->getKind() == 1) {
            EV << "MME: Received initial context setup from eNodeB" << endl;
        } else if (msg->getKind() == 2) {
            EV << "MME: Got attach complete, UE is now able to send uplink packets" << endl;
            // send a "modify bearer" request to the SGW
            EV << "MME: Sending modify bearer request to SGW" << endl;
            cMessage *newMsg = new cMessage("Modify Bearer", 3);
            send(newMsg, "out", 1);
        } else {
            EV << "MME: Received unknown message from eNodeB, ignoring" << endl;
        }
    } else if (strcmp(sourceFullName, "in[2]") == 0) {  // HSS
        if (msg->getKind() == 0) {
            EV << "MME: Received subscription data from HSS" << endl;
            int ueId = msg->par("id").longValue();
            bool found = false;
            for (int i = 0; i < awaiting_authentication.size() || !found; i++) {
                if (awaiting_authentication[i] != ueId) continue;
                found = true;
                awaiting_authentication.erase(awaiting_authentication.begin() + i);
                break;
            }
            if (!found) {
                EV << "MME: Received a response from HSS for unknown UE, ignoring" << endl;
                return;
            }
            std::string subscription = msg->par("userData").stringValue();
            // The MME sends a "Create Session" request message to the SGW.
            cMessage *newMsg = new cMessage("Create Session", 1);
            newMsg->addPar("ueId");
            newMsg->par("ueId").setLongValue(ueId);
            EV << "MME: Sending Create Session request to SGW" << endl;
            send(newMsg, "out", 1);
        } else {
            EV << "MME: Received unknown message from HSS, ignoring" << endl;
        }
    } else if (strcmp(sourceFullName, "in[1]") == 0) {  // SGW
        if (msg->getKind() == 0) {
            EV << "MME: Received session data from SGW" << endl;
            int ueId = msg->par("ueId").longValue();
            EV << "MME: got IP adress for UE " << ueId << ": " << msg->par("ip").stringValue() << endl;
            // The MME sends the "Attach Accept/Initial Context Setup" request
            // message to the eNodeB/BS.
            cMessage *newMsg = new cMessage("Attach Accept/Initial Context Setup", 2);
            newMsg->addPar("ueId");
            newMsg->par("ueId").setLongValue(ueId);
            EV << "MME: Sending Attach Accept/Initial Context Setup request to eNodeB" << endl;
            send(newMsg, "out", 0);
        } else if (msg->getKind() == 1) {
            EV << "MME: Received modify bearer response from SGW" << endl;
            EV << "[global] Now UE should be able to start recieving downlink packets" << endl;
        } else {
            EV << "MME: Received unknown message from SGW, ignoring" << endl;
            EV << msg->getKind() << endl;
        }
    }

    else {
        EV << "MME: Received unknown message from unknown source, ignoring" << endl;
    }
}
