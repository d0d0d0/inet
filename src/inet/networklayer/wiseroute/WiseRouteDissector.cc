//
// Copyright (C) 2018 OpenSim Ltd.
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
// @author: Zoltan Bojthe
//

#include "inet/networklayer/wiseroute/WiseRouteHeader_m.h"
#include "inet/common/packet/dissector/ProtocolDissectorRegistry.h"
#include "inet/networklayer/wiseroute/WiseRouteDissector.h"


namespace inet {

Register_Protocol_Dissector(&Protocol::wiseroute, WiseRouteDissector);

void WiseRouteDissector::dissect(Packet *packet, ICallback& callback) const
{
    auto header = packet->popHeader<WiseRouteHeader>();
    auto trailerPopOffset = packet->getTrailerPopOffset();
    auto payloadEndOffset = packet->getHeaderPopOffset() + header->getPayloadLengthField();
    callback.startProtocolDataUnit(&Protocol::wiseroute);
    callback.visitChunk(header, &Protocol::wiseroute);
    packet->setTrailerPopOffset(payloadEndOffset);
    callback.dissectPacket(packet, header->getProtocol());
    ASSERT(packet->getDataLength() == B(0));
    packet->setHeaderPopOffset(payloadEndOffset);
    packet->setTrailerPopOffset(trailerPopOffset);
    callback.endProtocolDataUnit(&Protocol::wiseroute);
}

} // namespace inet

