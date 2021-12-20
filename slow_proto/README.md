# Why is protobuf slow for writing large chunks of data?

In this demo, `person->set_data` is much slower than memcpy. Why is that? My understanding is that protobuf for a large sequence of bytes should just be a copy of data.
