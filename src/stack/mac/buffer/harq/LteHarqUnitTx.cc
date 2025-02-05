//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "stack/mac/buffer/harq/LteHarqUnitTx.h"
#include "stack/mac/layer/LteMacEnb.h"
#include <omnetpp.h>

using namespace omnetpp;

LteHarqUnitTx::LteHarqUnitTx(unsigned char acid, Codeword cw,
    LteMacBase *macOwner, LteMacBase *dstMac)
{
    pdu_ = nullptr;
    pduId_ = -1;
    acid_ = acid;
    cw_ = cw;
    transmissions_ = 0;
    txTime_ = 0;
    status_ = TXHARQ_PDU_EMPTY;
    macOwner_ = macOwner;
    dstMac_ = dstMac;
    maxHarqRtx_ = macOwner->par("maxHarqRtx");



    HarqErrorRateDlSum = 0;
    HarqErrorRateDlCount = 0;
    HarqErrorRateUlSum = 0;
    HarqErrorRateUlCount = 0;

    Harq1Sum = 0;
    Harq1Count = 0;
    Harq2Sum = 0;
    Harq2Count = 0;
    Harq3Sum = 0;
    Harq3Count = 0;
    Harq4Sum = 0;
    Harq4Count = 0;


    harqSumLast = 0;
    harqCountLast = 0;







    if (macOwner_->getNodeType() == ENODEB || macOwner_->getNodeType() == GNODEB)
    {
        nodeB_ = macOwner_;
        macPacketLoss_ = dstMac_->registerSignal("macPacketLossDl");
        macCellPacketLoss_ = macOwner_->registerSignal("macCellPacketLossDl");
        harqErrorRate_ = dstMac_->registerSignal("harqErrorRateDl");
        harqErrorRate_1_ = dstMac_->registerSignal("harqErrorRate_1st_Dl");
        harqErrorRate_2_ = dstMac_->registerSignal("harqErrorRate_2nd_Dl");
        harqErrorRate_3_ = dstMac_->registerSignal("harqErrorRate_3rd_Dl");
        harqErrorRate_4_ = dstMac_->registerSignal("harqErrorRate_4th_Dl");
        harqTxAttempts_ = macOwner->registerSignal("harqTxAttemptsDl");
    }
    else  // UE
    {
        nodeB_ = getMacByMacNodeId(macOwner_->getMacCellId());
        if (dstMac_ == nodeB_)  // UL
        {
            macPacketLoss_ = macOwner_->registerSignal("macPacketLossUl");
            macCellPacketLoss_ = nodeB_->registerSignal("macCellPacketLossUl");
            harqErrorRate_ = macOwner_->registerSignal("harqErrorRateUl");
            harqErrorRate_1_ = macOwner_->registerSignal("harqErrorRate_1st_Ul");
            harqErrorRate_2_ = macOwner_->registerSignal("harqErrorRate_2nd_Ul");
            harqErrorRate_3_ = macOwner_->registerSignal("harqErrorRate_3rd_Ul");
            harqErrorRate_4_ = macOwner_->registerSignal("harqErrorRate_4th_Ul");
            harqTxAttempts_ = macOwner->registerSignal("harqTxAttemptsUl");
        }
        else
        {
            macPacketLoss_ = 0;
            macCellPacketLoss_ = 0;
            harqErrorRate_ = 0;
            harqErrorRate_1_ = 0;
            harqErrorRate_2_ = 0;
            harqErrorRate_3_ = 0;
            harqErrorRate_4_ = 0;
            harqTxAttempts_ = 0;
        }
    }
}

LteHarqUnitTx& LteHarqUnitTx::operator=(const LteHarqUnitTx& other)
{
    if (&other == this)
        return *this;

    pdu_ = other.pdu_;
    pduId_ = other.pduId_;
    acid_ = other.acid_;
    cw_ = other.cw_;
    transmissions_ = other.transmissions_;
    txTime_ = other.txTime_;
    status_ = other.status_;
    macOwner_ = other.macOwner_;
    dstMac_ = other.dstMac_;
    maxHarqRtx_ = other.maxHarqRtx_;

    nodeB_ = other.nodeB_;

    macPacketLoss_ = other.macPacketLoss_;
    macCellPacketLoss_ = other.macCellPacketLoss_;
    harqErrorRate_ = other.harqErrorRate_;
    harqErrorRate_1_ = other.harqErrorRate_1_;
    harqErrorRate_2_ = other.harqErrorRate_2_;
    harqErrorRate_3_ = other.harqErrorRate_3_;
    harqErrorRate_4_ = other.harqErrorRate_4_;

    macCellPacketLossD2D_ = other.macCellPacketLossD2D_;
    macPacketLossD2D_ = other.macPacketLossD2D_;
    harqErrorRateD2D_ = other.harqErrorRateD2D_;
    harqErrorRateD2D_1_ = other.harqErrorRateD2D_1_;
    harqErrorRateD2D_2_ = other.harqErrorRateD2D_2_;
    harqErrorRateD2D_3_ = other.harqErrorRateD2D_3_;
    harqErrorRateD2D_4_ = other.harqErrorRateD2D_4_;

    return *this;
}

