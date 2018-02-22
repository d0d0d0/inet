//
// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_WIRELESSSIGNAL_H
#define __INET_WIRELESSSIGNAL_H

#include "inet/physicallayer/common/packetlevel/Signal.h"
#include "inet/physicallayer/contract/packetlevel/IWirelessSignal.h"

namespace inet {

namespace physicallayer {

class INET_API WirelessSignal : public Signal, public IWirelessSignal
{
  protected:
    const int transmissionId;
    const ITransmission *transmission;
    const IRadioMedium *radioMedium;
    mutable const IRadio *receiver = nullptr;
    mutable const IArrival *arrival = nullptr;
    mutable const IListening *listening = nullptr;
    mutable const IReception *reception = nullptr;

  protected:
    bool isDup() const { return transmission == nullptr; }

  public:
    WirelessSignal(const ITransmission *transmission);
    WirelessSignal(const WirelessSignal& other);

    virtual WirelessSignal *dup() const override { return new WirelessSignal(*this); }

    virtual std::ostream& printToStream(std::ostream& stream, int level) const override;

    virtual const IRadio *getTransmitter() const;
    virtual const IRadio *getReceiver() const;
    virtual const ITransmission *getTransmission() const override;
    virtual const IArrival *getArrival() const override;
    virtual const IListening *getListening() const override;
    virtual const IReception *getReception() const override;
};

} // namespace physicallayer

} // namespace inet

#endif // ifndef __INET_WIRELESSSIGNAL_H

