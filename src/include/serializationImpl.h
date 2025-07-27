#pragma once

#include "serializationUtils.h"
#include "models.h"

// Using the Model namespace
using namespace Model;

void DateTimeSerializer::serialize(std::ofstream& out, const DateTime& dt) {
    StringSerializer::serialize(out, dt.toString());
}

void DateTimeSerializer::deserialize(std::ifstream& in, DateTime& dt) {
    dt = DateTime(StringSerializer::deserialize(in));
}