void LteHarqUnitTx::insertPdu(Packet *pkt)
{
    if (!pkt)
        throw cRuntimeError("Trying to insert NULL macPdu pointer in harq unit");

    // cannot insert if the unit is not idle
    if (!this->isEmpty())
        throw cRuntimeError("Trying to insert macPdu in already busy harq unit");

    auto pdu = pkt->removeAtFront<LteMacPdu>();
    pdu_ = pkt;
    pduId_ = pdu->getId();
    // as unique MacPDUId the OMNET id is used (need separate member since it must not be changed by dup())
    pdu->setMacPduId(pdu->getId());
    pkt->insertAtFront(pdu);
    transmissions_ = 0;
    status_ = TXHARQ_PDU_SELECTED;
    pduLength_ = pdu_->getByteLength();
    auto lteInfo = pdu_->getTagForUpdate<UserControlInfo>();

    lteInfo->setAcid(acid_);
    Codeword cw_old = lteInfo->getCw();
    if (cw_ != cw_old)
        throw cRuntimeError("mismatch in cw settings");

    lteInfo->setCw(cw_);
}

void LteHarqUnitTx::markSelected()
{
    EV << NOW << " LteHarqUnitTx::markSelected trying to select buffer "
       << acid_ << " codeword " << cw_ << " for transmission " << endl;

    if (!(this->isReady()))
    throw cRuntimeError("ERROR acid %d codeword %d trying to select for transmission an empty buffer", acid_, cw_);

    status_ = TXHARQ_PDU_SELECTED;
}

Packet *LteHarqUnitTx::extractPdu()
{
    if (!(status_ == TXHARQ_PDU_SELECTED))
        throw cRuntimeError("Trying to extract macPdu from not selected H-ARQ unit");

    txTime_ = NOW;
    transmissions_++;
    status_ = TXHARQ_PDU_WAITING; // waiting for feedback

    auto lteInfo = pdu_->getTagForUpdate<UserControlInfo>();
    lteInfo->setTxNumber(transmissions_);
    lteInfo->setNdi((transmissions_ == 1) ? true : false);
    EV << "LteHarqUnitTx::extractPdu - ndi set to " << ((transmissions_ == 1) ? "true" : "false") << endl;

    auto extractedPdu = pdu_->dup();
    macOwner_->takeObj(extractedPdu);
    return extractedPdu;
}

