/*
=== Gates ===
0: sgw
1: pcrf
2: internet
*/
#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;

class PGW : public cSimpleModule {
   protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    // map of ue id to ip address
    std::map<int, std::string> ueIdToIp;

   private:
    virtual void generateIp(char *buffer);
    virtual void createPDNConnecton();
};

// The module class needs to be registered with OMNeT++
Define_Module(PGW);

void PGW::initialize() {}

void PGW::handleMessage(cMessage *msg) {
    cGate *source = msg->getArrivalGate();
    if (strcmp(source->getFullName(), "in[0]") == 0) {
        switch (msg->getKind()) {
            case 0: {  // got attach request
                EV << "PGW: Received attach request from SGW" << endl;
                char ip[16] = {0};
                generateIp(ip);
                ueIdToIp[msg->par("ueId").longValue()] = ip;
                cMessage *ipMsg = new cMessage("Create session response");
                ipMsg->addPar("ip");
                ipMsg->par("ip").setStringValue(ip);
                ipMsg->addPar("ueId");
                ipMsg->par("ueId").setLongValue(msg->par("ueId").longValue());
                createPDNConnecton();
                EV << "PGW: Create session response" << endl;
                send(ipMsg, "out", 0);
                break;
            }
            default:
                EV << "PGW: Received unknown message from SGW, ignoring" << endl;
                break;
        }
    }
}

void PGW::generateIp(char *buffer) {
    srand(time(0));
    sprintf(buffer, "%d.%d.%d.%d",
            rand() % 256,
            rand() % 256,
            rand() % 256,
            rand() % 256);
}

void PGW::createPDNConnecton() {
    EV << "PGW: Creating PDN connection" << endl;
    return;
}
