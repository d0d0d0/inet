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

#include "inet/linklayer/csmaca/CsmaCaMacDissector.h"

#include "inet/common/ProtocolGroup.h"
#include "inet/common/packet/dissector/ProtocolDissectorRegistry.h"
#include "inet/linklayer/csmaca/CsmaCaMacHeader_m.h"


namespace inet {

Register_Protocol_Dissector(&Protocol::csmacamac, CsmaCaMacDissector);

void CsmaCaMacDissector::dissect(Packet *packet, ICallback& callback) const
{
    auto header = packet->popHeader<CsmaCaMacHeader>();
    auto trailer = packet->popTrailer<CsmaCaMacTrailer>(B(4));
    callback.startProtocolDataUnit(&Protocol::csmacamac);
    callback.visitChunk(header, &Protocol::csmacamac);
    if (auto dataHeader = dynamicPtrCast<const CsmaCaMacDataHeader>(header)) {
        auto payloadProtocol = ProtocolGroup::ethertype.getProtocol(dataHeader->getNetworkProtocol());
        callback.dissectPacket(packet, payloadProtocol);
    }
    ASSERT(packet->getDataLength() == B(0));
    callback.visitChunk(trailer, &Protocol::csmacamac);
    callback.endProtocolDataUnit(&Protocol::csmacamac);
}

} // namespace inet