bool LteHarqUnitTx::pduFeedback(HarqAcknowledgment a)
{
    EV << "LteHarqUnitTx::pduFeedback - Welcome!" << endl;
    double sample;
    bool reset = false;
    auto lteInfo = pdu_->getTag<UserControlInfo>();
    short unsigned int dir = lteInfo->getDirection();
    unsigned int ntx = transmissions_;
    if (!(status_ == TXHARQ_PDU_WAITING))
        throw cRuntimeError("Feedback sent to an H-ARQ unit not waiting for it");

    if (a == HARQACK)
    {
        // pdu_ has been sent and received correctly
        EV << "\t pdu_ has been sent and received correctly " << endl;
        resetUnit();
        reset = true;
        sample = 0;
    }
    else if (a == HARQNACK)
    {
        sample = 1;
        if (transmissions_ == (maxHarqRtx_ + 1))
        {
            // discard
            EV << NOW << " LteHarqUnitTx::pduFeedback H-ARQ process  " << (unsigned int)acid_ << " Codeword " << cw_ << " PDU "
               << pdu_->getId() << " discarded (max retransmissions reached) : " << maxHarqRtx_ << endl;

            // @author Alessandro Noferi
            // notify discard macPduId to packetFlowManager

            macOwner_->discardMacPdu(pdu_);

            resetUnit();
            reset = true;
        }
        else
        {
            // pdu_ ready for next transmission
            macOwner_->takeObj(pdu_);
            status_ = TXHARQ_PDU_BUFFERED;
            EV << NOW << " LteHarqUnitTx::pduFeedbackH-ARQ process  " << (unsigned int)acid_ << " Codeword " << cw_ << " PDU "
               << pdu_->getId() << " set for RTX " << endl;

            if (macOwner_->getNodeType() == ENODEB || macOwner_->getNodeType() == GNODEB)
            {
                // signal the MAC the need for retransmission
                check_and_cast<LteMacEnb*>(macOwner_)->signalProcessForRtx(lteInfo->getDestId(), lteInfo->getCarrierFrequency(), (Direction)lteInfo->getDirection());
            }
        }
    }
    else
    {
        throw cRuntimeError("LteHarqUnitTx::pduFeedback unknown feedback received from process %d , Codeword %d", acid_, cw_);
    }

    LteMacBase* ue;
    if (dir == DL)
    {
        ue = dstMac_;
    }
    else if (dir == UL)
    {
        ue = macOwner_;
    }
    else
    {
        throw cRuntimeError("LteHarqUnitTx::pduFeedback(): unknown direction");
    }

    // emit H-ARQ statistics




    static int sumAll = 0;
    static int countAll = 0;





    switch (ntx)
    {
        case 1:
        ue->emit(harqErrorRate_1_, sample);

        Harq1Sum += sample;
        Harq1Count ++;

        break;
        case 2:
        ue->emit(harqErrorRate_2_, sample);

        Harq2Sum += sample;
        Harq2Count ++;

        break;
        case 3:
        ue->emit(harqErrorRate_3_, sample);

        Harq3Sum += sample;
        Harq3Count ++;

        break;
        case 4:
        ue->emit(harqErrorRate_4_, sample);

        Harq4Sum += sample;
        Harq4Count ++;

        break;
        default:
        break;
    }

    ue->emit(harqErrorRate_, sample);

    if (ntx < 4){
        ue->recordHarqErrorRate(sample, (Direction)dir);

        if ((Direction)dir == DL){
            HarqErrorRateDlSum += sample;
            HarqErrorRateDlCount ++;
        }
        if ((Direction)dir == UL){
            HarqErrorRateUlSum += sample;
            HarqErrorRateUlCount ++;
            sumAll += sample;
            countAll ++;
        }
    }

    else if (ntx == 4){
        ue->recordHarqErrorRate(0, (Direction)dir);
        if ((Direction)dir == DL){
            HarqErrorRateDlCount ++;
        }
        if ((Direction)dir == UL){
            HarqErrorRateUlCount ++;
            countAll ++;
        }
    }

    if (a == HARQACK)
        ue->emit(harqTxAttempts_, ntx);

    if (reset)
    {
        ue->emit(macPacketLoss_, sample);
        nodeB_->emit(macCellPacketLoss_, sample);
    }


        std::ostringstream oss1;
            oss1 << lteInfo->getSourceId();
            std::string ueIDStr = oss1.str();

        std::ostringstream oss2;
            oss2 << lteInfo->getDestId();
            std::string DestStr = oss2.str();

        std::string combinedInfo = "HarqErrorRateDlSum = " + std::to_string(HarqErrorRateDlSum) + ", ";
        combinedInfo += "HarqErrorRateDlCount = " + std::to_string(HarqErrorRateDlCount) + ", ";
        combinedInfo += "HarqErrorRateUlSum = " + std::to_string(HarqErrorRateUlSum) + ", ";
        combinedInfo += "HarqErrorRateUlCount = " + std::to_string(HarqErrorRateUlCount) + ", ";
        combinedInfo += "Harq1Sum = " + std::to_string(Harq1Sum) + ", ";
        combinedInfo += "Harq1Count = " + std::to_string(Harq1Count) + ", ";
        combinedInfo += "Harq2Sum = " + std::to_string(Harq2Sum) + ", ";
        combinedInfo += "Harq2Count = " + std::to_string(Harq2Count) + ", ";
        combinedInfo += "Harq3Sum = " + std::to_string(Harq3Sum) + ", ";
        combinedInfo += "Harq3Count = " + std::to_string(Harq3Count) + ", ";
        combinedInfo += "Harq4Sum = " + std::to_string(Harq4Sum) + ", ";
        combinedInfo += "Harq4Count = " + std::to_string(Harq4Count) + ", ";

        std::string key = "[LteHarqUnit ";
        key += ueIDStr + "->" + DestStr;
        key += "]";

        DataStorage::setData(key, combinedInfo);






        static simtime_t lastWriteTime[8] = {0};
        static simtime_t lastWriteTimeAll = 0;
        simtime_t now = simTime();
        static std::ostringstream buffer("");

        static int row = 80;
        static int currentRow = 1;


        static int sumLastAll = 0;
        static int countLastAll = 0;

        for (int ii = 0; ii <= 7; ++ii) {
            int acid = lteInfo->getAcid();
            if (now - lastWriteTime[ii] >= 1 && acid == ii && false) {
                double nn = HarqErrorRateUlSum - harqSumLast;
                double dd = HarqErrorRateUlCount - harqCountLast;

                double harqRate = (dd > 0) ? nn / dd : 0;

                harqSumLast = HarqErrorRateUlSum;
                harqCountLast = HarqErrorRateUlCount;

                std::ofstream lossOutputFile("harq_log.txt", std::ios::app);
                if (lossOutputFile.is_open()) {
                    lossOutputFile << "Time: " << now
                                   << ", Acid: " << acid
                                   << ", HarqErrorSum: " << nn
                                   << ", HarqErrorRateUlCount: " << dd
                                   << ", SumTotal: " << HarqErrorRateUlSum
                                   << ", CountTotal: " << HarqErrorRateUlCount
                                   << ", HARQErrorRate: " << harqRate
                                   << "\n";
                    lossOutputFile.close();
                }
                lastWriteTime[ii] = std::floor(now.dbl());
            }
        }


        if (now - lastWriteTimeAll >= 1){
                double nn = sumAll - sumLastAll;
                double dd = countAll - countLastAll;

                double harqRate = (dd > 0) ? nn / dd : 0;

                sumLastAll = sumAll;
                countLastAll = countAll;

                std::ofstream lossOutputFile("harq_logw.txt", std::ios::app);
                if (lossOutputFile.is_open()) {
                lossOutputFile << "Time: " << now
                                //                   << ", Acid: " << acid
                               //<< ", HarqErrorSum: " << nn
                               //<< ", HarqErrorRateUlCount: " << dd
                                //                   << ", SumTotal: " << HarqErrorRateUlSum
                                //                   << ", CountTotal: " << HarqErrorRateUlCount
                               << ", HARQErrorRate: " << harqRate
                               << "\n";
                               lossOutputFile.close();
                }
                lastWriteTimeAll = std::floor(now.dbl());
        }

        //currentRow++;

        //if (currentRow > row) {
        //    std::ofstream lossOutputFile("harq_log.txt", std::ios::app);
        //    if (lossOutputFile.is_open()) {
        //        lossOutputFile << "end\n";
        //        lossOutputFile.close();
        //}

    return reset;
}

