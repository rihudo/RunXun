#include "record_entry.hpp"

ChatRecordEntry::ChatRecordEntry() : is_self(false)
{}

ChatRecordEntry::ChatRecordEntry(ChatRecordEntry&& other) : is_self(other.is_self), info(std::move(other.info))
{}