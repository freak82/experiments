import std.stdio, std.string, std.algorithm, std.conv, std.typecons;

void main()
{
    auto log = File("shadow.log");
    auto lines = log.byLine();

    long[string] bytes;
    long[string] resps;
    foreach (line; lines) {
        auto split = line.split(" | ");
        if (split.length > 5 && split[5] == "HIT") {
            auto key = to!string(split[6]);
            bytes[key] += to!long(split[7]);
            resps[key] += 1;
        }
    }

    alias Entry = 
        Tuple!(string, "host", long, "bytes", long, "reqs", long, "avgSize");
    Entry[] data;
    foreach (key; bytes.keys()) {
        long r = resps[key];
        long b = bytes[key];
        long a = b / r;
        data ~= Entry(key, b, r, a);
    }
    sort!((l, r) => l.bytes > r.bytes)(data);

    writeln("Host,Bytes,Requests,AverageSize");
    foreach (ref e; data) {
        writeln(e.host, ',', e.bytes, ',', e.reqs, ',', e.avgSize);
    }
}