bool LteHarqUnitTx::isEmpty()
{
    return (status_ == TXHARQ_PDU_EMPTY);
}

bool LteHarqUnitTx::isReady()
{
    return (status_ == TXHARQ_PDU_BUFFERED);
}

bool LteHarqUnitTx::selfNack()
{
    if (status_ == TXHARQ_PDU_WAITING)
        // wrong usage, manual nack now is dangerous (a real one may arrive too)
        throw cRuntimeError("LteHarqUnitTx::selfNack(): Trying to send self NACK to a unit waiting for feedback");

    if (status_ != TXHARQ_PDU_BUFFERED)
        throw cRuntimeError("LteHarqUnitTx::selfNack(): Trying to send self NACK to an idle or selected unit");

    transmissions_++;
    txTime_ = NOW;
    bool result = this->pduFeedback(HARQNACK);
    return result;
}

void LteHarqUnitTx::dropPdu()
{
    if (status_ != TXHARQ_PDU_BUFFERED)
        throw cRuntimeError("LteHarqUnitTx::dropPdu(): H-ARQ TX unit: cannot drop pdu if state is not BUFFERED");

    resetUnit();
}

void LteHarqUnitTx::forceDropUnit()
{
    if (status_ == TXHARQ_PDU_BUFFERED || status_ == TXHARQ_PDU_SELECTED || status_ == TXHARQ_PDU_WAITING)
    {
        delete pdu_;
        pdu_ = nullptr;
    }
    resetUnit();
}

Packet *LteHarqUnitTx::getPdu()
{
    auto temp = pdu_->peekAtFront<LteMacPdu>();
    return pdu_;
}

LteHarqUnitTx::~LteHarqUnitTx()
{
    resetUnit();
}

void LteHarqUnitTx::resetUnit()
{
    transmissions_ = 0;
    pduId_ = -1;
    if(pdu_ != nullptr){
        delete pdu_;
        pdu_ = nullptr;
    }

    status_ = TXHARQ_PDU_EMPTY;
    pduLength_ = 0;
}
