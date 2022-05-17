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

    struct Spaceship {
        username @0 :Text;
        position @1 :Vector;
        velocity @2 :Vector;
        angle    @3 :Float32;
    }

    struct Vector {
        x @0 :Float32;
        y @1 :Float32;
    }

    interface ItemSink {
        done @0 ();
        sendItem @1 (item :Item) -> ();
    }

    interface ShipCallback {
        sendSink @0 (spaceship :Spaceship) -> (sink :ItemSink);
        getSpaceship @1 () -> (spaceship :Spaceship);
        setSpaceship @2 (spaceship :Spaceship) -> ();
    }

    ping @0 ();
    seed @1 () -> (seed :UInt64);
    join @2 (spaceship :Spaceship, other :Maybe(Synchro), shipCallback :ShipCallback) -> (itemSink :ItemSink);
}
