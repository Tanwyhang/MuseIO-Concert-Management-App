#ifndef SERIALIZATION_IMPL_H
#define SERIALIZATION_IMPL_H

#include "serializationUtils.h"
#include "models.h"

namespace MuseIO {

// Implementation of DateTimeSerializer
void DateTimeSerializer::serialize(std::ofstream& out, const DateTime& dt) {
    StringSerializer::serialize(out, dt.toString());
}

void DateTimeSerializer::deserialize(std::ifstream& in, DateTime& dt) {
    dt = DateTime(StringSerializer::deserialize(in));
}

} // namespace MuseIO

#endif // SERIALIZATION_IMPL_H
