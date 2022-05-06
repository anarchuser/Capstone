@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct Position {
        x @0 :Float32;
        y @1 :Float32;
    }

    interface DirectionCallback {
        sendDirection @0 (direction :Direction) -> stream;
        done @1 ();
    }

    struct Maybe(T) {
        union {
            nothing @0 :Void;
            value @1 :T;
        }
    }

    connect @0 () -> ();
    streamDirections @1 (client :Maybe(Synchro)) -> (callback :DirectionCallback);
    randomSeed @2 () -> (seed :UInt64);
}
