@0x8b03f59616e377c8;

interface Synchro {
    struct Maybe(T) {
        union {
            nothing @0 :Void;
            value   @1 :T;
        }
    }

    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct Item {
        direction @0 :Direction;
    }

    interface ItemSink {
        done @0 ();
        sendItem @1 (item :Item) -> ();
    }

    interface ShipCallback {
        sendSink @0 (username :Text) -> (ship :ItemSink);
    }

    ping @0 ();
    seed @1 () -> (seed :UInt64);
    join @2 (username :Text, other :Maybe(Synchro), shipCallback :ShipCallback) -> (itemSink :ItemSink);
    waitScope @3 (scope :UInt64);
}
