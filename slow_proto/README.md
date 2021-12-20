# Why is protobuf slow for writing large chunks of data?

In this demo, `person->set_data` is much slower than memcpy. Why is that? My understanding is that protobuf for a large sequence of bytes should just be a copy of data.

Question on Stack Overflow: https://stackoverflow.com/questions/70428781/why-is-protobuf-5-10x-slower-than-memcpy-for-a-list-of-bytes
